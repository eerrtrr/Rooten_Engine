#ifndef __LOGGER_H_
#define __LOGGER_H_

namespace Rooten{

    class Logger{
    public:
        static void Trace(const char* message, ...);
        static void Log(const char* message, ...);
        static void Warn(const char* message, ...);
        static void Error(const char* message, ...);
        static void Fatal(const char* message, ...);
    };
}

#endif
