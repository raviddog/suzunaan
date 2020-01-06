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
            bullet_instr->insert({"angle", std::make_pair(5, 1)});
        }

        if(enemy_instr == nullptr) {
            enemy_instr = new std::unordered_map<std::string, std::pair<int, int>>();
            enemy_instr->insert({"speed", std::make_pair(1, 1)});
        }

    }

    std::unordered_map<uint64_t, script_instruction*> *loadScriptBullet(const std::string &path) {
        printf("loading bullet script file %s\n", path.c_str());
        std::string content;
        size_t next = 0, offset = 0, length;
        int line = 1;
        std::ifstream file;

        file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try {
            file.open(path);
            std::stringstream filestream;
            filestream << file.rdbuf();
            file.close();
            content = filestream.str();
        } catch(std::ifstream::failure ex) {
            printf("failed to open file, %s\n", ex.what());
        }

        length = content.length();

        //  read script type in first line
        while(content[next+offset] != ':' && next + offset < length) offset++;
        if(content.substr(next, offset) != "type") {
            //  no type header in script file
            printf("no type header in script, aborting\n");
            return nullptr;
        }

        //  skip past colon and spaces
        offset++;
        while(content[next + offset] == ' ') offset++;
        next += offset;
        offset = 0;
        while(content[next + offset] != '\n' && next + offset < length) offset++;

        std::string script_type = content.substr(next, offset);
        if(script_type == "enemy") {
            printf("enemy script detected instead, aborting");
            return nullptr;
        } else if(script_type == "stage") {
            printf("stage script detected instead, aborting");
            return nullptr;
        } else if(script_type != "bullet") {
            printf("unknown script type, aborting\n");
            return nullptr;
        }

        //  first time using auto lol
        //  honestly i should just use it for every 'new' statement
        //  i dont like using it on function returns and most things in general though
        auto script = new std::unordered_map<uint64_t, script_instruction*>();

        next += offset + 1;
        line++;

        while(next < length) {
            //  skip spaces
            while(content[next] == ' ') next++;
            if(content[next] == '\n') {
                next++;
                line++;
            } else if(content[next] == '/' && content[next + 1] == '/') {
                //  skip comment lines starting with "//"
                while(content[next] != '\n' && next < length) next++;
            } else {
                //  process instruction
                //  first get trigger function name
                offset = 0;
                while(content[next + offset] != '(' && content[next + offset] != '\n'
                    && content[next + offset] != ':' && next + offset < length) offset++;

                std::string trigger_type = content.substr(next, offset);
                //  next get trigger argument string
                next += offset + 1;
                offset = 0;
                while(content[next + offset] != ')' && content[next + offset] != '\n'
                    && content[next + offset] != ':' && next + offset < length) offset++;
                //  check trigger type and create/load instructions
                script_instruction *instruction;
                uint64_t trigger;
                bool abort = false;
                if(trigger_type == "frame") {
                    try {
                        trigger = 1;
                        //  one int
                        uint64_t frame = std::stoul(content.substr(next, offset), nullptr);
                        trigger += frame << 8;
                        printf("frame %d:", frame);
                        //  check if frame data exists
                        if(script->count(trigger) > 0) {
                            //  exists
                            instruction = script->at(trigger);
                        } else {
                            //  doesn't exist
                            instruction = new script_instruction();
                            script->insert({trigger, instruction});
                        }
                    } catch (std::invalid_argument ex) {
                        printf("invalid frame trigger in line %d, skipping line", line);
                        abort = true;
                    }
                } else {
                    printf("unknown trigger in line %d, skipping line", line);
                    abort = true;
                }
                //  skip closing bracket, spaces and colon to functions
                next += offset + 1;
                offset = 0;
                while(next + offset < length && (content[next + offset] == ' ' || content[next + offset] == ':')) next++;
                //  no other trigger types for now
                if(!abort) {
                    //  read functions until semicolon terminator
                    while(content[next] != ';' && next < length) {
                        //  skip spaces and newlines
                        while(content[next] == ' ' || content[next] == '\n') {
                            if(content[next] == '\n') line++;
                            next++;
                        }
                        
                        offset = 0;
                        //  set offset to end of function name, or semicolon/newline in case of improperly written script
                        while(next + offset < length && content[next + offset] != '(' && content[next + offset] != ';') {
                            offset++;
                        }

                        //  get function string
                        std::string function_name = content.substr(next, offset);
                        //  set next to beginning of arguments
                        next += offset + 1;
                        offset = 0;
                        //  set offset to end of function
                        while(next + offset < length && content[next + offset] != ')'
                            && content[next + offset] != ';' && content[next + offset] != '\n') offset++;

                        //  load function info
                        std::pair<int, int> function_info;

                        if(bullet_instr->count(function_name) > 0) {
                            //  load function info and insert
                            printf(" function %s", function_name.c_str());
                            function_info = bullet_instr->at(function_name);
                            //  only insert function if arguments are successfully inserted
                            bool success = true;                                

                            //  get arguments
                            if(function_info.second == 1) {
                                //  single float
                                try {
                                    float arg_1 = std::stof(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;   
                                    }
                                    script_args args;
                                    args.type_1 = arg_1;
                                    instruction->val->push_back(args);
                                    printf("(float %f)", arg_1);
                                } catch (std::invalid_argument ex) {
                                    success = false;
                                    printf(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            } else if(function_info.second == 2) {
                                //  single int
                                try {
                                    int arg_1 = std::stoi(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;   
                                    }
                                    script_args args;
                                    args.type_2 = arg_1;
                                    instruction->val->push_back(args);
                                    printf("(int %d)", arg_1);
                                } catch (std::invalid_argument ex) {
                                    success = false;
                                    printf(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            } else if(function_info.second == 3) {
                                //  float, int
                                try {
                                    float arg_1 = std::stof(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;   
                                    }
                                    int arg_2 = std::stoi(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;   
                                    }
                                    script_args args;
                                    args.type_3 = new std::tuple<float, int>(arg_1, arg_2);
                                    instruction->val->push_back(args);
                                    printf("(float %f, int %d)", arg_1, arg_2);
                                } catch (std::invalid_argument ex) {
                                    success = false;
                                    printf(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            }

                            if(success) instruction->instruct->push_back(function_info.first);

                        } else {
                            printf("unable to locate instruction %s\n", function_name.c_str());
                        }
                        
                        next += offset + 1;
                    }
                }

                while(next < length && content[next] != '\n') next++;
                printf("\n");
            }
        }
        printf("finished loading");
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
                //  #FIX 18
                case 3:
                    delete val->at(i).type_3;
                    break;
            }
        }
        delete instruct;
        delete val;
    }

    
    uint64_t trigger_eraseType(uint64_t storage) {
        storage = storage >> 8;
        return storage;
    }
    uint8_t trigger_getType(uint64_t storage) {
        //  probably not even needed
        return storage;
    }
    uint64_t trigger_setType(uint8_t type) {
        //  might not need this either
        return (uint64_t)type;
    }
    uint64_t trigger_setInt(uint64_t storage, uint16_t data) {
        return storage += (uint64_t)(data << 8);
    }
    uint64_t trigger_setFloat(uint64_t storage, float data) {
        char *bytes = reinterpret_cast<char*>(&data);
        for(int i = 0; i < sizeof(float); i++) {
            storage += (uint32_t)*bytes << (i + 1) * 8;
        }
        return storage;
    }
    uint64_t trigger_setData(uint64_t storage, trigger_int_int data) {
        storage += (uint64_t)data.frame << 8;
        storage += (uint64_t)data.id << 24;
        return storage;
    }

    uint16_t trigger_getInt(uint64_t storage) {
        return trigger_eraseType(storage);
    }
    float trigger_getFloat(uint64_t storage) {
        uint32_t data = trigger_eraseType(storage);
        float *f = reinterpret_cast<float*>(&data);
        return *f;
    }
    trigger_int_int trigger_getIntInt(uint64_t storage) {
        storage = trigger_eraseType(storage);
        trigger_int_int data;
        data.frame = (uint16_t)storage;
        data.id = (uint16_t)(storage >> 16);
        return data;
    }

}