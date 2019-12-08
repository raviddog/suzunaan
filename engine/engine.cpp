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

        float verts[] = {
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
        };

        uint32_t inds[] = {
            0, 1, 2,
            0, 1, 3
        };

        vbo->bufferVerts(sizeof(verts), verts, sizeof(inds), inds);
        vbo->createVertexAttribPointer(4, GL_FLOAT, 4*sizeof(float), (void*)0);


        this->numSprites = numSprites;
        sprites = new Sprite[numSprites];
        //glActiveTexture(GL_TEXTURE0);
        tex = new gl::Texture();
        tex->init();
        tex->bind();
        tex->load(path);


        vao->unbind();
    }

    void SpriteSheet::setSprite(int num, int x, int y, int width, int height)
    {
        if(num < numSprites && num >= 0) {
            sprites[num].x = x;
            sprites[num].y = y;
            sprites[num].width = width;
            sprites[num].height = height;
        }
    }

    void SpriteSheet::drawSprite(int num, int x, int y) {
        vao->bind();
        spriteShader->use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3((float)x, (float)y, 0.0f));
        spriteShader->setMat4("scrModel", model);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3((float)tex->srcWidth, (float)tex->srcHeight, 1.0f));
        spriteShader->setMat4("scrScale", scale);
        // std::cout<<glm::to_string(model)<<std::endl;
        tex->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        vao->unbind();


    };



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
        glViewport(0, 0, 640, 480);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_BLEND);
        //glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        stbi_set_flip_vertically_on_load(true);
        glm::mat4 scrProjection = glm::mat4(1.0f);
        scrProjection = glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, -1.0f, 1.0f);
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