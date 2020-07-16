#ifndef DEBUG_H
#define DEBUG_H

namespace engine {
    void log_debug(const char *text, ...);

    template<typename ...Args>
    void log_release(const char *text, ...);
}


#endif