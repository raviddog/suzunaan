#ifndef DEBUG_H
#define DEBUG_H

// #if defined(DEBUG) || defined(_DEBUG)
// #if !defined(DEBUG) && !defined(_DEBUG)

#define _MSG_DEBUG_ENABLED
#define _MSG_RELEASE_ENABLED

#define _MSG_DEBUG_ENABLED_VER "Touhou Suzunaan ver d.0.0.2\n\n"
// #define _MSG_DEBUG_ENABLED_SPRITE
#define _MSG_DEBUG_ENABLED_FPS
#define _MSG_DEBUG_ENABLED_BULLETCOUNT
#define _MSG_DEBUG_ENABLED_CONSOLE
#define _MSG_DEBUG_ENABLED_LOG

#define _MSG_RPERF_ENABLED_BULLETCOUNT
#define _MSG_RPERF_ENABLED_FPS

namespace engine {
    void debug_init();

    void log_debug(const char *text, ...);

    void log_release(const char *text, ...);
}


#endif