#include "script.hpp"
#include "debug.hpp"

#include <fstream>
#include <sstream>
// #include <iostream>

namespace Game {

    uint64_t script_setIntInt(int, int);
    uint64_t script_setFloatFloat(float, float);

     //  load instructions  
    
    bullet_script *loadScriptBullet(const std::string&);
    enemy_script *loadScriptEnemy(const std::string&);
    stage_script *loadScriptStage(const std::string&);

    /*
        LISTENER IDS
            try to keep them mostly similar


        BULLET LISTENER IDS
        1 - distToPlayer


        ENEMY LISTENER IDS
        2 - init
        
    */

    //  instruction id converter thingies
    std::map<std::string, std::pair<int, int>> *stage_instr = nullptr;
    std::map<std::string, std::pair<int, int>> *bullet_instr = nullptr;
    std::map<std::string, std::pair<int, int>> *enemy_instr = nullptr;

    void script_init() {
        if(bullet_instr == nullptr) {
            bullet_instr = new std::map<std::string, std::pair<int, int>>();
            bullet_instr->insert({"speed", std::make_pair(1, 1)});
            bullet_instr->insert({"accel", std::make_pair(2, 1)});
            bullet_instr->insert({"angle_change", std::make_pair(3, 1)});
            bullet_instr->insert({"type_set_relative", std::make_pair(4, 2)});
            bullet_instr->insert({"angle", std::make_pair(5, 1)});
            bullet_instr->insert({"stop", std::make_pair(6, 3)});
            bullet_instr->insert({"start", std::make_pair(7, 3)});
            bullet_instr->insert({"frameTrigger", std::make_pair(8, 4)});
            bullet_instr->insert({"frameTriggerOffset", std::make_pair(9, 4)});
            bullet_instr->insert({"stopInterval", std::make_pair(10, 3)});
            bullet_instr->insert({"angle_atPlayer", std::make_pair(11, 0)});
            bullet_instr->insert({"random_angle_change", std::make_pair(12, 5)});
            bullet_instr->insert({"random_angle", std::make_pair(13, 5)});
        }

        if(enemy_instr == nullptr) {
            enemy_instr = new std::map<std::string, std::pair<int, int>>();
            enemy_instr->insert({"speed", std::make_pair(1, 1)});
            enemy_instr->insert({"angle", std::make_pair(2, 1)});
            enemy_instr->insert({"accel", std::make_pair(3, 1)});
            //  put various bullet functions at 100+, because they need to insert bullet spawns
            //  also all functions should insert the same function ID because it reads from the same spawn struct
            enemy_instr->insert({"bullet", std::make_pair(4, 100)});
            enemy_instr->insert({"setHP", std::make_pair(5, 2)});
            enemy_instr->insert({"stop", std::make_pair(6, 3)});
            enemy_instr->insert({"start", std::make_pair(7, 3)});
            enemy_instr->insert({"frameTrigger", std::make_pair(8, 4)});
            enemy_instr->insert({"frameTriggerOffset", std::make_pair(9, 4)});
            enemy_instr->insert({"stopInterval", std::make_pair(10, 3)});
            enemy_instr->insert({"enemy", std::make_pair(11, 200)});
            enemy_instr->insert({"angle_change", std::make_pair(12, 1)});
            enemy_instr->insert({"bullet_ring", std::make_pair(13, 101)});
            enemy_instr->insert({"moveToParent", std::make_pair(14, 0)});
        }

        if(stage_instr == nullptr) {
            stage_instr = new std::map<std::string, std::pair<int, int>>();
            stage_instr->insert({"bullet", std::make_pair(1, 100)});
            stage_instr->insert({"enemy", std::make_pair(2, 200)});
        }

    }

    void script_cleanup() {
        delete stage_instr;
        delete enemy_instr;
        delete bullet_instr;

        stage_instr = nullptr;
        enemy_instr = nullptr;
        bullet_instr = nullptr;
    }

    int loadScript(const std::string &path, stage_script **stageptr, std::map<int, std::shared_ptr<enemy_script>> **enemy_scripts_ptr, std::map<int, std::shared_ptr<bullet_script>> **bullet_scripts_ptr) {
        //  path to stage loader script
        //  load the stage script and the listed enemy and bullet scripts
        //  should be the only thing needed to be called from stage.cpp
        //  TODO pass in bullet script array thingy and enemy shit to stage load into thing  blabla

        
        script_init();
        engine::log_debug("loading scripts from %s\n", path.c_str());

        //  get directory location of this script loader file
        std::string dir = "./";
        int path_length = path.length();
        do {
            --path_length;
        } while(path_length > -1 && path[path_length] != '/');

        if(path_length < 0) {
            //  local directory
            //  just leave default
        } else {
            dir = path.substr(0, path_length + 1);
            
        }

        stage_script *stage = *stageptr;
        std::map<int, std::shared_ptr<enemy_script>> *enemy_scripts = *enemy_scripts_ptr;
        std::map<int, std::shared_ptr<bullet_script>> *bullet_scripts = *bullet_scripts_ptr;



        //  section
        //  0 = type and stuff
        //  1 = stage scripte
        //  2 = enemy scripts
        //  3 = bullet scripts
        int section = 0;

        std::string content;
        size_t next = 0, offset = 0, length;
        int line = 1;
        int files = 0;
        std::ifstream file;

        file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try {
            file.open(path);
            std::stringstream filestream;
            filestream << file.rdbuf();
            file.close();
            content = filestream.str();
        } catch(std::ifstream::failure &ex) {
            engine::log_debug("failed to open file, %s\n", ex.what());
        }

        length = content.length();

        //  read script type in first line
        while(next + offset < length && content[next + offset] != ':') offset++;
        if(content.substr(next, offset) != "type") {
            //  no type header in script file
            engine::log_debug("no type header in script, aborting\n");
            return -1;
        }

        //  skip past colon and spaces
        offset++;
        while(content[next + offset] == ' ') offset++;
        next += offset;
        offset = 0;
        while(content[next + offset] != '\n' && next + offset < length) offset++;

        std::string script_type = content.substr(next, offset);
        if(script_type == "enemy") {
            engine::log_debug("enemy script detected instead, aborting");
            return -1;
        } else if(script_type == "stage") {
            engine::log_debug("stage script detected instead, aborting");
            return -1;
        } else if(script_type == "bullet") {
            engine::log_debug("bullet script detected instead, aborting");
            return -1;
        } else if(script_type != "scriptloader") {
            engine::log_debug("unknown script type, aborting\n");
            return -1;
        }

        //  its a real file

        //  TODO
        //  NEED TO CLEAR THESE PROPERLY
        //  AAAAAAAAAAAAAAAAAAAAAAAA
        if(stage) delete stage;
        if(enemy_scripts) delete enemy_scripts;
        if(bullet_scripts) delete bullet_scripts;

        enemy_scripts = new std::map<int, std::shared_ptr<enemy_script>>();
        bullet_scripts = new std::map<int, std::shared_ptr<bullet_script>>();

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
                while(next < length && content[next] != '\n') next++;
            } else {
                //  a line which actually has stuff on it
                if(content[next] == '#') {
                    //  change section type
                    next++;
                    while(content[next + offset] != '\n') offset++;
                    std::string sectionID = content.substr(next, offset);
                    if(sectionID == "stage") {
                        section = 1;
                    } else if(sectionID == "enemy") {
                        section = 2;
                    } else if(sectionID == "bullet") {
                        section = 3;
                    }
                    next += offset;
                    offset = 0;
                } else if(section == 1) {
                    // TODO do error checking later
                    while(content[next + offset] != '\n') offset++;
                    //  substring is stage script file location
                    stage = loadScriptStage(dir + content.substr(next, offset));
                    next += offset;
                    offset = 0;
                    ++files;
                } else if(section == 2) {
                    //  TODO do error checking later
                    //  get id
                    while(content[next + offset] != ':') offset++;
                    int id = std::stol(content.substr(next, offset), nullptr);
                    next += offset;
                    offset = 0;
                    //  skip colon
                    ++next;
                    //  get file
                    while(content[next + offset] != '\n') offset++;
                    //  substring is enemy script file location
                    std::shared_ptr<enemy_script> temp(loadScriptEnemy(dir + content.substr(next, offset)));
                    next += offset;
                    offset = 0;
                    enemy_scripts->insert({id, temp});
                    ++files;
                } else if(section == 3) {
                    //  TODO do error checking later
                    //  get id
                    while(content[next + offset] != ':') offset++;
                    int id = std::stol(content.substr(next, offset), nullptr);
                    next += offset;
                    offset = 0;
                    //  skip colon
                    ++next;
                    //  get file
                    while(content[next + offset] != '\n') offset++;
                    //  substring is enemy script file location
                    std::shared_ptr<bullet_script> temp(loadScriptBullet(dir + content.substr(next, offset)));
                    next += offset;
                    offset = 0;
                    bullet_scripts->insert({id, temp});
                    ++files;
                }
            }
        }
        *stageptr = stage;
        *enemy_scripts_ptr = enemy_scripts;
        *bullet_scripts_ptr = bullet_scripts;

        engine::log_debug("loaded %d files\n", files);
        return 0;
    }

    bullet_script *loadScriptBullet(const std::string &path) {
        engine::log_debug("loading bullet script file %s\n", path.c_str());
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
        } catch(std::ifstream::failure &ex) {
            engine::log_debug("failed to open file, %s\n", ex.what());
        }

        length = content.length();

        //  read script type in first line
        while(next + offset < length && content[next + offset] != ':') offset++;
        if(content.substr(next, offset) != "type") {
            //  no type header in script file
            engine::log_debug("no type header in script, aborting\n");
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
            engine::log_debug("enemy script detected instead, aborting");
            return nullptr;
        } else if(script_type == "stage") {
            engine::log_debug("stage script detected instead, aborting");
            return nullptr;
        } else if(script_type != "bullet") {
            engine::log_debug("unknown script type, aborting\n");
            return nullptr;
        }

        bullet_script *script = new bullet_script();
        int nextId = 0;

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
                while(next < length && content[next] != '\n') next++;
            } else {
                //  process instruction
                //  check if the line has an id in it by counting colons
                //  this is a really shitty jank way of doing it
                int colons = 0;
                int instruct_id = 0;
                while(next + offset < length && content[next + offset] != '\n') {
                    if(content[next + offset] == ':') colons++;
                    offset++;
                }
                //  2 colons, has id
                if(colons == 2) {
                    //  read id
                    try {
                        instruct_id = std::stol(content.substr(next, offset), nullptr);
                        engine::log_debug("id: %u ", instruct_id);
                    } catch (std::invalid_argument &ex) {
                        //  invalid id, use nextId
                        engine::log_debug("invalid id, ignoring. ");
                        instruct_id = nextId;
                        nextId--;
                    }
                    //  position next at the trigger function name
                    while(next < length && content[next] != ':') next++;
                    next++;
                } else {
                    //  no id given, just use nextId
                    instruct_id = nextId;
                    nextId--;
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
                script_instruction *instruction = nullptr;
                bool abort = false;
                if(trigger_type == "frame") {
                    //  add to frame triggers
                    try {
                        //  get frame number
                        int frame = std::stol(content.substr(next, offset), nullptr);
                        //  check if frame data exists
                        if(script->frame_triggers->count(frame) == 0) {
                            //  doesn't exist, create the vector
                            std::vector<int> *v = new std::vector<int>();
                            script->frame_triggers->insert({frame, v});
                        }
                        //  insert instruction ID into frame triggers
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
                        engine::log_debug("trigger: frame %u ", frame);
                    } catch (std::invalid_argument &ex) {
                        engine::log_debug("invalid frame trigger in line %d, skipping line", line);
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
                    engine::log_debug("trigger: none ");
                } else if(trigger_type == "noneCont") {
                    if(script->instructions->count(instruct_id) > 0) {
                        instruction = script->instructions->at(instruct_id);
                    } else {
                        instruction = new script_instruction();
                        script->instructions->insert({instruct_id, instruction});
                    }
                    engine::log_debug("trigger: none (continous) ");
                } else if(trigger_type == "frameCont") {
                    //  same as frame, but selfdestruct is false
                    try {
                        //  get frame number
                        int frame = std::stol(content.substr(next, offset), nullptr);
                        engine::log_debug("frame %u:", frame);
                        //  check if frame data exists
                        if(script->frame_triggers->count(frame) == 0) {
                            //  doesn't exist, create the vector
                            std::vector<int> *v = new std::vector<int>();
                            script->frame_triggers->insert({frame, v});
                        }
                        //  insert instruction ID into frame triggers
                        script->frame_triggers->at(frame)->push_back(instruct_id);
                        //  insert instruction
                        if(script->instructions->count(instruct_id) > 0) {
                            instruction = script->instructions->at(instruct_id);
                        } else {
                            instruction = new script_instruction();
                            script->instructions->insert({instruct_id, instruction});
                        }
                        engine::log_debug("trigger: frame %u (continuous) ", frame);
                    } catch (std::invalid_argument &ex) {
                        engine::log_debug("invalid frame trigger in line %d, skipping line", line);
                        abort = true;
                    }
                } else if(trigger_type == "interval") {
                    try {
                        //  get arguments
                        int interval = std::stol(content.substr(next, offset), nullptr);
                        //  no initial trigger, just insert instruction ID
                        if(script->instructions->count(instruct_id) > 0) {
                            instruction = script->instructions->at(instruct_id);
                        } else {
                            instruction = new script_instruction();
                            script->instructions->insert({instruct_id, instruction});
                        }
                        //  instruction is self destructing, but calls itself later with frameTriggerOffset so it doesn't matter
                        instruction->selfdestruct = true;
                        //  create and insert frameTriggerOffset into the instructions
                        script_args args;
                        args.type_4 = script_setIntInt(instruct_id, interval);
                        instruction->instruct->push_back(9);
                        instruction->val->push_back(args);
                        engine::log_debug("trigger: interval %u ", interval);
                    } catch (std::invalid_argument &ex) {
                        engine::log_debug("invalid frame trigger in line %d, skipping line", line);
                        abort = true;
                    }
                } else if(trigger_type == "intervalStart") {
                    //  same as interval but with a frame trigger
                    try {
                        //  get arguments
                        int interval = std::stol(content.substr(next, offset), nullptr);
                        while(content[next] != ',' && offset > 0) {
                            next++;
                            offset--;
                        }
                        next++;
                        offset--;
                        int frame = std::stol(content.substr(next, offset), nullptr);
                        //  check if frame data exists
                        if(script->frame_triggers->count(frame) == 0) {
                            //  doesn't exist, create the vector
                            std::vector<int> *v = new std::vector<int>();
                            script->frame_triggers->insert({frame, v});
                        }
                        //  insert instruction ID into frame triggers
                        script->frame_triggers->at(frame)->push_back(instruct_id);
                        //  insert instruction
                        if(script->instructions->count(instruct_id) > 0) {
                            instruction = script->instructions->at(instruct_id);
                        } else {
                            instruction = new script_instruction();
                            script->instructions->insert({instruct_id, instruction});
                        }
                        //  instruction is self destructing, but calls itself later with frameTriggerOffset so it doesn't matter
                        instruction->selfdestruct = true;
                        //  create and insert frameTriggerOffset into the instructions
                        script_args args;
                        //  get the supplied user id for the trigger id, or generate one if not supplied
                        args.type_4 = script_setIntInt(instruct_id, interval);
                        instruction->instruct->push_back(9);
                        instruction->val->push_back(args);
                        engine::log_debug("trigger: frame %u interval %u ", frame, interval);
                    } catch (std::invalid_argument &ex) {
                        engine::log_debug("invalid frame trigger in line %d, skipping line", line);
                        abort = true;
                    }
                } else if(trigger_type == "init") {
                    //  insert listener argument
                    std::pair<script_args, int> args;
                    args.second = instruct_id;
                    //  there's not really any arguments for this trigger
                    //  just insert a blank one?
                    script->listeners->insert({2u, args});
                    //  create instruction
                    if(script->instructions->count(instruct_id) > 0) {
                        instruction = script->instructions->at(instruct_id);
                    } else  {
                        instruction = new script_instruction();
                        script->instructions->insert({instruct_id, instruction});
                    }
                    instruction->selfdestruct = true;
                    engine::log_debug("trigger: init ");
                } else if(trigger_type == "distToPlayer") {
                    try {
                        //  get distance
                        float distance = std::stof(content.substr(next, offset), nullptr);
                        //  prepare listener arguments
                        std::pair<script_args, int> args;
                        args.first.type_1 = distance;
                        args.second = instruct_id;
                        //  place into listeners
                        script->listeners->insert({1u, args});
                        //  create instruction
                        if(script->instructions->count(instruct_id) > 0) {
                            instruction = script->instructions->at(instruct_id);
                        } else {
                            instruction = new script_instruction();
                            script->instructions->insert({instruct_id, instruction});
                        }
                        //  if you want it to be continuous, just make it call a continuous function
                        instruction->selfdestruct = true;
                        engine::log_debug("trigger: distToPlayer %f ", distance);
                    } catch (std::invalid_argument &ex) {
                        engine::log_debug("invalid distance in line %d, skipping line", line);
                        abort = true;
                    }
                } else {
                    engine::log_debug("unknown trigger in line %d, skipping line", line);
                    abort = true;
                }
                //  no other trigger types for now
                //  skip closing bracket, spaces and colon to functions
                next += offset + 1;
                offset = 0;
                while(next + offset < length && (content[next + offset] == ' ' || content[next + offset] == ':')) next++;

                if(!abort) {
                    //  read functions until semicolon terminator
                    while(next < length && content[next] != ';') {
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
                            engine::log_debug(" function %s", function_name.c_str());
                            function_info = bullet_instr->at(function_name);
                            //  only insert function if arguments are successfully inserted
                            bool success = true;                                

                            //  get arguments
                            if(function_info.second == 0) {
                                //  no arguments, insert blank arg data
                                script_args args;
                                args.type_1 = 0.f;
                                instruction->val->push_back(args);

                            } else if(function_info.second == 1) {
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
                                    engine::log_debug("(float %f)", arg_1);
                                } catch (std::invalid_argument &ex) {
                                    success = false;
                                    engine::log_debug(", can't read argument in function %s, line %d", ex.what(), line);
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
                                    engine::log_debug("(int %d)", arg_1);
                                } catch (std::invalid_argument &ex) {
                                    success = false;
                                    engine::log_debug(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            } else if(function_info.second == 3) {
                                //  unsigned int, probably only used for start and stop because frame ids
                                //  single int
                                try {
                                    int arg_1 = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    script_args args;
                                    args.type_2 = arg_1;
                                    instruction->val->push_back(args);
                                    engine::log_debug("(int %u)", arg_1);
                                } catch (std::invalid_argument &ex) {
                                    success = false;
                                    engine::log_debug(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            } else if(function_info.second == 4) {
                                //  two unsigned ints
                                try {
                                    int arg_1 = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    int arg_2 = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    script_args args;
                                    args.type_4 = script_setIntInt(arg_1, arg_2);
                                    instruction->val->push_back(args);
                                    engine::log_debug("(int %u, int %u)", arg_1, arg_2);
                                } catch (std::invalid_argument &ex) {
                                    success = false;
                                    engine::log_debug(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            } else if(function_info.second == 5) {
                                //  two floats
                                try {
                                    float arg_1 = std::stof(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    float arg_2 = std::stof(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    script_args args;
                                    args.type_5 = script_setFloatFloat(arg_1, arg_2);
                                    instruction->val->push_back(args);
                                    engine::log_debug("(float %f, float %f)", arg_1, arg_2);
                                } catch  (std::invalid_argument &ex) {
                                    success = false;
                                    engine::log_debug(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            }
                            if(success) {
                                if(instruction) instruction->instruct->push_back(function_info.first);
                            }

                        } else {
                            engine::log_debug("unable to locate instruction %s\n", function_name.c_str());
                        }
                        next += offset + 1;
                    }
                }
                while(next < length && content[next] != '\n') next++;
                engine::log_debug("\n");
            }
        }
        engine::log_debug("finished loading script\n");
        return script;
    }

    //  basically the same as load bullet, but with different trigger detection
    //  also returns a different type, so can't merge the two
    //  i want to be able to change the enemy/stage/bullet script structs in the future
    //  so i'm keeping them separate
    enemy_script *loadScriptEnemy(const std::string &path) {
        engine::log_debug("loading enemy script file %s\n", path.c_str());
        std::string content;
        size_t next = 0, offset = 0, length;
        int line = 1;
        std::ifstream file;

        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            file.open(path);
            std::stringstream filestream;
            filestream << file.rdbuf();
            file.close();
            content = filestream.str();
        } catch (std::ifstream::failure &ex) {
            engine::log_debug("failed to open file, %s\n", ex.what());
        }

        length = content.length();

        //  read script type in first line
        while(next + offset < length && content[next + offset] != ':') offset++;
        if(content.substr(next, offset) != "type") {
            //  no type header in script file
            engine::log_debug("no type header in script, aborting\n");
            return nullptr;
        }

        //  skip past colon and spaces
        offset++;
        while(content[next + offset] == ' ') offset++;
        next += offset;
        offset = 0;
        while(content[next + offset] != '\n' && next + offset < length) offset++;

        std::string script_type = content.substr(next, offset);
        if(script_type == "bullet") {
            engine::log_debug("bullet script detected instead, aborting\n");
            return nullptr;
        } else if(script_type == "stage") {
            engine::log_debug("stage script detected instead, aborting\n");
            return nullptr;
        } else if(script_type != "enemy") {
            engine::log_debug("unknown script type, aborting\n");
            return nullptr;
        }

        enemy_script *script = new enemy_script();
        int nextId = 0, enemy_id = 0;

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
                while(next < length && content[next] != '\n') next++;
            } else {
                //  process instruction
                //  check if the line has an id in it by counting colons
                //  this is a really shitty jank way of doing it
                int colons = 0;
                int instruct_id = 0;
                //  next free bullet_spawn id
                int bullet_id = 0;
                while(next + offset < length && content[next + offset] != '\n') {
                    if(content[next + offset] == ':') colons++;
                    offset++;
                }
                //  2 colons, has id
                if(colons == 2) {
                    //  read id
                    try {
                        instruct_id = std::stol(content.substr(next, offset), nullptr);
                        engine::log_debug("id: %u ", instruct_id);
                    } catch (std::invalid_argument &ex) {
                        //  invalid id, use nextId
                        engine::log_debug("invalid id, ignoring. ");
                        instruct_id = nextId;
                        nextId--;
                    }
                    //  position next at the trigger function name
                    while(next < length && content[next] != ':') next++;
                    next++;
                } else {
                    //  no id given, just use nextId
                    instruct_id = nextId;
                    nextId--;
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
                script_instruction *instruction = nullptr;
                bool abort = false;

                if(trigger_type == "frame") {
                    //  add to frame triggers
                    try {
                        //  get frame number
                        int frame = std::stol(content.substr(next, offset), nullptr);
                        //  check if frame data exists
                        if(script->frame_triggers->count(frame) == 0) {
                            //  doesn't exist, create the vector
                            std::vector<int> *v = new std::vector<int>();
                            script->frame_triggers->insert({frame, v});
                        }
                        //  insert instruction ID into frame triggers
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
                        engine::log_debug("trigger: frame %u ", frame);
                    } catch (std::invalid_argument &ex) {
                        engine::log_debug("invalid frame trigger in line %d, skipping line", line);
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
                    engine::log_debug("trigger: none ");
                } else if(trigger_type == "noneCont") {
                    if(script->instructions->count(instruct_id) > 0) {
                        instruction = script->instructions->at(instruct_id);
                    } else {
                        instruction = new script_instruction();
                        script->instructions->insert({instruct_id, instruction});
                    }
                    engine::log_debug("trigger: none (continous) ");
                } else if(trigger_type == "frameCont") {
                    //  same as frame, but selfdestruct is false
                    try {
                        //  get frame number
                        int frame = std::stol(content.substr(next, offset), nullptr);
                        engine::log_debug("frame %u:", frame);
                        //  check if frame data exists
                        if(script->frame_triggers->count(frame) == 0) {
                            //  doesn't exist, create the vector
                            std::vector<int> *v = new std::vector<int>();
                            script->frame_triggers->insert({frame, v});
                        }
                        //  insert instruction ID into frame triggers
                        script->frame_triggers->at(frame)->push_back(instruct_id);
                        //  insert instruction
                        if(script->instructions->count(instruct_id) > 0) {
                            instruction = script->instructions->at(instruct_id);
                        } else {
                            instruction = new script_instruction();
                            script->instructions->insert({instruct_id, instruction});
                        }
                        engine::log_debug("trigger: frame %u (continuous) ", frame);
                    } catch (std::invalid_argument &ex) {
                        engine::log_debug("invalid frame trigger in line %d, skipping line", line);
                        abort = true;
                    }
                } else if(trigger_type == "interval") {
                    try {
                        //  get arguments
                        int interval = std::stol(content.substr(next, offset), nullptr);
                        //  no initial trigger, just insert instruction ID
                        if(script->instructions->count(instruct_id) > 0) {
                            instruction = script->instructions->at(instruct_id);
                        } else {
                            instruction = new script_instruction();
                            script->instructions->insert({instruct_id, instruction});
                        }
                        //  instruction is self destructing, but calls itself later with frameTriggerOffset so it doesn't matter
                        instruction->selfdestruct = true;
                        //  create and insert frameTriggerOffset into the instructions
                        script_args args;
                        //  get the supplied user id for the trigger id, or generate one if not supplied
                        args.type_4 = script_setIntInt(instruct_id, interval);
                        instruction->instruct->push_back(9);
                        instruction->val->push_back(args);
                        engine::log_debug("trigger: interval %u ", interval);
                    } catch (std::invalid_argument &ex) {
                        engine::log_debug("invalid frame trigger in line %d, skipping line", line);
                        abort = true;
                    }
                } else if(trigger_type == "intervalStart") {
                    //  same as interval but with a frame trigger
                    try {
                        //  get arguments
                        int interval = std::stol(content.substr(next, offset), nullptr);
                        while(content[next] != ',' && offset > 0) {
                            next++;
                            offset--;
                        }
                        next++;
                        offset--;
                        int frame = std::stol(content.substr(next, offset), nullptr);
                        //  check if frame data exists
                        if(script->frame_triggers->count(frame) == 0) {
                            //  doesn't exist, create the vector
                            std::vector<int> *v = new std::vector<int>();
                            script->frame_triggers->insert({frame, v});
                        }
                        //  insert instruction ID into frame triggers
                        script->frame_triggers->at(frame)->push_back(instruct_id);
                        //  insert instruction
                        if(script->instructions->count(instruct_id) > 0) {
                            instruction = script->instructions->at(instruct_id);
                        } else {
                            instruction = new script_instruction();
                            script->instructions->insert({instruct_id, instruction});
                        }
                        //  instruction is self destructing, but calls itself later with frameTriggerOffset so it doesn't matter
                        instruction->selfdestruct = true;
                        //  create and insert frameTriggerOffset into the instructions
                        script_args args;
                        //  get the supplied user id for the trigger id, or generate one if not supplied
                        args.type_4 = script_setIntInt(instruct_id, interval);
                        instruction->instruct->push_back(9);
                        instruction->val->push_back(args);
                        engine::log_debug("trigger: frame %u interval %u ", frame, interval);
                    } catch (std::invalid_argument &ex) {
                        engine::log_debug("invalid frame trigger in line %d, skipping line", line);
                        abort = true;
                    }
                } else if(trigger_type == "init") {
                    //  insert listener argument
                    std::pair<script_args, int> args;
                    args.second = instruct_id;
                    //  there's not really any arguments for this trigger
                    //  just insert a blank one?
                    script->listeners->insert({2u, args});
                    //  create instruction
                    if(script->instructions->count(instruct_id) > 0) {
                        instruction = script->instructions->at(instruct_id);
                    } else  {
                        instruction = new script_instruction();
                        script->instructions->insert({instruct_id, instruction});
                    }
                    instruction->selfdestruct = true;
                    engine::log_debug("trigger: init ");
                } else if(trigger_type == "death") {
                    //  insert listener argument
                    std::pair<script_args, int> args;
                    args.second = instruct_id;
                    //  there's not really any arguments for this trigger
                    //  just insert a blank one?
                    script->listeners->insert({3u, args});
                    //  create instruction
                    if(script->instructions->count(instruct_id) > 0) {
                        instruction = script->instructions->at(instruct_id);
                    } else  {
                        instruction = new script_instruction();
                        script->instructions->insert({instruct_id, instruction});
                    }
                    instruction->selfdestruct = true;
                    engine::log_debug("trigger: death ");
                } else {
                    engine::log_debug("unknown trigger in line %d, skipping line", line);
                    abort = true;
                }
                //  no other trigger types for now
                //  skip closing bracket, spaces and colon to functions
                next += offset + 1;
                offset = 0;
                while(next + offset < length && (content[next + offset] == ' ' || content[next + offset] == ':')) next++;

                if(!abort) {
                    //  read functions until semicolon terminator
                    while(next < length && content[next] != ';') {
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

                        if(enemy_instr->count(function_name) > 0) {
                            //  load function info and insert
                            engine::log_debug(" function %s", function_name.c_str());
                            function_info = enemy_instr->at(function_name);
                            //  only insert function if arguments are successfully inserted
                            bool success = true;                                

                            //  get arguments
                            if(function_info.second == 0) {
                                //  no arguments, insert blank arg data
                                script_args args;
                                args.type_1 = 0.f;
                                instruction->val->push_back(args);

                            } else if(function_info.second == 1) {
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
                                    engine::log_debug("(float %f)", arg_1);
                                } catch (std::invalid_argument &ex) {
                                    success = false;
                                    engine::log_debug(", can't read argument in function %s, line %d", ex.what(), line);
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
                                    engine::log_debug("(int %d)", arg_1);
                                } catch (std::invalid_argument &ex) {
                                    success = false;
                                    engine::log_debug(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            } else if(function_info.second == 3) {
                                //  unsigned int, probably only used for start and stop because frame ids
                                //  single int
                                try {
                                    int arg_1 = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    script_args args;
                                    args.type_2 = arg_1;
                                    instruction->val->push_back(args);
                                    engine::log_debug("(int %u)", arg_1);
                                } catch (std::invalid_argument &ex) {
                                    success = false;
                                    engine::log_debug(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            } else if(function_info.second == 4) {
                                //  two unsigned ints
                                try {
                                    int arg_1 = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    int arg_2 = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    script_args args;
                                    args.type_4 = script_setIntInt(arg_1, arg_2);
                                    instruction->val->push_back(args);
                                    engine::log_debug("(int %u, int %u)", arg_1, arg_2);
                                } catch (std::invalid_argument &ex) {
                                    success = false;
                                    engine::log_debug(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            } else if(function_info.second == 5) {
                                //  two floats
                                try {
                                    float arg_1 = std::stof(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    float arg_2 = std::stof(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    script_args args;
                                    args.type_5 = script_setFloatFloat(arg_1, arg_2);
                                    instruction->val->push_back(args);
                                    engine::log_debug("(float %f, float %f)", arg_1, arg_2);
                                } catch  (std::invalid_argument &ex) {
                                    success = false;
                                    engine::log_debug(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            } else if(function_info.second == 100) {
                                //  bullet firing function
                                //  definitely need to modify this at some point
                                //  these values are only used by bullet firing functions
                                try {
                                    //  get function arguments
                                    int type = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    int scriptID = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    float speed = std::stof(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    float angle = std::stof(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    
                                    //  set bullet spawn properties
                                    bullet_spawn bs;
                                    bs.type = type;
                                    bs.scriptID = scriptID;
                                    bs.speed = speed;
                                    bs.angle = angle;
                                    script->bullet_spawns->insert({bullet_id, bs});

                                    script_args args;
                                    args.type_2 = bullet_id;
                                    instruction->val->push_back(args);
                                    bullet_id++;
                                    engine::log_debug("(type=%u, scriptID=%u, speed=%f, angle=%f)", type, scriptID, speed, angle);
                                } catch (std::invalid_argument &ex) {
                                    success = false;
                                    engine::log_debug(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            } else if(function_info.second == 101) {
                                try {
                                    //  get function arguments
                                    int type = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    int scriptID = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    float speed = std::stof(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    int num = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    
                                    //  set bullet spawn properties
                                    bullet_spawn bs;
                                    bs.type = type;
                                    bs.scriptID = scriptID;
                                    bs.speed = speed;
                                    script->bullet_spawns->insert({bullet_id, bs});

                                    script_args args;
                                    args.type_4 = script_setIntInt(bullet_id, num);
                                    instruction->val->push_back(args);
                                    bullet_id++;
                                    engine::log_debug("(type=%u, scriptID=%u, speed=%f, num=%d)", type, scriptID, speed, num);
                                } catch (std::invalid_argument &ex) {
                                    success = false;
                                    engine::log_debug(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            } else if(function_info.second == 200) {
                                //  enemy spawning function
                                try {
                                    //  get function arguments
                                    int type = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    int scriptID = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    float loc_x = std::stof(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    float loc_y = std::stof(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;

                                    //  set enemy spawn properties
                                    enemy_spawn es;
                                    es.type = type;
                                    es.scriptID = scriptID;
                                    es.x_offset = loc_x;
                                    es.y_offset = loc_y;
                                    script->enemy_spawns->insert({enemy_id, es});

                                    script_args args;
                                    args.type_2 = enemy_id;
                                    instruction->val->push_back(args);
                                    enemy_id++;
                                    engine::log_debug("(type=%u, scriptID=%u, x=%f, y=%f)", type, scriptID, loc_x, loc_y);
                                } catch (std::invalid_argument &ex) {
                                    success = false;
                                    engine::log_debug(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            }
                            if(success) {
                                if(instruction) instruction->instruct->push_back(function_info.first);
                            }
                        } else {
                            engine::log_debug("unable to locate instruction %s\n", function_name.c_str());
                        }
                        next += offset + 1;
                    }
                }
                while(next < length && content[next] != '\n') next++;
                engine::log_debug("\n");
            }
        }
        engine::log_debug("finished loading script\n");
        return script;
    }

    stage_script *loadScriptStage(const std::string &path) {
        engine::log_debug("loading stage script file %s\n", path.c_str());
        std::string content;
        size_t next = 0, offset = 0, length;
        int line = 1;
        std::ifstream file;

        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            file.open(path);
            std::stringstream filestream;
            filestream << file.rdbuf();
            file.close();
            content = filestream.str();
        } catch (std::ifstream::failure &ex) {
            engine::log_debug("failed to open file, %s\n", ex.what());
        }

        length = content.length();

        //  read script type in first line
        while(next + offset < length && content[next + offset] != ':') offset++;
        if(content.substr(next, offset) != "type") {
            //  no type header in script file
            engine::log_debug("no type header in script, aborting\n");
            return nullptr;
        }

        //  skip past colon and spaces
        offset++;
        while(content[next + offset] == ' ') offset++;
        next += offset;
        offset = 0;
        while(content[next + offset] != '\n' && next + offset < length) offset++;

        std::string script_type = content.substr(next, offset);
        if(script_type == "bullet") {
            engine::log_debug("bullet script detected instead, aborting\n");
            return nullptr;
        } else if(script_type == "enemy") {
            engine::log_debug("enemy script detected instead, aborting\n");
            return nullptr;
        } else if(script_type != "stage") {
            engine::log_debug("unknown script type, aborting\n");
            return nullptr;
        }

        stage_script *script = new stage_script();
        int bullet_id = 0, enemy_id = 0;

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
                while(next < length && content[next] != '\n') next++;
            } else {
                //  process instruction
                //  go until colon or newline to get frame number
                while(next + offset < length && content[next + offset] != ':' && content[next+offset] != '\n') {
                    offset++;
                }
                //  get frame
                int frame;
                script_instruction* instruction = nullptr;
                try {
                    frame = std::stol(content.substr(next, offset), nullptr);
                    if(script->frame_triggers->count(frame) == 0) {
                        //  this frame hasn't got trigger data
                        //  create the instruction
                        instruction = new script_instruction;
                        script->frame_triggers->insert({frame, instruction});
                    } else {
                        instruction = script->frame_triggers->at(frame);
                    }
                } catch (std::invalid_argument &ex) {
                    engine::log_debug("can't read frame, skipping. ");
                    //  move to end of line?
                }

                //  advance next to start of functions
                next += offset + 1;
                offset = 0;

                if(instruction) {
                    // bool abort = false;
                    //  read functions until semicolon terminator

                    while(next < length && content[next] != ';') {
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
                        if(stage_instr->count(function_name) > 0) {
                            //  load function info and insert
                            engine::log_debug(" function %s", function_name.c_str());
                            function_info = stage_instr->at(function_name);
                            //  only insert function if arguments are successfully inserted
                            bool success = true;

                            //  get arguments
                            if(function_info.second == 100) {
                                //  bullet firing function
                                //  definitely need to modify this at some point
                                //  these values are only used by bullet firing functions
                                try {
                                    //  get function arguments
                                    int type = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    int scriptID = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    float speed = std::stof(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    float angle = std::stof(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    
                                    //  set bullet spawn properties
                                    bullet_spawn bs;
                                    bs.type = type;
                                    bs.scriptID = scriptID;
                                    bs.speed = speed;
                                    bs.angle = angle;
                                    script->bullet_spawns->push_back(bs);

                                    script_args args;
                                    args.type_2 = bullet_id;
                                    instruction->val->push_back(args);
                                    bullet_id++;
                                    engine::log_debug("(type=%u, scriptID=%u, speed=%f, angle=%f)", type, scriptID, speed, angle);
                                } catch (std::invalid_argument &ex) {
                                    success = false;
                                    engine::log_debug(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            } else if(function_info.second == 200) {
                                //  enemy spawning function
                                //  definitely need to modify this at some point
                                try {
                                    //  get function arguments
                                    int type = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    int scriptID = std::stol(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    float loc_x = std::stof(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    float loc_y = std::stof(content.substr(next, offset), nullptr);
                                    while(content[next] != ',' && offset > 0) {
                                        next++;
                                        offset--;
                                    }
                                    next++;
                                    offset--;
                                    
                                    //  set enemy spawn properties
                                    enemy_spawn es;
                                    es.type = type;
                                    es.scriptID = scriptID;
                                    es.x_offset = loc_x;
                                    es.y_offset = loc_y;
                                    script->enemy_spawns->push_back(es);

                                    script_args args;
                                    args.type_2 = enemy_id;
                                    instruction->val->push_back(args);
                                    enemy_id++;
                                    engine::log_debug("(type=%u, scriptID=%u, x=%f, y=%f)", type, scriptID, loc_x, loc_y);
                                } catch (std::invalid_argument &ex) {
                                    success = false;
                                    engine::log_debug(", can't read argument in function %s, line %d", ex.what(), line);
                                }
                            }
                            if(success) {
                                if(instruction) instruction->instruct->push_back(function_info.first);
                            }
                        } else {
                            engine::log_debug("unable to locate instruction %s\n", function_name.c_str());
                        }
                        next += offset + 1;
                        offset = 0;
                    }
                }
                while(next < length && content[next] != '\n') next++;
                engine::log_debug("\n");
            }
        }
        return script;
    }



    uint64_t script_setIntInt(int first, int second) {
        uint64_t out = 0;
        out += first;
        out += ((uint64_t)second) << 32;
        return out;
    }

    uint64_t script_setFloatFloat(float first, float second) {
        int storage_1 = 0u, storage_2 = 0u;
        memcpy(&storage_1, &first, 4);
        memcpy(&storage_2, &second, 4);
        uint64_t val = storage_1;
        val += ((uint64_t)storage_2) << 32;
        return val;
    }



    std::pair<int, int> script_getIntInt(uint64_t in) {
        std::pair<int, int> out;
        out.first = (int)in;
        out.second = (int)(in >> 32);
        return out;
    }

    std::pair<float, float> script_getFloatFloat(uint64_t in) {
        int data_1, data_2;
        data_1 = in;
        data_2 = in >> 32;
        float f1, f2;
        memcpy(&f1, &data_1, 4);
        memcpy(&f2, &data_2, 4);
        return std::make_pair(f1, f2);
        
    }


    bullet_script::bullet_script() {
        frame_triggers = new std::map<int, std::vector<int>*>();
        instructions = new std::map<int, script_instruction*>();
        listeners = new std::multimap<int, std::pair<script_args, int>>();
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
        delete listeners;
    }

    enemy_script::enemy_script() {
        frame_triggers = new std::map<int, std::vector<int>*>();
        instructions = new std::map<int, script_instruction*>();
        listeners = new std::multimap<int, std::pair<script_args, int>>();
        bullet_spawns = new std::map<int, bullet_spawn>();
        enemy_spawns = new std::map<int, enemy_spawn>();
    }

    enemy_script::~enemy_script() {
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
        delete bullet_spawns;
        delete enemy_spawns;
        delete listeners;
    }


    script_instruction::script_instruction() {
        instruct = new std::vector<int>();
        val = new std::vector<script_args>();
    }

    script_instruction::~script_instruction() {
        delete instruct;
        delete val;
    }

    stage_script::stage_script() {
        bullet_spawns = new std::vector<bullet_spawn>();
        enemy_spawns = new std::vector<enemy_spawn>();
        frame_triggers = new std::map<int, script_instruction*>();
    }

    stage_script::~stage_script() {
        delete bullet_spawns;
        delete enemy_spawns;
        delete frame_triggers;

        bullet_spawns = nullptr;
        enemy_spawns = nullptr;
        frame_triggers = nullptr;
    }

}