#include "script.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace game::content {

    //  instructions
    std::unordered_map<std::string, std::pair<int, int>> *bullet_instr = nullptr;
    std::unordered_map<std::string, std::pair<int, int>> *enemy_instr = nullptr;

    void script_init() {
        if(bullet_instr == nullptr) {
            bullet_instr = new std::unordered_map<std::string, std::pair<int, int>>();
            bullet_instr->insert({"speed", std::make_pair(1, 1)});
            bullet_instr->insert({"accel", std::make_pair(2, 1)});
            bullet_instr->insert({"angle_change", std::make_pair(3, 1)});
            bullet_instr->insert({"type_set_relative", std::make_pair(4, 2)});
        }

        if(enemy_instr == nullptr) {
            enemy_instr = new std::unordered_map<std::string, std::pair<int, int>>();
            enemy_instr->insert({"speed", std::make_pair(1, 1)});
        }

    }

    std::unordered_map<int, script_instruction*> *loadScript(const std::string &path) {
        //  load the entire file into 1 string, or read it while loading?
        std::string content;
        size_t next = 0, offset = 0;
        std::ifstream file;

        //  ensure ifstream objects can throw exceptions
        file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try {
            file.open(path);
            std::stringstream filestream;
            filestream << file.rdbuf();
            file.close();
            content = filestream.str();
        } catch(std::ifstream::failure e) {
            printf("failed to load script file %s\n", path.c_str());
        }

        //  first time using auto lol
        //  honestly i should just use it for every 'new' statement
        //  i dont like using it on function returns and most things in general though
        auto script = new std::unordered_map<int, script_instruction*>();

        //  read script type, should be in first line
        while(content[next + offset] != '\n' && content[next + offset] != '\0') {
            offset++;
        }

        std::unordered_map<std::string, std::pair<int, int>> *instr;
        std::string script_type = content.substr(next, offset);
        if(script_type == "type:bullet") {
            instr = bullet_instr;
        } else if(script_type == "type:enemy") {
            instr = enemy_instr;
        }

        next = offset + 1;

        while(content[next] != '\0') {
            if(content[next] == '\n') {
                //  skip blank newlines
                next++;
            } else if(content[next] == '/' && content[next + 1] != '\0' && content[next + 1] == '/') {
                //  skip lines starting with //
                while(content[next] != '\n') next++;
            } else {
                //  get instruction frame
                offset = 0;
                while(content[next + offset] != '\0' && content[next + offset] != ':') {
                    offset++;
                }
                int frame = std::stoi(content.substr(next, offset), nullptr);
                next += offset;
                //  if EOF, bail
                if(content[next] == '\0') break;

                script_instruction *i;

                //  check if frame data exists
                if(script->count(frame) > 0) {
                    //  exists
                    i = script->at(frame);
                } else {
                    //  doesn't exist
                    i = new script_instruction();
                    script->insert({frame, i});
                }

                //  get function
                next++;
                offset = 0;
                while(content[next + offset] != '\0' && content[next + offset] != '(') {
                    //  set offset to end of function name
                    offset++;
                }

                //  get function string
                std::string function_name = content.substr(next, offset);
                //  set next to beginning of arguments
                next += offset + 1;
                offset = 0;

                //  set offset to end of function
                while(content[next + offset] != '\0' && content[next + offset] != ';') {
                    offset++;
                }

                //  load function info
                std::pair<int, int> function_info;
                if(instr->count(function_name) > 0) {
                    //  load function info and insert
                    printf("loading function %s\n", function_name.c_str());
                    function_info = instr->at(function_name);
                    i->instruct->push_back(function_info.first);

                    //  get arguments
                    if(function_info.second == 1) {
                        //  single float
                        float arg_1 = std::stof(content.substr(next, offset), nullptr);
                        while(content[next] != ',' && next < offset) next++;
                        script_args args;
                        args.type_1 = arg_1;
                        i->val->push_back(args);
                    } else if(function_info.second == 2) {
                        //  single int
                        int arg_1 = std::stoi(content.substr(next, offset), nullptr);
                        while(content[next] != ',' && next < offset) next++;
                        script_args args;
                        args.type_2 = arg_1;
                        i->val->push_back(args);
                    } else if(function_info.second == 3) {
                        //  float, int
                        float arg_1 = std::stof(content.substr(next, offset), nullptr);
                        while(content[next] != ',' && next < offset) next++;
                        int arg_2 = std::stoi(content.substr(next, offset), nullptr);
                        while(content[next] != ',' && next < offset) next++;
                        script_args args;
                        args.type_3 = new std::tuple<float, int>(arg_1, arg_2);
                        i->val->push_back(args);
                    }

                } else {
                    printf("unable to locate instruction %s\n", function_name.c_str());
                }

                next += offset;
                while(content[next] != '\0' && content[next] != '\n') next++;

            }
        }

        return script;
    }
   


    script_instruction::script_instruction() {
        instruct = new std::vector<int>();
        val = new std::vector<script_args>();
    }

    script_instruction::~script_instruction() {
        for(int i = 0; i < instruct->size(); i++) {
            switch(instruct->at(i)) {
                //  add a case for every function that uses a tuple as its arguments
                //  and just let them drop through to the delete
                case 3:
                    delete val->at(i).type_3;
                    break;

            }
        }
        delete instruct;
        delete val;
    }
}