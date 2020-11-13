#include "Logger.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdarg.h>


namespace Rooten{

    static void writeLog(const char* pretend, const char* message, va_list args){
        vprintf((std::string(pretend) + message + "\n").c_str(), args);
    }

    void Logger::Trace(const char* message, ... ){
        va_list args;
        va_start(args, message);
        writeLog("[TRACE]: ", message, args);
        va_end(args);
    }

    void Logger::Log(const char* message, ... ){
        va_list args;
        va_start(args, message);
        writeLog("[LOG]: ", message, args);
        va_end(args);
    }

    void Logger::Warn(const char* message, ... ){
        va_list args;
        va_start(args, message);
        writeLog("[WARNER]: ", message, args);
        va_end(args);
    }

    void Logger::Error(const char* message, ... ){
        va_list args;
        va_start(args, message);
        writeLog("[ERROR]: ", message, args);
        va_end(args);
    }

    void Logger::Fatal(const char* message, ... ){
        va_list args;
        va_start(args, message);
        writeLog("[FATAL]: ", message, args);
        va_end(args);

        exit(EXIT_FAILURE);
    }
}
