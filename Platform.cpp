#define VK_USE_PLATFORM_XLIB_KHR
#define VK_KHR_XLIB_SURFACE

#include "Platform.hpp"
#include "Engine.hpp"
#include "AXL.hpp"
#include "Logger.hpp"
#include "VulkanUtils.hpp"
#include "VulkanRenderer.hpp"


namespace Rooten{

    Platform::Platform(Engine* _engine, const char* applicationName){
        Logger::Trace("Initializing platform layer...");
        this->engine = _engine;

        this->window = new Winaxl(0, 0, 0, 1280, 720, applicationName);

    }

    Platform::~Platform(){
        if(this->window){
            delete this->window;
        }
    }


    void Platform::createSurface(VkInstance instance, VkSurfaceKHR* surface){
        VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = {VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR};
        surfaceCreateInfo.pNext = nullptr;
        surfaceCreateInfo.flags = 0;
        surfaceCreateInfo.dpy = window->getDisplay();
        surfaceCreateInfo.window = window->getWindow();

        VK_CHECK(vkCreateXlibSurfaceKHR(instance, &surfaceCreateInfo, nullptr, surface));
    }


    const bool Platform::startGameLoop(){
        while(this->window->getStatus() == true){
            window->pollEvents();

            engine->onLoop(0);
        }

        return true;
    }
}
