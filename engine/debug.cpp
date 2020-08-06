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
        //  find a better way of versioning
        #ifdef _DEBUG_MSG_ENABLED_LOG
        freopen("./debug.log", "w", stderr);
        #endif
        #ifdef _DEBUG_MSG_ENABLED_VER
        log_debug("Touhou Suzunaan ver d.0.0.1\n\n");
        #endif
    }

    void log_debug(const char *text, ...) {
        #if defined(DEBUG) || defined(_DEBUG)
        va_list args;
        va_start(args, text);
        #ifdef _DEBUG_MSG_ENABLED_CONSOLE
        vfprintf(stdout, text, args);
        #endif
        #ifdef _DEBUG_MSG_ENABLED_LOG
        vfprintf(stderr, text, args);
        #endif
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
        #ifdef _DEBUG_MSG_ENABLED_CONSOLE
        vfprintf(stdout, text, args);
        #endif
        #ifdef _DEBUG_MSG_ENABLED_LOG
        vfprintf(stderr, text, args);
        #endif
        va_end(args);
        #endif
    }
}