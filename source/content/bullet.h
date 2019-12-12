#ifndef _BULLET_H
#define _BULLET_H

#include <unordered_map>

namespace game::content {
    struct bullet_s {
        bool active;
        int type, frames;
        float x_pos, y_pos;
        
        //  not sure which way to implement movement
        float x_speed, y_speed, x_accel, y_accel;
        float speed, angle;

        std::unordered_map<int, std::string> *instructions;
        void update();
    };
}


#endif