#ifndef __VULKAN_UTILS_H__
#define __VULKAN_UTILS_H__

#include <vulkan/vulkan.h>
#include <stdlib.h>

namespace Rooten{

    bool VK_CHECK(VkResult result){if(result == VK_SUCCESS){return true;} else{ exit(EXIT_FAILURE); return false;}}
}

#endif
