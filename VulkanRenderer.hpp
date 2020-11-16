#ifndef __VULKAN_RENDERER_H__
#define __VULKAN_RENDERER_H__

#include <vulkan/vulkan.h>

namespace Rooten{

    class Platform;

    class VulkanRenderer{
    public:
        VulkanRenderer(Platform* platform);
        ~VulkanRenderer();

    private:
        Platform* platform;

        VkInstance instance;
        
        VkDebugUtilsMessengerEXT debugMessenger;

        VkPhysicalDevice physicalDevice;
        VkDevice device;
    };
}

#endif
