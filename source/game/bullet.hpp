#ifndef _BULLET_H
#define _BULLET_H

#include "player.hpp"
#include "script.hpp"

#include <unordered_set>
#include <memory>

//#include <stdint.h>

namespace Game {
    extern float bullet_bounds_x, bullet_bounds_y, bullet_bounds_xmax, bullet_bounds_ymax;
    
    enum BulletTypes {
            BTLaser = 0,
            BTArrowhead = 16,
            BTBallOutlined = 32,
            BTBall = 48,
            BTRice = 64,
            BTKunai = 80,
            BTIcicle = 96,
            BTAmulet = 112,
            BTBullet = 128,
            BTRiceDark = 144,
            BTStar = 160,
            BTBallBouncy = 176,

            BCBlack = 0,
            BCRed = 1,
            BCLightRed = 2,
            BCPurple = 3,
            BCPink = 4,
            BCDarkBlue = 5,
            BCBlue = 6,
            BCLightBlue = 7,
            BCAqua = 8,
            BCTurquoise = 9,
            BCGreen = 10,
            BCLime = 11,
            BCYellow = 12,
            BCLightYellow = 13,
            BCOrange = 14,
            BCGrey = 15
        };
        
    class bullet_s {
        private:
            void run_instructions();
            //  bullet instruction functions
            void instr_speed(float);
            void instr_accel(float);
            void instr_angle_change(float);
            void instr_type_set_relative(int);
            void instr_angle(float);
            //  needs to inform the instruction loop that an element was removed
            size_t instr_stop(uint32_t);  
            void instr_start(uint32_t);
            void instr_frameTrigger(uint32_t, uint32_t);
            void instr_frameTriggerOffset(uint32_t, uint32_t);
            void instr_stopInterval(uint32_t);
            void instr_angle_atPlayer();
            void instr_random_angle_change(float, float);
            void instr_random_angle(float, float);
            



        public:
            bullet_s();
            ~bullet_s();

            bool active;
            uint32_t type, frames;
            int owner;
            float x_pos, y_pos;
            
            //  not sure which way to implement movement
            //  should accel be per second or per frame
            //  probably per frame, but then the values have to be super small
            float speed, angle;
            float accel, angle_change;
            float draw_angle;

            //  pointer to player that this bullet interacts with
            //  all bullets interact with same player for now, this aint pofv
            //  so i can make this static and save space
            static player_s *player;

            void reset();
            void update();

            void setPos(float x_pos, float y_pos);
            void setPos(float x_pos, float y_pos, float speed, float angle);

            //  source instructions
            // bullet_script *instructions = nullptr;
            //  continuous instructions
            std::vector<uint32_t> *active_instructions = nullptr;
            //  frame triggers that are created by instructions
            // std::unordered_multimap<uint32_t, uint32_t> *cust_triggers = nullptr;
            std::unordered_set<uint32_t> *cancel_cust_triggers = nullptr;
            //  local version of non-frame trigger listeners
            // std::unordered_multimap<uint32_t, std::pair<script_args, uint32_t>> *listener_triggers;

            

            //  compiled scripts shouldn't need instruction maps
            //  instead just provide some storage variables
            union storage_u {
                uint32_t u;
                int32_t i;
                float f;
                bool b;
            };

            //  adjust this to the number of storage variables needed
            storage_u storage[4];
            //  pointer to the correct instruction function
            // void (*run_instructions)(bullet_s*);            
            //  helper functions that the instruction function can use
            float instr_getAngleToPlayer();

    };
}


#endif