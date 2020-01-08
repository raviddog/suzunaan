#include "script.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

namespace game::content {

    uint64_t script_setIntInt(uint32_t, uint32_t);

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
            bullet_instr->insert({"stop", std::make_pair(6, 3)});
            bullet_instr->insert({"start", std::make_pair(7, 3)});
            bullet_instr->insert({"frameTrigger", std::make_pair(8, 4)});
            bullet_instr->insert({"frameTriggerOffset", std::make_pair(9, 4)});
        }

        if(enemy_instr == nullptr) {
            enemy_instr = new std::unordered_map<std::string, std::pair<int, int>>();
            enemy_instr->insert({"speed", std::make_pair(1, 1)});
        }

    }

    bullet_script *loadScriptBullet(const std::string &path) {
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

        bullet_script *script = new bullet_script();
        uint32_t nextId = 0;

        next += offset + 1;
        offset = 0;
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
                //  check if the line has an id in it by counting colons
                //  this is a really shitty jank way of doing it
                int colons = 0;
                uint32_t instruct_id = 0;
                while(next + offset < length && content[next + offset] != '\n') {
                    if(content[next + offset] == ':') colons++;
                    offset++;
                }
                //  2 colons, has id
                if(colons == 2) {
                    //  read id
                    try {
                        uint32_t tempid = std::stoul(content.substr(next, offset), nullptr);
                        //  check if it already exists
                        if(script->id_map->count(tempid) > 0) {
                            //  just assume the user knows what they're doing, duplicating an id
                            //  maybe you can do some cool tricks with this
                            instruct_id = script->id_map->at(tempid);
                        } else {
                            //  insert into user id map
                            script->id_map->insert({tempid, nextId});
                            instruct_id = nextId;
                            nextId++;
                        }
                    } catch (std::invalid_argument ex) {
                        //  invalid id, use nextId
                        printf("invalid id, ignoring. ");
                        instruct_id = nextId;
                        nextId++;
                    }
                    //  position next at the trigger function name
                    while(next < length && content[next] != ':') next++;
                    next++;
                } else {
                    //  no id given, just use nextId
                    instruct_id = nextId;
                    nextId++;
                }


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
                bool abort = false;
                if(trigger_type == "frame") {
                    //  add to frame triggers
                    try {
                        //  get frame number
                        uint32_t frame = std::stoul(content.substr(next, offset), nullptr);
                        printf("frame %d:", frame);
                        //  check if frame data exists
                        if(script->frame_triggers->count(frame) == 0) {
                            //  doesn't exist, create the vector
                            std::vector<uint32_t> *v = new std::vector<uint32_t>();
                            script->frame_triggers->insert({frame, v});
                        }
                        //  insert instruction ID into vector
                        //  do it now after a valid trigger has been determined
                        //  if this throws an excecption after inserting, things will break
                        //  have to do this on every trigger because some of them dont use frames as triggers,
                        //      so i need for them to be able to insert the id into whatever new structure they use
                        script->frame_triggers->at(frame)->push_back(instruct_id);
                        //  i dont know why im supporting setting multiple things to the same id
                        //  but i am for some reason
                        //  "supporting", since i can't guarantee what happens, but ill make it so the functions at least activate
                        if(script->instructions->count(instruct_id) > 0) {
                            instruction = script->instructions->at(instruct_id);
                        } else {
                            instruction = new script_instruction();
                            script->instructions->insert({instruct_id, instruction});
                        }
                        instruction->selfdestruct = true;
                    } catch (std::invalid_argument ex) {
                        printf("invalid frame trigger in line %d, skipping line", line);
                        abort = true;
                    }
                } else if(trigger_type == "none") {
                    //  this trigger shouldn't do shit
                    //  just insert the instruction id
                    if(script->instructions->count(instruct_id) > 0) {
                        instruction = script->instructions->at(instruct_id);
                    } else {
                        instruction = new script_instruction();
                        script->instructions->insert({instruct_id, instruction});
                    }
                    instruction->selfdestruct = true;
                } else if(trigger_type == "noneCont") {
                    if(script->instructions->count(instruct_id) > 0) {
                        instruction = script->instructions->at(instruct_id);
                    } else {
                        instruction = new script_instruction();
                        script->instructions->insert({instruct_id, instruction});
                    }
                } else if(trigger_type == "frameCont") {
                    //  same as frame, but selfdestruct is false
                    try {
                        //  get frame number
                        uint32_t frame = std::stoul(content.substr(next, offset), nullptr);
                        printf("frame %d:", frame);
                        //  check if frame data exists
                        if(script->frame_triggers->count(frame) == 0) {
                            //  doesn't exist, create the vector
                            std::vector<uint32_t> *v = new std::vector<uint32_t>();
                            script->frame_triggers->insert({frame, v});
                        }
                        //  insert instruction ID into vector
                        script->frame_triggers->at(frame)->push_back(instruct_id);
                        if(script->instructions->count(instruct_id) > 0) {
                            instruction = script->instructions->at(instruct_id);
                        } else {
                            instruction = new script_instruction();
                            script->instructions->insert({instruct_id, instruction});
                        }
                    } catch (std::invalid_argument ex) {
                        printf("invalid frame trigger in line %d, skipping line", line);
                        abort = true;
                    }
                // } else if(trigger_type == "interval") {
                //     //  add an instruction to this instruction set frameTriggerOffset

                //     try {

                //     } catch (std::invalid_argument ex) {
                //         printf("invalid frame trigger in line %d, skipping line", line);
                //         abort = true;
                //     }
                // } else if(trigger_type == "intervalStart") {
                //     //  same as interval 

                } else {
                    printf("unknown trigger in line %d, skipping line", line);
                    abort = true;
                }
                //  no other trigger types for now
                //  skip closing bracket, spaces and colon to functions
                next += offset + 1;
                offset = 0;
                while(next + offset < length && (content[next + offset] == ' ' || content[next + offset] == ':')) next++;

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
                                //  unsigned int, probably only used for start and stop because frame ids
                                //  single int
                                try {
                                    uint32_t arg_1 = std::stoul(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    //  additional logic for start and stop, gotta create an entry in the id map
                                    if(function_name == "start" || function_name == "stop") {
                                        if(script->id_map->count(arg_1) > 0) {
                                            //  there's an entry already, switch the argument to the new id
                                            arg_1 = script->id_map->at(arg_1);
                                        } else {
                                            //  no entry, create one
                                            script->id_map->insert({arg_1, nextId});
                                            arg_1 = nextId;
                                            nextId++;
                                        }
                                    }
                                    script_args args;
                                    args.type_3 = arg_1;
                                    instruction->val->push_back(args);
                                    printf("(uint %d)", arg_1);
                                } catch (std::invalid_argument ex) {
                                    success = false;
                                    printf(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            } else if(function_info.second == 4) {
                                //  two unsigned ints
                                try {
                                    uint32_t arg_1 = std::stoul(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    uint32_t arg_2 = std::stoul(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    script_args args;
                                    args.type_4 = script_setIntInt(arg_1, arg_2);
                                    instruction->val->push_back(args);
                                    printf("(uint %d, uint %d)", arg_1, arg_2);
                                } catch (std::invalid_argument ex) {
                                    success = false;
                                    printf(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            }
                            //  ignore type 3 for the moment
                            // } else if(function_info.second == 3) {
                            //     //  float, int
                            //     try {
                            //         float arg_1 = std::stof(content.substr(next, offset), nullptr);
                            //         while(content[next] != ',' && offset > 0) {
                            //             next++;
                            //             offset--;   
                            //         }
                            //         int arg_2 = std::stoi(content.substr(next, offset), nullptr);
                            //         while(content[next] != ',' && offset > 0) {
                            //             next++;
                            //             offset--;   
                            //         }
                            //         script_args args;
                            //         uint64_t arg = 
                            //         args.type_3 = new std::tuple<float, int>(arg_1, arg_2);
                            //         instruction->val->push_back(args);
                            //         printf("(float %f, int %d)", arg_1, arg_2);
                            //     } catch (std::invalid_argument ex) {
                            //         success = false;
                            //         printf(", can't read argument in function %s, line %d", ex.what(), line);
                            //     }
                            // }

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
        printf("finished loading script\n");
        return script;
    }

    uint64_t script_setIntInt(uint32_t first, uint32_t second) {
        uint64_t out = 0;
        out += first;
        out += ((uint64_t)second) << 32;
        return out;
    }

    std::pair<uint32_t, uint32_t> script_getIntInt(uint64_t in) {
        std::pair<uint32_t, uint32_t> out;
        out.first = (uint32_t)in;
        out.second = (uint32_t)(in >> 32);
        return out;
    }

    bullet_script::bullet_script() {
        id_map = new std::unordered_map<uint32_t, uint32_t>();
        frame_triggers = new std::unordered_map<uint32_t, std::vector<uint32_t>*>();
        instructions = new std::unordered_map<uint32_t, script_instruction*>();
    }

    bullet_script::~bullet_script() {
        //  iterators, not sure what format these are
        auto fit_begin = frame_triggers->begin();
        auto fit_end = frame_triggers->end();
        while(fit_begin != fit_end) {
            delete fit_begin->second;
            fit_begin++;
        }
        delete frame_triggers;
        auto iit_begin = instructions->begin();
        auto iit_end = instructions->end();
        while(iit_begin != iit_end) {
            delete iit_begin->second;
            iit_begin++;
        }
        delete instructions;
        delete id_map;
    }


    script_instruction::script_instruction() {
        instruct = new std::vector<uint32_t>();
        val = new std::vector<script_args>();
    }

    script_instruction::~script_instruction() {
        delete instruct;
        delete val;
    }

}