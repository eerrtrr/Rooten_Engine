#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <vulkan/vulkan.h>

namespace Rooten{

    class Engine;
    class Winaxl;

    class Platform{
    public:
        Platform(Engine* _engine, const char* applicationName);
        ~Platform();

        const bool startGameLoop();

        void createSurface(VkInstance instance, VkSurfaceKHR* surface);

    private:
        Winaxl* window;
        Engine* engine;
    };
}

#endif
