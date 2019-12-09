#include "engine.h"
#include "glm/gtx/string_cast.hpp"

namespace engine {

    int scrWidth;
    int scrHeight;
    gl::Shader *spriteShader;

    void SpriteSheet::load(const std::string &path, int numSprites) {

        vao = new gl::VAO();
        vbo = new gl::VBO();
        
        vao->init();
        vao->bind();

        vbo->init();
        vbo->bind();
        vbo->createVertexAttribPointer(4, GL_FLOAT, 4*sizeof(float), (void*)0);

        this->numSprites = numSprites;
        sprites = new Sprite[numSprites];
        tex = new gl::Texture();
        tex->init();
        tex->bind();
        tex->load(path);

        vao->unbind();

        verts = new std::vector<float>();
        indices = new std::vector<uint32_t>();
    }

    void SpriteSheet::setSprite(int num, int x, int y, int width, int height)
    {
        if(num < numSprites && num >= 0) {
            sprites[num].x = (1.0f / (float)tex->srcWidth) * (float)x;
            sprites[num].y = (1.0f / (float)tex->srcHeight) * (float)y;
            sprites[num].z = (1.0f / (float)tex->srcWidth) * (float)(x + width);
            sprites[num].w = (1.0f / (float)tex->srcWidth) * (float)(y + height);
            sprites[num].width = width;
            sprites[num].height = height;
        }
        printf("sprite #%d: %f %f %f %f\n", num, sprites[num].x, sprites[num].y, sprites[num].z, sprites[num].w);
    }

    void SpriteSheet::drawSprite(int num, float x, float y) {
        float v[] = {
            x, y + sprites[num].height, sprites[num].x, sprites[num].w,
            x + sprites[num].width, y, sprites[num].z, sprites[num].y,
            x, y, sprites[num].x, sprites[num].y,
            x + sprites[num].width, y + sprites[num].height, sprites[num].z, sprites[num].w
        };

        uint32_t size = verts->size();

        uint32_t ind[] = {
            size, size + 1, size + 2,
            size, size + 1, size + 3
        };

        for(int i = 0; i < 16; i++) {
            verts->push_back(v[i]);
        }

        for(int i = 0; i < 6; i++) {
            indices->push_back(ind[i]);
        }
    };

    void SpriteSheet::draw() {
        vao->bind();
        spriteShader->use();
        tex->bind();
        vbo->bind();
        // vbo->bufferVerts(sizeof(float) * data->size(), data->data());
        vbo->bufferVerts(sizeof(float) * verts->size(), verts->data(), sizeof(uint32_t) * indices->size(), indices->data());
        // glDrawArrays(GL_TRIANGLES, 0, data->size()/4);
        glDrawElements(GL_TRIANGLES, indices->size(), GL_UNSIGNED_INT, (void*)0);
        vao->unbind();
        verts->clear();
        indices->clear();
    }

    void SpriteSheet::unload() {
        tex->remove();
        vbo->remove();
        vao->remove();
        delete verts;
        delete indices;
        delete sprites;
    }



    void init(const char *title, int width, int height) {
        scrWidth = width;
        scrHeight = height;

        SDL_Init(SDL_INIT_EVERYTHING);
        SDL_GL_LoadLibrary(NULL);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);


        gl::window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, scrWidth, scrHeight, SDL_WINDOW_OPENGL);
        gl::maincontext = SDL_GL_CreateContext(gl::window);
        gladLoadGLLoader(SDL_GL_GetProcAddress);

        SDL_GL_SetSwapInterval(0);
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_BLEND);
        //glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //stbi_set_flip_vertically_on_load(true);
        glm::mat4 scrProjection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
        spriteShader = new gl::Shader();
        spriteShader->load("./shaders/sprite.vert", "./shaders/sprite.frag");
        spriteShader->use();
        spriteShader->setMat4("scrProjection", scrProjection);
        spriteShader->setInt("txUnit", 0);
    }

    void close() {
        SDL_GL_DeleteContext(gl::maincontext);
        SDL_DestroyWindow(gl::window);
        SDL_Quit();
    }

}