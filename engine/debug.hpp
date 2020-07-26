#ifndef DEBUG_H
#define DEBUG_H

// #define _DEBUG_MSG_ENABLED_SPRITE
// #define _DEBUG_MSG_ENABLED_FPS

namespace engine {
    void debug_init();

    void log_debug(const char *text, ...);

    template<typename ...Args>
    void log_release(const char *text, ...);
}


#endif