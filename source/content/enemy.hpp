#ifndef _ENEMY_H
#define _ENEMY_H

#include <unordered_map>
#include "script.hpp"
#include "bullet.hpp"

namespace game::content {

    extern bullet_s* (*getBullet)();

    class enemy_s {
        private:
            void run_instructions();

            //  script instructions
            void instr_speed(float);
            void instr_angle(float);
            void instr_accel(float);
            void instr_bullet(uint32_t);
            void instr_setHP(int);
            size_t instr_stop(uint32_t);
            void instr_start(uint32_t);
            void instr_frameTrigger(uint32_t, uint32_t);
            void instr_frameTriggerOffset(uint32_t, uint32_t);
            void instr_stopInterval(uint32_t);
            

        public:
            enemy_s();
            ~enemy_s();
            void activate();
            void update();

            bool active;
            int id, type, frames, hp;
            float x_pos, y_pos;

            float speed, angle;
            float accel, angle_change;
            float draw_angle;

            enemy_script *instructions = nullptr;
            std::vector<uint32_t> *active_instructions = nullptr;
            //  frame triggers that are created by instructions
            std::unordered_multimap<uint32_t, uint32_t> *cust_triggers = nullptr;
            std::unordered_set<uint32_t> *cancel_cust_triggers = nullptr;
            //  local version of non-frame trigger listeners
            std::unordered_multimap<uint32_t, std::pair<script_args, uint32_t>> *listener_triggers;
    };


}



#endif