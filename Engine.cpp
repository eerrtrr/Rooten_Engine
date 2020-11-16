#include "Engine.hpp"
#include "Platform.hpp"
#include "Logger.hpp"
#include "VulkanRenderer.hpp"

namespace Rooten{

    Engine::Engine(const char* applicationName){
        Rooten::Logger::Log("Initializing Rooten Engine...");
        this->platform = new Platform(this, applicationName);
        this->renderer = new VulkanRenderer(this->platform);
    }

    Engine::~Engine(){
        Rooten::Logger::Log("Leaving Rooten Engine...");
    }

    void Engine::run(){
        this->platform->startGameLoop();
    }

    void Engine::onLoop(const float deltaTime){

    }
}
