#ifndef _SCRIPT_H
#define _SCRIPT_H

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

//  loads a script file into an std::unordered_map

namespace game::content {

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

    union script_args{
        float type_1;
        int32_t type_2;
        // uint64_t type_3;
        // std::unique_ptr<std::tuple<float, int>> type_3;
        // std::tuple<float, int> *type_3;
    };

    class script_instruction {
        public:
            script_instruction();
            ~script_instruction();
            bool selfdestruct = false;
            std::vector<uint32_t> *instruct;
            std::vector<script_args> *val;
    };

    struct bullet_script {
        bullet_script();
        ~bullet_script();

        std::unordered_map<uint32_t, uint32_t> *id_map;
        std::unordered_map<uint32_t, std::vector<uint32_t>*> *frame_triggers;
        std::unordered_map<uint32_t, script_instruction*> *instructions;

        //  list of listeners (non-frame based triggers)

    };


    //  load instructions  
    
    bullet_script *loadScriptBullet(const std::string&);
    stage_scripts loadStage(const std::string&);

    void script_init();

}

#endif