#include "script.hpp"

namespace Game {
    asIScriptEngine *scriptEngine;
    asIScriptModule *mod;
    asIScriptFunction *func;
    asIScriptContext *ctx;

    void print(std::string &msg)
    {
        printf("%s", msg.c_str());
    }
    

    void initScriptEngine() {
        scriptEngine = asCreateScriptEngine();
        // int r = scriptEngine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
        // assert(r >= 0);
        RegisterStdString(scriptEngine);
        scriptEngine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
        
        std::string content;
        std::ifstream testScript("./test.as", std::ifstream::in);
        if(testScript.good()) {
            std::stringstream filestream;
            filestream << testScript.rdbuf();
            content = filestream.str();
        }
        testScript.close();
        CScriptBuilder builder;
        builder.StartNewModule(scriptEngine, "test");
        builder.AddSectionFromFile("./test.as");
        builder.BuildModule();
    }

    void RunScript() {
        mod = scriptEngine->GetModule("test");
        func = mod->GetFunctionByDecl("void main()");
        ctx = scriptEngine->CreateContext();
        ctx->Prepare(func);
        ctx->Execute();
    }

    void CleanUp() {
        ctx->Release();
        scriptEngine->ShutDownAndRelease();
    }
}