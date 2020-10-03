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
#include <vector>

namespace engine {
    namespace kb {
        enum keyCodes{
            Unknown = 0,
            A = 4, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
            k1, k2, k3, k4, k5, k6, k7, k8, k9, k0,
            Return, Escape, Backspace, Tab, Space, Minus, Equals, Leftbracket, Rightbracket, Backslash,
            NonUSHash, Semicolon, Apostrophe, Grave, Comma, Period, Slash, Capslock,
            F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
            Printscreen, Scrolllock, Pause, Insert, Home, Pageup, Delete, End, Pagedown,
            Right, Left, Down, Up, Numlockclear,
            nDivide, nMultiplly, nMinus, nPlus, nEnter,
            n1, n2, n3, n4, n5, n6, n7, n8, n9, n0,
            nPeriod, NonUSBackslash, Application, Power, nEquals,
            F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,
            Execute, Help, Menu, Select, Stop, Again, Undo, Cut, Copy, Paste, Find, Mute,
            VolumeUp, VolumeDown,
            nComma = 133, nEqualsAs400,
            LCtrl = 224, LShift, LAlt, LGUI, RCtrl, RShift, RAlt, RGUI,

            KeycodesLength = 284
        };
    }

    extern int mouseX, mouseY, mouseMoveX, mouseMoveY;
    extern bool quit;
    extern bool keyState[kb::KeycodesLength];
    extern bool keyPressed[kb::KeycodesLength];
    void inputs();

    namespace gl {
        extern SDL_Window *window;
        extern SDL_GLContext maincontext;

        struct modelVertex {
            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec2 TexCoords;
        };

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
                void bufferVerts(size_t vertsize, modelVertex *verts);
                void bufferVerts(size_t vertsize, modelVertex *verts, size_t indsize, uint32_t *indices);
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
            private:
                // static int curTxUnit;
            public:
                Texture();
                ~Texture();
                Texture(Texture&& t);
                Texture& operator=(Texture&& t);

                GLuint ID;
                int srcWidth, srcHeight, srcChannels;
                std::string type;
                std::string path;
                
                void bind();
                void bind(int txUnit);
                void load(const std::string &path);

                static void activateUnit(int txUnit);
                // static int getActiveUnit();
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