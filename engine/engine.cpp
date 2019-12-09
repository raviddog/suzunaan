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
        
        texProjection = glm::ortho(0.0f, (float)tex->srcWidth, 0.0f, (float)tex->srcHeight);
        spriteShader->setMat4("txProjection", texProjection);

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3((float)tex->srcWidth, (float)tex->srcHeight, 1.0f));
        spriteShader->setMat4("scrScale", scale);

        vao->unbind();

        data = new std::vector<float>();
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

    void SpriteSheet::drawSprite(int num, int x, int y) {

        float verts[] = {
            (float)x, (float)y + sprites[num].height, sprites[num].x, sprites[num].w,
            (float)x + sprites[num].width, (float)y, sprites[num].z, sprites[num].y,
            (float)x, (float)y, sprites[num].x, sprites[num].y,
            (float)x, (float)y + sprites[num].height, sprites[num].x, sprites[num].w,
            (float)x + sprites[num].width, (float)y, sprites[num].z, sprites[num].y,
            (float)x + sprites[num].width, (float)y + sprites[num].height, sprites[num].z, sprites[num].w
        };

        for(int i = 0; i < 24; i++) {
            data->push_back(verts[i]);
        }
    };

    void SpriteSheet::draw() {
        vao->bind();
        spriteShader->use();
        tex->bind();
        vbo->bufferVerts(sizeof(float) * data->size(), data->data());
        glDrawArrays(GL_TRIANGLES, 0, data->size()/4);
        vao->unbind();
        data->clear();
    }

    void SpriteSheet::unload() {
        tex->remove();
        vbo->remove();
        vao->remove();
        delete data;
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

        SDL_GL_SetSwapInterval(1);
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