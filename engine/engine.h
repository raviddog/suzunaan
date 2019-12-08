#ifndef _ENGINE_H
#define _ENGINE_H

#include "engine_gl.h"

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
        public:
            glm::mat4 texProjection;
            int numSprites;
            Sprite *sprites;
            gl::VAO *vao;
            gl::VBO *vbo;
            gl::Texture *tex;



            void load(const std::string&, int);
            void setSprite(int, int, int, int, int);

            // temp
            void drawSprite(int, int, int);
    };

}


#endif