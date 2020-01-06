#ifndef _SCRIPT_H
#define _SCRIPT_H

#include <unordered_map>
#include <string>
#include <vector>

//  loads a script file into an std::unordered_map

namespace game::content {
    union script_args{
        float type_1;
        int type_2;
        std::tuple<float, int> *type_3;
    };

    struct stage_scripts {
        /*  
            stage script
            enemy script
            bullet script
                use a struct for triggers
                enum/int + union
                unordered map for frame triggers


        */
        // std::unordered_map<int, std::unordered_map<int, script_instruction*>*> *enemy, *bullet;
        

    };

    class script_instruction {
        public:
            script_instruction();
            ~script_instruction();
            std::vector<int> *instruct;
            std::vector<script_args> *val;
    };

    //  script trigger helper functions
    uint8_t trigger_getType(uint64_t);
    uint64_t trigger_setType(uint8_t);
    //  1 = frame (frame)
    //  2 = external (id)
    //  3 = fromFrame (frame)
    //  4 = fromFrameCancel (frame, id)
    //  5 = betweenFrames (frameStart, frameEnd)

    //  all data structures must be 7 bytes or less
    struct trigger_int_int {
        uint16_t frame;
        uint16_t id;
    };

    uint64_t trigger_setInt(uint64_t, uint16_t);
    uint64_t trigger_setFloat(uint64_t, float);
    uint64_t trigger_setIntInt(uint64_t, trigger_int_int);

    uint16_t trigger_getInt(uint64_t);
    float trigger_getFloat(uint64_t);
    trigger_int_int trigger_getIntInt(uint64_t);
    

    //  load instructions  
    std::unordered_map<uint64_t, script_instruction*> *loadScriptBullet(const std::string&);
    stage_scripts loadStage(const std::string&);

    void script_init();

}

#endif