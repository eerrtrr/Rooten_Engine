#ifndef __VULKAN_RENDERER_H__
#define __VULKAN_RENDERER_H__

#include <vulkan/vulkan.h>
#include <vector>


namespace Rooten{

    struct VulkanSwapchainSupportDetails{
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentationModes;
    };

    class Platform;

    class VulkanRenderer{
    public:
        VulkanRenderer(Platform* platform);
        ~VulkanRenderer();

    private:
        VkPhysicalDevice selectPhysicalDevice();
        bool physicalDeviceMeetsRequirements(VkPhysicalDevice physicalDevice);
        void detectQueueFamilyIndices(VkPhysicalDevice physicalDevice, int32_t* graphicsQueueIndex, int32_t* presentationQueue);
        VulkanSwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice physicalDevice);
        void createLogicalDevice(std::vector<const char*> &requiredValidationLayers);

    private:
        Platform* platform;

        VkInstance instance;

        VkDebugUtilsMessengerEXT debugMessenger;

        VkPhysicalDevice physicalDevice;
        VkDevice device;

        int32_t graphicsFamilyQueueIndex;
        int32_t presentationFamilyQueueIndex;
        VkQueue graphicsQueue;
        VkQueue presentationQueue;

        VkSurfaceKHR surface;
    };
}

#endif
