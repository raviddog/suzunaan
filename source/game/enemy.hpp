#ifndef _ENEMY_H
#define _ENEMY_H

#include "bullet.hpp"
#include "script.hpp"

#include <map>
#include <vector>
#include "stdint.h"

namespace Game {

    struct enem_data {
        int type;
        float hitbox;
    };
    
    class enemy_s {
        private:
            void run_instructions();

            //  script instructions
            void instr_speed(float);
            void instr_angle(float);
            void instr_accel(float);
            void instr_bullet(int);
            void instr_setHP(int);
            size_t instr_stop(int);
            void instr_start(int);
            void instr_frameTrigger(int, int);
            void instr_frameTriggerOffset(int, int);
            void instr_stopInterval(int);
            void instr_enemy(int);
            void instr_angle_change(float);
            void instr_bullet_ring(int, int);
            void instr_moveToParent();
        public:
            enemy_s();
            enemy_s(float x_pos, float y_pos, int type, int hp);
            ~enemy_s();
            void activate();
            void update();

            //  static functions
            static bullet_s* (*getBullet)();
            static std::vector<enemy_s*> *enemy_draw;
            //  do i need an array for existing enemies
            //  don't for now

            bool active, owned;
            int id, owner, type, animFrame, animDelay, moveDir, frames, hp;
            float x_pos, y_pos;

            float speed, angle;
            float accel, angle_change;
            float draw_angle;

            enemy_script *instructions = nullptr;
            std::vector<int> *active_instructions = nullptr;
            //  frame triggers that are created by instructions
            std::multimap<int, int> *cust_triggers = nullptr;
            std::set<int> *cancel_cust_triggers = nullptr;
            //  local version of non-frame trigger listeners
            std::multimap<int, std::pair<script_args, int>> *listener_triggers;
            
            //  compiled scripts shouldn't need instruction maps
            //  instead just provide some storage variables
            union storage_u {
                int i;
                float f;
                bool b;
            };

            //  adjust this to the number of storage variables needed
            storage_u storage[6];
            //  pointer to the correct instruction function
            // void (*run_instructions)(enemy_s*);            
            //  helper functions that the instruction function can use
            // float instr_angleToPlayer();
    };


}



#endif