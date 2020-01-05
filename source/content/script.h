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

    struct script_trigger {
        uint8_t type;
            //  1 = frame
            //  2 = external
        uint16_t value;
        // union {
        //     uint16_t frame;  //  frame number
        //     uint16_t external;   //  called by parent enemy or stage or something
        // } value;

        uint32_t pack();
        static script_trigger unpack(uint32_t);
    };
    
    //  frame, instruction data
    std::unordered_map<int, script_instruction*>* loadScript(const std::string&);
    
    std::unordered_map<uint32_t, script_instruction*> *loadScriptBullet(const std::string&);
    stage_scripts loadStage(const std::string&);

    void script_init();

}

#endif