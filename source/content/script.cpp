#include "script.h"

namespace game::content {
    std::unordered_map<int, std::string>* loadScript(const std::string &path) {
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
            printf("failed to load script file %s\n", path);
        }

        //  parse content string into mapped sections
        std::unordered_map<int, std::string> *script = new std::unordered_map<int, std::string>();

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
                
                script->insert({frame, content.substr(next, offset)});
                printf("script instruction #%d: %s\n", frame, content.substr(next, offset).c_str());
                next += offset;
                while(content[next] != '\0' && content[next] != '\n') next++;
            }
        }

        return script;
    }
}