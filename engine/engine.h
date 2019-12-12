#ifndef _ENGINE_H
#define _ENGINE_H

#include "engine_gl.h"
#include <vector>
#include "math.h"

namespace engine {
    void init(const char*, bool, int, int);
    void init(const char*, bool, int, int, int, int);
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
            uint32_t indices_stored_size;
            Sprite *sprites;
            gl::VAO *vao;
            gl::VBO *vbo;
            gl::Texture *tex;
        public:
            int numSprites;

            void load(const std::string&, int);
            void setSprite(int, int, int, int, int);
            void unload();

            // temp
            void drawSprite(int, float, float);
            void drawSprite(int, float, float, float);
            void buffer();
            void draw();
    };

}


#endif