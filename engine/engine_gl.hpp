#ifndef _ENGINE_GL_H
#define _ENGINE_GL_H

#include "glad/glad.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <string>
#include <fstream>
#include <sstream>

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
                GLuint ID;
                VAO();
                ~VAO();
                void bind();
                static void unbind();
        };

        class VBO {
            public:
                GLuint ID_VBO, ID_EBO;
                int vertexAttribs;
                size_t bufferSizeVert, bufferSizeInd;
                VBO();
                ~VBO();
                void bind();
                static void unbind();

                void createBuffer(size_t vertsize);
                void createBuffer(size_t vertsize, size_t indsize);

                void bufferVerts(size_t vertsize, float *verts);
                void bufferVerts(size_t vertsize, float *verts, size_t indsize, uint32_t *indices);
                void bufferSubVerts(size_t vertsize, float *verts);
                void bufferSubVerts(size_t vertsize, float *verts, size_t indsize, uint32_t *indices);
                
                void createVertexAttribPointer(size_t size, GLenum type, GLsizei stride, const void *pointer);
                void createVertexAttribPointerNormalized(size_t size, GLenum type, GLsizei stride, const void *pointer);

        };

        class FBO
        {
            public:
                GLuint ID;

                FBO();
                ~FBO();
                void bind();
                static void unbind();
        };

        class RBO
        {
            public:
                GLuint ID;

                RBO();
                ~RBO();
                void bind();
                static void unbind();
        };

        class Texture {
            public:
                GLuint ID;
                int srcWidth, srcHeight, srcChannels;
                Texture();
                ~Texture();
                void bind();
                void load(const std::string &path);
                static void unbind();
        };

        class FrameBuffer
        {
            public:
                FrameBuffer(int w, int h);
                ~FrameBuffer();
                void bind();
                static void unbind();

                //  test
                void draw();

                Texture *tex;
                FBO *fbo;
                RBO *rbo;
                int width, height;
            
        };

        class RenderTarget
        {
            public:
                int w, h;
                GLuint *ID_FBO, *ID_TEX, *ID_RBO;

                RenderTarget(int w, int h);
                ~RenderTarget();
                void bind();
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
                void setBool(const std::string &name, bool value) const;
                void setInt(const std::string &name, int value) const;
                void setFloat(const std::string &name, float value) const;
                void setVec2(const std::string &name, const glm::vec2 &value) const;
                void setVec2(const std::string &name, float x, float y) const;
                void setVec3(const std::string &name, const glm::vec3 &value) const;
                void setVec3(const std::string &name, float x, float y, float z) const;
                void setVec4(const std::string &name, const glm::vec4 &value) const;
                void setVec4(const std::string &name, float x, float y, float z, float w) const;
                void setMat2(const std::string &name, const glm::mat2 &value) const;
                void setMat3(const std::string &name, const glm::mat3 &value) const;
                void setMat4(const std::string &name, const glm::mat4 &value) const;
        };
    }    
}





#endif