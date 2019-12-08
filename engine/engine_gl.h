#ifndef _ENGINE_GL_H
#define _ENGINE_GL_H

#include "glad/glad.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#include "stb_image.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace engine {
    extern bool quit;
    extern bool keyState[83];
    extern bool keyPressed[83];
    void inputs();
    enum keyCodes{
        kbLeft, kbRight, kbUp, kbDown,
        kbEscape, kbF1, kbF2, kbF3, kbF4, kbF5, kbF6, kbF7, kbF8, kbF9, kbF10, kbF11, kbF12,
        kbTilde, kb1, kb2, kb3, kb4, kb5, kb6, kb7, kb8, kb9, kb0, kbMinus, kbEquals, kbBackspace,
        kbTab, kbQ, kbW, kbE, kbR, kbT, kbY, kbU, kbI, kbO, kbP, kbLBracket, kbRBracket, kbBackslash,
        kbA, kbS, kbD, kbF, kbG, kbH, kbJ, kbK, kbL, kbColon, kbQuote, kbEnter,
        kbLShift, kbZ, kbX, kbC, kbV, kbB, kbN, kbM, kbPeriod, kbComma, kbSlash, kbRShift,
        kbLCtrl, kbLWin, kbLAlt, kbSpace, kbRAlt, kbRWin, kbMenu, kbRCtrl,
        kbInsert, kbHome, kbPgup, kbDelete, kbEnd, kbPgdn
    };

    namespace gl {
        extern SDL_Window *window;
        extern SDL_GLContext maincontext;


        class VAO {
            public:
                GLuint *ID;
                void init();
                void bind();
                void remove();
                static void unbind();
        };

        class VBO {
            public:
                GLuint *ID_VBO, *ID_EBO;
                int vertexAttribs;
                void init();
                void bind();
                void remove();
                static void unbind();

                void bufferVerts(int, float*, int, uint32_t*);
                void createVertexAttribPointer(int, GLenum, GLsizei, const void*);
                void createVertexAttribPointerNormalized(int, GLenum, GLsizei, const void*);

        };

        class Texture {
            public:
                GLuint *ID;
                int srcWidth, srcHeight, srcChanels;
                void init();
                void bind();
                void load(const std::string&);
                void remove();
                static void unbind();
        };

        class Shader
        {
            public:
                //program id
                uint32_t ID;

                //reads and builds the shader
                void load(const GLchar*, const GLchar*);
                //use/activate the shader
                void use();
                //utility uniform functions
                void setBool(const std::string&, bool) const;
                void setInt(const std::string&, int) const;
                void setFloat(const std::string&, float) const;
                void setVec2(const std::string&, const glm::vec2&) const;
                void setVec2(const std::string&, float, float) const;
                void setVec3(const std::string&, const glm::vec3&) const;
                void setVec3(const std::string&, float, float, float) const;
                void setVec4(const std::string&, const glm::vec4&) const;
                void setVec4(const std::string&, float, float, float, float) const;
                void setMat2(const std::string&, const glm::mat2&) const;
                void setMat3(const std::string&, const glm::mat3&) const;
                void setMat4(const std::string&, const glm::mat4&) const;
        };

    }    
}





#endif