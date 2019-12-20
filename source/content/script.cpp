#include "bullet.h"
#include "script.h"

namespace game::content {

    //  bullet instructions
    std::unordered_map<std::string, void (bullet_s::*)(float)> *bullet_instr = nullptr;

    void script_init() {
        if(bullet_instr == nullptr) {
            bullet_instr = new std::unordered_map<std::string, void (bullet_s::*)(float)>();
            bullet_instr->insert({"accel", &bullet_s::instr_accel});
            bullet_instr->insert({"angle_change", &bullet_s::instr_angle_change});
            bullet_instr->insert({"type_change", &bullet_s::instr_type_change});
            bullet_instr->insert({"speed", &bullet_s::instr_speed});
        }
    }


    std::unordered_map<int, script_instruction_bullet*> *loadScriptBullet(const std::string &path) {
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

        //  parse content string into mapped sections
        std::unordered_map<int, script_instruction_bullet*> *script = new std::unordered_map<int, script_instruction_bullet*>();

        //  loop
        while(content[next] != '\0') {
            if(content[next] == '\n') {
                next++; //  skip blank newlines
            } else {
                //  instruction frame
                int frame = std::stoi(content.substr(next), nullptr);
                while(content[next] != '\0' && content[next] != ':') {
                    next++;
                }
                next++;
                offset = 0;
                while(content[next + offset] != '\0' && content[next + offset] != ';') {
                    //  just go until either EOF or end of instruction
                    offset++;
                }

                script_instruction_bullet *i;

                //  check if frame data exists
                if(script->count(frame) > 0) {
                    //  exists
                    i = script->at(frame);
                } else {
                    //  doesnt exist
                    i = new script_instruction_bullet();
                    script->insert({frame, i});
                }

                // std::string instr = content.substr(next, offset);
                size_t instr_offset = 0;
                while(instr_offset < offset && content[next + instr_offset] != '(') {
                    instr_offset++;
                }
                try {
                    i->instruct->push_back(bullet_instr->at(content.substr(next, instr_offset)));
                    printf("script instruction #%d: %s", frame, content.substr(next, instr_offset).c_str());
                    instr_offset++;
                    i->val->push_back(std::strtof(content.substr(next + instr_offset, offset).c_str(), nullptr));
                    printf(" %s\n", content.substr(next + instr_offset, offset).c_str());
                } catch (std::out_of_range e) {
                    //  instruction doesnt exist
                    //  dont do anything i guess
                    printf("instruction %s doesn't exist\n", content.substr(next, offset).c_str());
                }
                
                // printf("script instruction #%d: %s\n", frame, content.substr(next, offset).c_str());
                next += offset;
                while(content[next] != '\0' && content[next] != '\n') next++;
            }
        }

        return script;
    }

    script_instruction_bullet::script_instruction_bullet() {
        instruct = new std::vector<void (bullet_s::*)(float)>();
        val = new std::vector<float>();
    }

    script_instruction_bullet::~script_instruction_bullet() {
        delete instruct;
        delete val;
    }
}