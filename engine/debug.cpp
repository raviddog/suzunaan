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
        #if defined(_MSG_DEBUG_ENABLED_LOG) || defined(_MSG_RELEASE_ENABLED)
        freopen("./debug.log", "w", stderr);
        #endif
        #ifdef _MSG_DEBUG_ENABLED_VER
        log_debug(_MSG_DEBUG_ENABLED_VER);
        #endif
    }

    void log_debug(const char *text, ...) {
        #ifdef _MSG_DEBUG_ENABLED
        va_list args;
        va_start(args, text);
        #ifdef _MSG_DEBUG_ENABLED_LOG
        vfprintf(stderr, text, args);
        fflush(stderr);
        #elif
        vfprintf(stdout, text, args);
        #endif
        va_end(args);
        #endif
    }

        // time(&rawtime);
        // timeinfo = localtime(&rawtime);
        // printf("DEBUG [%s]: ");
    void log_release(const char *text, ...) {
        #ifdef _MSG_RELEASE_ENABLED
        //  write release messages to console as well
        va_list args;
        va_start(args, text);
        #ifdef _MSG_DEBUG_ENABLED_LOG
        vfprintf(stderr, text, args);
        fflush(stderr);
        #endif
        va_end(args);
        #endif
    }
}