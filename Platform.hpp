#ifndef __PLATFORM_H__
#define __PLATFORM_H__


namespace Rooten{

    class Engine;
    class Winaxl;

    class Platform{
    public:
        Platform(Engine* _engine, const char* applicationName);
        ~Platform();
        
        const bool startGameLoop();

    private:
        Winaxl* window;
        Engine* engine;
    };
}

#endif
