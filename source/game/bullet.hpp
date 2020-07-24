#ifndef _BULLET_H
#define _BULLET_H

#include "player.hpp"

#include <stdint.h>

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
        public:
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
            void (*run_instructions)(bullet_s*);            
            //  helper functions that the instruction function can use
            float instr_getAngleToPlayer();

    };
}


#endif