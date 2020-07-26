#include "debug.hpp"

#include <cstdio>
#include <cstdarg>
// #include <ctime>

// time_t rawtime;
// struct tm *timeinfo;
// char timestring[5];

//  dump messages in console for now
//  in future, write to logfile and record time

namespace engine {
    void debug_init() {
        freopen("./test.log", "w", stderr);
    }

    void log_debug(const char *text, ...) {
        #if defined(DEBUG) || defined(_DEBUG)
        va_list args;
        va_start(args, text);
        vfprintf(stdout, text, args);
        vfprintf(stderr, text, args);
        fflush(stderr);
        va_end(args);
        #endif
    }

        // time(&rawtime);
        // timeinfo = localtime(&rawtime);
        // printf("DEBUG [%s]: ");
    void log_release(const char *text, ...) {
        #if defined(DEBUG) || defined(_DEBUG)
        //  write release messages to console as well
        va_list args;
        va_start(args, text);
        vfprintf(stdout, text, args);
        vfprintf(stderr, text, args);
        va_end(args);
        #endif
    }
}