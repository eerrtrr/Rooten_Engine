#ifndef __ENGINE_H__
#define __ENGINE_H__


namespace Rooten{

    class Platform;
    class VulkanRenderer;

    class Engine{
    public:
        Engine(const char* applicationName);
        ~Engine();

        void run();

        void onLoop(const float deltaTime);

    private:
        Platform* platform;
        VulkanRenderer* renderer;
    };
}

#endif
