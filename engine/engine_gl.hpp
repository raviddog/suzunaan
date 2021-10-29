#ifndef _ENGINE_GL_H
#define _ENGINE_GL_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"

// #include "SDL2/SDL.h"
// #include "SDL2/SDL_opengl.h"
#include "libs/stb_image.h"
#include "libs/assetsys.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

namespace engine {
    namespace kb {
        //  https://www.glfw.org/docs/3.3/group__keys.html
        enum keyCodes{
            Unknown = -1,
            Space = 32,
            Apostrophe = 39,
            Comma = 44, Minus, Period, Slash,
            k0, k1, k2, k3, k4, k5, k6, k7, k8, k9,
            Semicolon = 59,
            Equals = 61,
            A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
            Leftbracket, Backslash, Rightbracket,
            Grave_Accent = 96,
            Escape = 256, Enter, Tab, Backspace, Insert, Delete, Right, Left, Down, Up,
            Pageup, Pagedown, Home, End,
            Capslock = 280, Scrolllock, Numlock, Printscreen, Pause,
            F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
            F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25,
            n0, n1, n2, n3, n4, n5, n6, n7, n8, n9,
            nPeriod, nDivide, nMultiply, nMinus, nPlus, nEnter, nEqual,
            LShift = 340, LControl, LAlt, LSuper,
            RShift, RControl, RAlt, RSuper, Menu,
            
            KeycodesLength = keyCodes::Menu
        };
    }

    extern double mouseX, mouseY, mouseMoveX, mouseMoveY;
    extern bool quit, loadFromZip;
    extern assetsys_t *assets;
    extern int keyState[kb::KeycodesLength];
    extern bool keyPressed[kb::KeycodesLength];
    extern void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void inputs();

    namespace gl {
        extern GLFWwindow *window;
        // extern SDL_GLContext maincontext;

        struct modelVertex {
            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec2 TexCoords;
        };

        class VAO {
            public:
                GLuint *ID;
                VAO();
                ~VAO();
                void bind();
                static void unbind();
        };

        class VBO {
            public:
                GLuint *ID_VBO, *ID_EBO;
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
                GLuint *ID;

                FBO();
                ~FBO();
                void bind();
                static void unbind();
        };

        class RBO
        {
            public:
                GLuint *ID;

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

                GLuint *ID;
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