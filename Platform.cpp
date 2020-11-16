#define VK_USE_PLATFORM_XLIB_KHR

#include "Platform.hpp"
#include "Engine.hpp"
#include "AXL.hpp"
#include "Logger.hpp"
#include <vulkan/vulkan.h>


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

    const bool Platform::startGameLoop(){
        while(this->window->getStatus() == true){
            window->pollEvents();

            engine->onLoop(0);
        }

        return true;
    }
}
