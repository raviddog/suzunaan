#ifndef SCRIPT_H
#define SCRIPT_H

#include "angelscript.h"
#include "angelscript/scriptstdstring.h"
#include "angelscript/scriptmath.h"
#include "angelscript/scriptbuilder.h"
#include "as_jit.h"

#include <string>
#include <fstream>
#include <sstream>

namespace Game {
    void print(std::string &msg);
    void initScriptEngine();
    void RunScript();
    void CleanUp();
}

#endif