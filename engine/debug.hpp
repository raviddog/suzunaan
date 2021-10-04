#ifndef DEBUG_H
#define DEBUG_H

#if defined(DEBUG) || defined(_DEBUG)
#define _MSG_DEBUG_ENABLED
#endif
// #if !defined(DEBUG) && !defined(_DEBUG)

// #define _MSG_RELEASE_ENABLED
#ifdef _MSG_DEBUG_ENABLED
#define _MSG_DEBUG_ENABLED_VER "E1\n\n"
// #define _MSG_DEBUG_ENABLED_SPRITE
#define _MSG_DEBUG_ENABLED_FPS
#define _MSG_DEBUG_ENABLED_BULLETCOUNT
#define _MSG_DEBUG_ENABLED_CONSOLE
#define _MSG_DEBUG_ENABLED_LOG

#define _MSG_RPERF_ENABLED_BULLETCOUNT
#define _MSG_RPERF_ENABLED_FPS
#endif

namespace engine {
    void debug_init();

    void log_debug(const char *text, ...);

    void log_release(const char *text, ...);
}


#endif