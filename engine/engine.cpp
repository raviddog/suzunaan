#include "engine.h"
#include "glm/gtx/string_cast.hpp"

namespace engine {

    int scrWidth;
    int scrHeight;

    static gl::Shader *shaderSpriteSheet;
    // static gl::Shader *shaderSprite;

    //  test function
    void setspritesheetfloat(float val) {
        shaderSpriteSheet->setFloat("distortion_amount", val);
    }

    void SpriteSheet::load(const std::string &path, int numSprites) {

        vao = new gl::VAO();
        vbo = new gl::VBO();
        
        vao->init();
        vao->bind();

        vbo->init();
        vbo->bind();
        vbo->createVertexAttribPointer(2, GL_FLOAT, 7*sizeof(float), (void*)0);
        vbo->createVertexAttribPointer(2, GL_FLOAT, 7*sizeof(float), (void*)(2*sizeof(float)));
        vbo->createVertexAttribPointer(2, GL_FLOAT, 7*sizeof(float), (void*)(4*sizeof(float)));
        vbo->createVertexAttribPointer(1, GL_FLOAT, 7*sizeof(float), (void*)(6*sizeof(float)));
        vbo->unbind();

        this->numSprites = numSprites;
        sprites = new Sprite[numSprites];
        tex = new gl::Texture();
        tex->init();
        tex->bind();
        tex->load(path);
        tex->unbind();

        vao->unbind();

        verts = new std::vector<float>();
        indices = new std::vector<uint32_t>();
    }

    void SpriteSheet::setSprite(int num, int x, int y, int width, int height)
    {
        if(num < numSprites && num >= 0) {
            //  normalised texture coordinates
            sprites[num].x = (1.0f / (float)tex->srcWidth) * (float)x;
            sprites[num].y = (1.0f / (float)tex->srcHeight) * (float)y;
            sprites[num].z = (1.0f / (float)tex->srcWidth) * (float)(x + width);
            sprites[num].w = (1.0f / (float)tex->srcHeight) * (float)(y + height);
            //  size in pixels, is normalized to screen size in shader
            sprites[num].width = (float)width;
            sprites[num].height = (float)height;
        }
        printf("sprite #%d: %f %f %f %f\n", num, sprites[num].x, sprites[num].y, sprites[num].z, sprites[num].w);
    }

    void SpriteSheet::drawSprite(int num, float x, float y) {
        drawSprite(num, x, y, 0.0f);
    };

    void SpriteSheet::drawSprite(int num, float x, float y, float angle) {
        if(num > -1 && num < numSprites) {
            float v[] {
                x, y, sprites[num].x, sprites[num].w, -sprites[num].width, -sprites[num].height, angle,
                x, y, sprites[num].x, sprites[num].y, -sprites[num].width, sprites[num].height, angle,
                x, y, sprites[num].z, sprites[num].y, sprites[num].width, sprites[num].height, angle,
                x, y, sprites[num].z, sprites[num].w, sprites[num].width, -sprites[num].height, angle

            };

            uint32_t size = verts->size() / 7;

            uint32_t ind[] = {
                size, size + 1, size + 2,
                size, size + 3, size + 2
            };

            for(int i = 0; i < 28; i++) {
                verts->push_back(v[i]);
            }

            for(int i = 0; i < 6; i++) {
                indices->push_back(ind[i]);
            }
        }
    };

    void SpriteSheet::buffer() {
        vao->bind();
        vbo->bind();
        vbo->bufferVerts(sizeof(float) * verts->size(), verts->data(), sizeof(uint32_t) * indices->size(), indices->data());
        indices_stored_size = indices->size();
        vao->unbind();
        verts->clear();
        indices->clear();
    }

    void SpriteSheet::draw() {
        vao->bind();
        shaderSpriteSheet->use();
        tex->bind();
        glDrawElements(GL_TRIANGLES, indices_stored_size, GL_UNSIGNED_INT, (void*)0);
        vao->unbind();
    }

    void SpriteSheet::unload() {
        tex->remove();
        vbo->remove();
        vao->remove();
        delete tex;
        delete vbo;
        delete vao;
        delete verts;
        delete indices;
        delete sprites;
    }

    void init(const char *title, bool fullscreen, int width, int height) {
        init(title, fullscreen, width, height, width, height);
    }

    void init(const char *title, bool fullscreen, int width_win, int height_win, int width_draw, int height_draw) {
        scrWidth = width_win;
        scrHeight = height_win;

        SDL_Init(SDL_INIT_EVERYTHING);
        SDL_GL_LoadLibrary(NULL);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        if(fullscreen) {
            gl::window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, scrWidth, scrHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
        } else {
            gl::window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, scrWidth, scrHeight, SDL_WINDOW_OPENGL);
        }
        gl::maincontext = SDL_GL_CreateContext(gl::window);
        gladLoadGLLoader(SDL_GL_GetProcAddress);

        SDL_GL_SetSwapInterval(1);
        glViewport(0, 0, width_win, height_win);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_BLEND);
        //glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // stbi_set_flip_vertically_on_load(true);
        // glm::mat4 scrProjection = glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);
        shaderSpriteSheet = new gl::Shader();
        shaderSpriteSheet->load("./shaders/spritesheet.vert", "./shaders/spritesheet.frag");
        shaderSpriteSheet->use();
        shaderSpriteSheet->setInt("txUnit", 0);
        // shaderSpriteSheet->setMat4("scrProjection", scrProjection);


        // glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(60.0f), glm::vec3(0.0, 0.0, 1.0));
        // shaderSpriteSheet->setMat4("r", rotate);
        // std::cout<<glm::to_string(rotate)<<std::endl;

        glm::vec2 scrRes = glm::vec2((float)width_draw, (float)height_draw);
        shaderSpriteSheet->setVec2("res", scrRes);

        // shaderSprite = new gl::Shader();
        // shaderSprite->load("./shaders/sprite.vert", "./shaders/sprite.frag");
        // shaderSprite->use();
        // shaderSprite->setInt("txUnit", 0);
    }

    void close() {
        SDL_GL_DeleteContext(gl::maincontext);
        SDL_DestroyWindow(gl::window);
        SDL_Quit();
    }

}