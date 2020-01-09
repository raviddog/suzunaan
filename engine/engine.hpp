#ifndef _ENGINE_H
#define _ENGINE_H

#include "engine_gl.hpp"
#include <vector>

namespace engine {
    void init(const char*, int, int, int);
    void init(const char*, int, int, int, int, int);
    void inputs();
    void close();

    // class Sprite {
    //     private:
    //         std::vector<float> *verts;
    //         std::vector<uint32_t> *indices;
    //         uint32_t indices_stored_size;
    //     public:
    //         gl::VAO *vao;
    //         gl::VBO *vbo;
    //         gl::Texture *tex;
    //         int width, height;

    //         void load(const std::string&);
    //         void unload();
    //         void drawSprite(float, float);
    //         void buffer();
    //         void draw();

    // };
    struct Sprite {
        float x;
        float y;
        float z;
        float w;
        float width;
        float height;
    };

    class SpriteSheet {
        private:
            std::vector<float> *verts;
            std::vector<uint32_t> *indices;
            size_t indices_stored_size;
            Sprite *sprites;
            gl::VAO *vao;
            gl::VBO *vbo;
            gl::Texture *tex;
            bool realloc;

            void load(const std::string&, int);
            void load(const std::string&, int, size_t);
        public:
            int numSprites;

            SpriteSheet(const std::string&, int);
            SpriteSheet(const std::string&, int, int);
            ~SpriteSheet();

            void setSprite(int, int, int, int, int);

            // temp
            void drawSprite(int, float, float);
            void drawSprite(int, float, float, float);
            void buffer();
            void draw();

            //  more temp or game specific
            static void useShaderInvert();
            static void useShaderNormal();
    };
}

#endif