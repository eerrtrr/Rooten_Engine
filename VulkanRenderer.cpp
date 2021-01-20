#include "VulkanRenderer.hpp"
#include "Platform.hpp"
#include "Logger.hpp"
#include "VulkanUtils.hpp"
#include <vector>
#include <cstring>

#define NDEBUG

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif


namespace Rooten{

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData){

        switch (messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                Logger::Error(pCallbackData->pMessage);
                break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                Logger::Warn(pCallbackData->pMessage);
                break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                Logger::Log(pCallbackData->pMessage);
                break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                Logger::Trace(pCallbackData->pMessage);
                break;

            default:
                break;
        }

        return VK_FALSE;
    }

    VulkanRenderer::VulkanRenderer(Platform* platform){
        this->platform = platform;
        Logger::Trace("Initializing Vulkan renderer...");

        VkApplicationInfo appInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
        appInfo.apiVersion = VK_API_VERSION_1_2;
        appInfo.pApplicationName = "Rooten";
        appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);

        VkInstanceCreateInfo instanceCreateInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
        instanceCreateInfo.pApplicationInfo = &appInfo;

        //Extensions (can not figure out how to use it with X11)
        std::vector<const char*> extensions;
        extensions.push_back("VK_KHR_xlib_surface");
        extensions.push_back("VK_KHR_surface");

        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

        //Validation layers
        std::vector<const char*> requiredValidationLayers = {
                "VK_LAYER_KHRONOS_validation"
        };

        //Get available layers
        unsigned int availableLayerCount = 0;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr));
        std::vector<VkLayerProperties> availableLayers(availableLayerCount);
        VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()));

        //Verify that all required layers are available
        for(unsigned int i=0; i<(unsigned int)requiredValidationLayers.size(); i++){
            bool found = false;
            for(unsigned int j=0; j<availableLayerCount; j++){
                if(strcmp(requiredValidationLayers[i], availableLayers[j].layerName) == 0){
                    found = true;
                    break;
                }
            }

            if(!found){
                Logger::Fatal("Required validation layer is missing: %s", requiredValidationLayers[i]);
                break;
            }
        }

        instanceCreateInfo.enabledLayerCount = (unsigned int)requiredValidationLayers.size();
        instanceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();

        //Create Instance
        Logger::Trace("Creating instance...");
        VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &this->instance));

        //Create debugger
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;
        debugCreateInfo.pUserData = this;

        if(enableValidationLayers){
            Logger::Trace("\tCreating debugger..");
            PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->instance, "vkCreateDebugUtilsMessengerEXT");
            VK_CHECK(func(this->instance, &debugCreateInfo, nullptr, &this->debugMessenger));
        }

        //Surface creation
        Logger::Trace("Creating surface...");
        platform->createSurface(this->instance, &this->surface);
        
        //SelectPhysicalDevice
        Logger::Trace("Selecting physical device...");
        physicalDevice = selectPhysicalDevice();

        //Create logical device
        createLogicalDevice(requiredValidationLayers);
    }


    VulkanRenderer::~VulkanRenderer(){
        if(enableValidationLayers){
            PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->instance, "vkDestroyDebugUtilsMessengerEXT");
            func(this->instance, this->debugMessenger, nullptr);
        }
        vkDestroyInstance(this->instance, nullptr);
    }




    VkPhysicalDevice VulkanRenderer::selectPhysicalDevice(){
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if(deviceCount == 0){
            Logger::Fatal("No supported physical device were found");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for(uint32_t i=0; i<deviceCount; i++){
            if(physicalDeviceMeetsRequirements(devices[i])){
                return devices[i];
            }
        }

        Logger::Fatal("No devices found which meet application requirements");
        return nullptr;     
    }


    bool VulkanRenderer::physicalDeviceMeetsRequirements(VkPhysicalDevice physicalDevice){
        int32_t graphicsQueueIndex = -1;
        int32_t presentationQueueIndex = -1;

        detectQueueFamilyIndices(physicalDevice, &graphicsQueueIndex, &presentationQueueIndex);
        VulkanSwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice);
    
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physicalDevice, &features);


        bool supportsRequirementQueueFamilies = (graphicsQueueIndex != -1) && (presentationQueueIndex != -1);
        
        //Device extension support
        uint32_t extentionCount = 0;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extentionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extentionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extentionCount, availableExtensions.data());                
   
        //Required extensions
        std::vector<const char*> requiredExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        bool success = true;
        for(uint64_t i=0; i<requiredExtensions.size(); i++){
            bool found = false;
            for(uint64_t j=0; j<extentionCount; j++){
                if(strcmp(requiredExtensions[i], availableExtensions[j].extensionName) == 0){
                    found = true;
                    break;
                }
            }

            if(!found){
                success = false;
                break;
            }
        }

        bool swapChainMeetsRequirements = false;
        if(supportsRequirementQueueFamilies){
            swapChainMeetsRequirements = swapchainSupport.Formats.size()>0 && swapchainSupport.PresentationModes.size() > 0;
        }

        Logger::Trace("Find a physical device that meets all the requirements...");
        //NOTE: Could also look for discrete GPU. We could score and rank them based on features and capabilities
        return supportsRequirementQueueFamilies && swapChainMeetsRequirements && features.samplerAnisotropy;
    }


    void VulkanRenderer::detectQueueFamilyIndices(VkPhysicalDevice physicalDevice, int32_t* graphicsQueueIndex, int32_t* presentationQueueIndex){
        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> familyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, familyProperties.data());      

        for(uint32_t i=0; i<queueFamilyCount; i++){
            
            //Does it support the graphics queue?
            if(familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
                *graphicsQueueIndex = i;
            }

            VkBool32 supportsPresentation = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportsPresentation);
            if(supportsPresentation){
                *presentationQueueIndex = i;
            }
        }
    }

    VulkanSwapchainSupportDetails VulkanRenderer::querySwapchainSupport(VkPhysicalDevice physicalDevice){
        VulkanSwapchainSupportDetails details;

        //Capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.Capabilities);     

        //Surface formats
        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
        if(formatCount != 0){
            details.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.Formats.data());
        }        

        //Presentation modes
        uint32_t presentationModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentationModeCount, nullptr);
        if(presentationModeCount != 0){
            details.PresentationModes.resize(presentationModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentationModeCount, details.PresentationModes.data());
        }   


        return details;
    }

    void VulkanRenderer::createLogicalDevice(std::vector<const char*> &requiredValidationLayers){
        Logger::Trace("Creating logical device");
        int32_t graphicsQueueIndex = -1;
        int32_t presentationQueueIndex = -1;
        detectQueueFamilyIndices(physicalDevice, &graphicsQueueIndex, &presentationQueueIndex);

        // If the queue indices are the same, only one queue needs to be created.
        bool presentationSharesGraphicsQueue = graphicsQueueIndex == presentationQueueIndex;

        std::vector<uint32_t> indices;
        indices.push_back(graphicsQueueIndex);
        if( !presentationSharesGraphicsQueue){
            indices.push_back(presentationQueueIndex);
        }

        // Device queues
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(indices.size());
        for(uint32_t i = 0; i<(uint32_t)indices.size(); ++i){
            queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfos[i].queueFamilyIndex = indices[i];
            queueCreateInfos[i].queueCount = 1;
            queueCreateInfos[i].flags = 0;
            queueCreateInfos[i].pNext = nullptr;
            float queuePriority = 1.0f;
            queueCreateInfos[i].pQueuePriorities = &queuePriority;
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE; // Request anistrophy

        VkDeviceCreateInfo deviceCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.queueCreateInfoCount = (uint32_t)indices.size();
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.enabledExtensionCount = 1;
        deviceCreateInfo.pNext = nullptr;
        const char* requiredExtensions[1] = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions;

        // TODO: disable on release builds.
        deviceCreateInfo.enabledLayerCount = (uint32_t)requiredValidationLayers.size();
        deviceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();

        // Create the device
        VK_CHECK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));

        // Save off the queue family indices
        graphicsFamilyQueueIndex = graphicsQueueIndex;
        presentationFamilyQueueIndex = presentationQueueIndex;

        // Create the queues.
        vkGetDeviceQueue(device, graphicsFamilyQueueIndex, 0, &graphicsQueue);
        vkGetDeviceQueue(device, presentationFamilyQueueIndex, 0, &presentationQueue);
        Logger::Trace("Queues created");
    }   
}
