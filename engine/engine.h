#ifndef _ENGINE_H
#define _ENGINE_H

#include "engine_gl.h"
#include <vector>

namespace engine {

    void init(const char*, int, int);
    void inputs();
    void close();

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
        public:
            glm::mat4 texProjection;
            int numSprites;
            Sprite *sprites;
            gl::VAO *vao;
            gl::VBO *vbo;
            gl::Texture *tex;



            void load(const std::string&, int);
            void setSprite(int, int, int, int, int);
            void unload();

            // temp
            void drawSprite(int, float, float);
            void draw();
    };

}


#endif