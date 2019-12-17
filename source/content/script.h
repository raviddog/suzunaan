#ifndef _SCRIPT_H
#define _SCRIPT_H

#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
//  loads a script file into an std::unordered_map

namespace game::content {
    std::unordered_map<int, std::string>* loadScript(const std::string &path);
}

#endif