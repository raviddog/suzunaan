#ifndef _SCRIPT_H
#define _SCRIPT_H

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

//  loads a script file into an std::unordered_map

namespace Game {


    union script_args{
        //  type_0 is no argument
        float type_1;
        int32_t type_2;
        uint32_t type_3;
        uint64_t type_4;    //  two unsigned ints
        uint64_t type_5;    //  two floats
        // std::unique_ptr<std::tuple<float, int>> type_3;
        // std::tuple<float, int> *type_3;
    };

    std::pair<uint32_t, uint32_t> script_getIntInt(uint64_t);
    std::pair<float, float> script_getFloatFloat(uint64_t);

    struct script_instruction {
        script_instruction();
        ~script_instruction();
        bool selfdestruct = false;
        std::vector<uint32_t> *instruct;
        std::vector<script_args> *val;
    };

    struct bullet_script {
        bullet_script();
        ~bullet_script();

        //  id map needs to be stored in script structure, in case other scripts need to interact with it
        std::unordered_map<uint32_t, uint32_t> *id_map;
        std::unordered_map<uint32_t, script_instruction*> *instructions;
        std::unordered_map<uint32_t, std::vector<uint32_t>*> *frame_triggers;

        //  list of listeners (non-frame based triggers)
        //  listener type, pair<listener type, value, instruction id>
        std::unordered_multimap<uint32_t, std::pair<script_args, uint32_t>> *listeners;
    };

    //  a struct containing the initial values a bullet is spawned with
    //  this will definitely need to be changed in the future when i add variables in enemy scripts
    //  also this name sucks, but whatever
    struct bullet_spawn {
        uint32_t type = 0;
        uint32_t scriptID = 0;
        //  have a type variable for changing whether these are offsets or absolute coordinates
        float x_offset = 0.f;
        float y_offset = 0.f;
        float speed = 0.f;
        float angle = 0.f;
        float accel = 0.f;
        float angle_change = 0.f;
    };

    struct enemy_spawn {
        uint32_t type = 0;
        uint32_t scriptID = 0;
        //  have a type variable for changing whether these are offsets or absolute coordinates
        float x_offset = 0.f;
        float y_offset = 0.f;
        float speed = 0.f;
        float angle = 0.f;
        float accel = 0.f;
    };

    struct enemy_script {
        enemy_script();
        ~enemy_script();

        std::unordered_map<uint32_t, uint32_t> *id_map;
        std::unordered_map<uint32_t, script_instruction*> *instructions;
        std::unordered_map<uint32_t, std::vector<uint32_t>*> *frame_triggers;

        std::unordered_multimap<uint32_t, std::pair<script_args, uint32_t>> *listeners;
        std::unordered_map<uint32_t, bullet_spawn> *bullet_spawns;
    
    };

    struct stage_script {
        stage_script();
        ~stage_script();
        
        // std::unordered_map<uint32_t, bullet_spawn> *bullet_spawns;
        std::vector<bullet_spawn> *bullet_spawns;
        std::vector<enemy_spawn> *enemy_spawns;
        std::unordered_map<uint32_t, script_instruction*> *frame_triggers;
    };
    

   

    void script_init();
    void script_cleanup();
    int loadScript(const std::string &path, stage_script **stageptr, std::unordered_map<uint32_t, std::shared_ptr<enemy_script>> **enemy_scripts_ptr, std::unordered_map<uint32_t, std::shared_ptr<bullet_script>> **bullet_scripts_ptr);

}

#endif