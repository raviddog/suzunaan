#ifndef _SCRIPT_H
#define _SCRIPT_H

#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

//  loads a script file into an std::unordered_map

namespace game::content {
    struct bullet_s;
    
    class script_instruction_bullet {
        public:
            script_instruction_bullet();
            ~script_instruction_bullet();
            std::vector<void (bullet_s::*)(float)> *instruct;
            std::vector<float> *val;
    };
    
    std::unordered_map<int, script_instruction_bullet*>* loadScriptBullet(const std::string &path);
    void script_init();

}

#endif