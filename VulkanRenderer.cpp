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
        instanceCreateInfo.enabledExtensionCount = 0;
        instanceCreateInfo.ppEnabledExtensionNames = nullptr;

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
        VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &this->instance));
        Logger::Trace("\tCreating instance...");

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
    }


    VulkanRenderer::~VulkanRenderer(){
        if(enableValidationLayers){
            PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->instance, "vkDestroyDebugUtilsMessengerEXT");
            func(this->instance, this->debugMessenger, nullptr);
        }
        vkDestroyInstance(this->instance, nullptr);
    }
}
