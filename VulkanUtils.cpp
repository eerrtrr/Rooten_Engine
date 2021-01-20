#include "VulkanUtils.hpp"

namespace Rooten{
    
    bool VK_CHECK(VkResult result){
        if(result == VK_SUCCESS){
            return true;
        } else{
            exit(EXIT_FAILURE);
            return false;
        }
    }
}
