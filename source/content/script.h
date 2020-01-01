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

    class script_instruction {
        public:
            script_instruction();
            ~script_instruction();
            std::vector<int> *instruct;
            std::vector<script_args> *val;
    };
    
    //  frame, instruction data
    std::unordered_map<int, script_instruction*>* loadScript(const std::string&);

    void script_init();

}

#endif