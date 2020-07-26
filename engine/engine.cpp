#include "engine.hpp"

#include "debug.hpp"

#include <chrono>
#include <thread>
#include <string>
#include <fstream>

#ifdef _MSC_VER
#include "windows.h"
#include "timeapi.h"
#endif

#ifdef __GNUG__
#include "pthread.h"
#endif

namespace engine {

    int scrWidth, scrHeight, drawWidth, drawHeight;
    int viewport[4], scrMode;

    //  framerate stuff
    static bool _vsync;
    uint32_t ticks, fps, frameTimeTicks, slept;
    std::chrono::high_resolution_clock::time_point cur_time, next_time;
    #define _ENGINE_NOVSYNC_DELAY_MICROSECONDS 16666

    static gl::Shader *shaderSpriteSheet, *shaderSpriteSheetInvert;

    SpriteSheet::SpriteSheet(const std::string &path, int numSprites) {
        load(path, numSprites);
    }

    SpriteSheet::SpriteSheet(const std::string &path, int numSprites, size_t maxDraws) {
        load(path, numSprites, maxDraws);
    }

    SpriteSheet::~SpriteSheet() {
        delete tex;
        delete vbo;
        delete vao;
        delete verts;
        delete indices;
        delete sprites;
    }

    void SpriteSheet::load(const std::string &path, int numSprites) {
        realloc = true;

        vao = new gl::VAO();
        vbo = new gl::VBO();
        
        vao->bind();
        vbo->bind();
        vbo->createVertexAttribPointer(2, GL_FLOAT, 7*sizeof(float), (void*)0);
        vbo->createVertexAttribPointer(2, GL_FLOAT, 7*sizeof(float), (void*)(2*sizeof(float)));
        vbo->createVertexAttribPointer(2, GL_FLOAT, 7*sizeof(float), (void*)(4*sizeof(float)));
        vbo->createVertexAttribPointer(1, GL_FLOAT, 7*sizeof(float), (void*)(6*sizeof(float)));
        vbo->unbind();

        this->numSprites = numSprites;
        sprites = new Sprite[numSprites];
        tex = new gl::Texture();
        tex->bind();
        tex->load(path);
        tex->unbind();

        vao->unbind();

        verts = new std::vector<float>();
        indices = new std::vector<uint32_t>();

        indices_stored_size = 0;
    }

    void SpriteSheet::load(const std::string &path, int numSprites, size_t maxDraws) {
        load(path, numSprites);
        vbo->bind();
        vbo->createBuffer(sizeof(float) * 28 * maxDraws, sizeof(uint32_t) * 6 * maxDraws);
        vbo->unbind();
        realloc = false;
        verts->reserve(maxDraws * 28);
        indices->reserve(maxDraws * 6);
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
        #ifdef _DEBUG_MSG_ENABLED_SPRITE
        log_debug("sprite #%d: %f %f %f %f\n", num, sprites[num].x, sprites[num].y, sprites[num].z, sprites[num].w);
        #endif
    }

    void SpriteSheet::drawSprite(int num, float x, float y) {
        drawSprite(num, x, y, 0.0f);
    }

    void SpriteSheet::drawSprite(int num, float x, float y, float angle) {
        if(num > -1 && num < numSprites) {
            float v[] {
                x + sprites[num].width / 2.f, y + sprites[num].height / 2.f, sprites[num].x, sprites[num].w, -sprites[num].width, -sprites[num].height, angle,
                x + sprites[num].width / 2.f, y + sprites[num].height / 2.f, sprites[num].x, sprites[num].y, -sprites[num].width, sprites[num].height, angle,
                x + sprites[num].width / 2.f, y + sprites[num].height / 2.f, sprites[num].z, sprites[num].y, sprites[num].width, sprites[num].height, angle,
                x + sprites[num].width / 2.f, y + sprites[num].height / 2.f, sprites[num].z, sprites[num].w, sprites[num].width, -sprites[num].height, angle

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
    }

    void SpriteSheet::drawSprite(int num, float x, float y, float angle, float width, float height) {
        if(num > -1 && num < numSprites) {
            float v[] {
                x + width / 2.f, y + height / 2.f, sprites[num].x, sprites[num].w, -width, -height, angle,
                x + width / 2.f, y + height / 2.f, sprites[num].x, sprites[num].y, -width, height, angle,
                x + width / 2.f, y + height / 2.f, sprites[num].z, sprites[num].y, width, height, angle,
                x + width / 2.f, y + height / 2.f, sprites[num].z, sprites[num].w, width, -height, angle

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
    }

    void SpriteSheet::drawSpriteCentered(int num, float x, float y) {
        drawSpriteCentered(num, x, y, 0.0f);
    }

    void SpriteSheet::drawSpriteCentered(int num, float x, float y, float angle) {
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
    }

    void SpriteSheet::drawSpriteCentered(int num, float x, float y, float angle, float width, float height) {
        if(num > -1 && num < numSprites) {
            float v[] {
                x, y, sprites[num].x, sprites[num].w, -width, -height, angle,
                x, y, sprites[num].x, sprites[num].y, -width, height, angle,
                x, y, sprites[num].z, sprites[num].y, width, height, angle,
                x, y, sprites[num].z, sprites[num].w, width, -height, angle

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
    }

    void SpriteSheet::buffer() {
        vao->bind();
        vbo->bind();
        if(realloc) {
            vbo->bufferVerts(sizeof(float) * verts->size(), verts->data(), sizeof(uint32_t) * indices->size(), indices->data());
        } else {
            vbo->bufferSubVerts(sizeof(float) * verts->size(), verts->data(), sizeof(uint32_t) * indices->size(), indices->data());
        }
        indices_stored_size = indices->size();
        vao->unbind();
        verts->clear();
        indices->clear();
    }

    void SpriteSheet::draw() {
        vao->bind();
        tex->bind();
        glDrawElements(GL_TRIANGLES, indices_stored_size, GL_UNSIGNED_INT, (void*)0);
        vao->unbind();
    }

    void SpriteSheet::useShaderInvert() {
        shaderSpriteSheetInvert->use();
    }

    void SpriteSheet::useShaderNormal() {
        shaderSpriteSheet->use();
    }

    //  load settings from file
    bool init(const char *title, const char *settingsPath) {
        debug_init();
        std::ifstream settings(settingsPath, std::ifstream::in);
        if(settings.good()) {
            
            //  settings
            int screenMode = 0;
            bool vsync = true;
            int width_win = 640, height_win = 480;
            const int width_draw = 640, height_draw = 480;

            while(settings.peek() != EOF) {
                if(settings.peek() == '[') {
                    settings.ignore(512, '\n');
                } else {
                    std::string id;
                    std::getline(settings, id, '=');
                    if(id == "Width") {
                        std::getline(settings, id);
                        sscanf(id.c_str(), "%d", &width_win);
                    } else if(id == "Height") {
                        std::getline(settings, id);
                        sscanf(id.c_str(), "%d", &height_win);
                    } else if(id == "Vsync") {
                        std::getline(settings, id);
                        id == "true" ? vsync = true : vsync = false;
                    } else if(id == "ScreenMode") {
                        std::getline(settings, id);
                        if(id == "windowed") screenMode = 0;
                        if(id == "borderless") screenMode = 1;
                        if(id == "fullscreen") screenMode = 2;
                    }
                }
            }
            init(title, screenMode, vsync, width_win, height_win, width_draw, height_draw);
            return true;
        } else return false;
    }

    void init(const char *title, int screenMode, bool vsync, int width, int height) {
        init(title, screenMode, vsync, width, height, width, height);
    }

    void init(const char *title, int screenMode, bool vsync, int width_win, int height_win, int width_draw, int height_draw) {
        drawWidth = width_draw;
        drawHeight = height_draw;
        scrMode = screenMode;
        ticks = SDL_GetTicks();
        frameTimeTicks = ticks;
        fps = 0u;
        slept = 0u;

        SDL_Init(SDL_INIT_EVERYTHING);
        SDL_GL_LoadLibrary(NULL);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        //  for borderless fullscreen calculation
        SDL_DisplayMode *dmode = new SDL_DisplayMode();
        SDL_GetCurrentDisplayMode(0, dmode);
        switch(screenMode) {
            case 1:
                //  borderless fullscreen
                gl::window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dmode->w, dmode->h, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED);
                scrWidth = dmode->w;
                scrHeight = dmode->h;
                break;
            case 2:
                //  fullscreen
                gl::window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width_draw, height_draw, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
                scrWidth = width_draw;
                scrHeight = height_draw;
                break;
            case 3:
                //  test, fullscreen but draw canvas mapped to screen res
                gl::window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dmode->w, dmode->h, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
                scrWidth = dmode->w;
                scrHeight = dmode->h;
                break;
            case 0:
            default:
                //  normal windowed
                gl::window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width_win, height_win, SDL_WINDOW_OPENGL);
                scrWidth = width_win;
                scrHeight = height_win;
                break;
        }
        
        gl::maincontext = SDL_GL_CreateContext(gl::window);
        gladLoadGLLoader(SDL_GL_GetProcAddress);

        switch(screenMode) {
            case 3:
            case 1:
            {
                float draw_ratio = (float)width_draw / (float)height_draw;
                float screen_ratio = (float)dmode->w / (float)dmode->h;
                if(draw_ratio > screen_ratio) {
                    //  draw area is wider than screen
                    float y_scale = (float)dmode->w / (float)width_draw;
                    float height = (float)height_draw * y_scale;
                    int offset = (dmode->h - (int)height) / 2;
                    glViewport(0, offset, dmode->w, (int)height);
                    viewport[0] = 0;
                    viewport[1] = offset;
                    viewport[2] = dmode->w;
                    viewport[3] = (int)height;
                    break;
                } else if(draw_ratio < screen_ratio) {
                    //  draw area is narrower than screen
                    float x_scale = (float)dmode->h / (float)height_draw;
                    float width = (float)width_draw * x_scale;
                    int offset = (dmode->w - (int)width) / 2;
                    glViewport(offset, 0, (int)width, dmode->h);
                    viewport[0] = offset;
                    viewport[1] = 0;
                    viewport[2] = (int)width;
                    viewport[3] = dmode->h;
                    break;
                } else {
                    //  matches aspect ratio, although i probably need a way better way to check this
                    //  handy dandy c++17 feature to make the fallthrough warning go away
                    [[fallthrough]];
                }
            }
            case 2:
                glViewport(0, 0, width_draw, height_draw);
                viewport[0] = 0;
                viewport[1] = 0;
                viewport[2] = width_draw;
                viewport[3] = height_draw;
                break;
            case 0:
            default:
                glViewport(0, 0, width_win, height_win);
                viewport[0] = 0;
                viewport[1] = 0;
                viewport[2] = width_win;
                viewport[3] = height_win;
                break;
        }

        delete dmode;


        if(vsync) {
            _vsync = true;
            SDL_GL_SetSwapInterval(1);
        } else {
            _vsync = false;
            SDL_GL_SetSwapInterval(0);

            // cur_time = std::chrono::steady_clock::now();
            // next_time = cur_time + std::chrono::milliseconds(_ENGINE_NOVSYNC_DELAY_MICROSECONDS);
            next_time = std::chrono::high_resolution_clock::now();
            #ifdef _MSC_VER
            timeBeginPeriod(1);
            #endif
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_BLEND);
        //glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // stbi_set_flip_vertically_on_load(true);  // don't need this because the shader i wrote accounts for it
        glm::vec2 scrRes = glm::vec2((float)width_draw, (float)height_draw);
        
        shaderSpriteSheetInvert = new gl::Shader();
        shaderSpriteSheetInvert->load("./shaders/spritesheet.vert", "./shaders/spritesheet_invert.frag");
        shaderSpriteSheetInvert->use();
        shaderSpriteSheetInvert->setInt("txUnit", 0);
        shaderSpriteSheetInvert->setVec2("res", scrRes);

        shaderSpriteSheet = new gl::Shader();
        shaderSpriteSheet->load("./shaders/spritesheet.vert", "./shaders/spritesheet.frag");
        shaderSpriteSheet->use();
        shaderSpriteSheet->setInt("txUnit", 0);
        shaderSpriteSheet->setVec2("res", scrRes);
    }

    void flip() {
        //  flip buffers
        SDL_GL_SwapWindow(gl::window);

        //  clear new buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //  if vsync disabled, cap fps
        int temp = 0;
        #ifdef _DEBUG_MSG_ENABLED_FPS
        uint32_t start, wake;
        #endif
        if(!_vsync) {
            //  wait
            // printf("before sleep %u ", SDL_GetTicks());
            // cur_time = std::chrono::steady_clock::now();
            #ifdef _DEBUG_MSG_ENABLED_FPS
            start = SDL_GetTicks();
            #endif
            // while(std::chrono::steady_clock::now() < next_time + std::chrono::microseconds(9000)) {
            //     slept++;
            //     std::this_thread::sleep_until(next_time + std::chrono::microseconds(10000));
            // }
            #ifdef __GNUG__
            timespec delayt, delayr;
            std::chrono::nanoseconds delaym = std::chrono::duration_cast<std::chrono::nanoseconds>(next_time - std::chrono::high_resolution_clock::now());
            delayt.tv_sec = 0;
            delayt.tv_nsec = delaym.count();
            delayr.tv_nsec = 0;
            do {
                nanosleep(&delayt, &delayr);
            } while (delayr.tv_nsec > 0);
            #else
            std::this_thread::sleep_until(next_time);
            #endif
            // auto wake_time = std::chrono::steady_clock::now();
            #ifdef _DEBUG_MSG_ENABLED_FPS
            wake = SDL_GetTicks();
            #endif
            while(std::chrono::high_resolution_clock::now() < next_time) {
                //  spin
                temp++;
            }
            if(temp == 0) {
                // log_debug("spun 0 times\n");
            } 
            // next_time = std::chrono::steady_clock::now();
            next_time += std::chrono::microseconds(_ENGINE_NOVSYNC_DELAY_MICROSECONDS);
            
            
            //  spin
            // next_time = cur_time + std::chrono::microseconds(_ENGINE_NOVSYNC_DELAY_MICROSECONDS);
            
            //  wait and spin
            // std::this_thread::sleep_until(cur_time + std::chrono::microseconds(8000));
            
            // do {
            //     cur_time = std::chrono::steady_clock::now();
            //     // std::this_thread::yield();
            // } while(cur_time < next_time);
            // next_time += std::chrono::microseconds(_ENGINE_NOVSYNC_DELAY_MICROSECONDS);
            
        }

        #ifdef _DEBUG_MSG_ENABLED_FPS
        //  print debug fps data
        uint32_t temp_ticks = SDL_GetTicks();
        if(temp_ticks > ticks + 1000) {
            log_debug("slept for %u ms ", wake - start);
            log_debug("slept %u times ", slept);
            log_debug("spun %d times ", temp);
            log_debug("frame time: %ums, ", temp_ticks - frameTimeTicks);
            log_debug("fps: %u\n", fps);
            fps = 0u;
            ticks = temp_ticks;
            slept = 0u;
        }
        frameTimeTicks = temp_ticks;
        #endif
        ++fps;
    }

    void close() {
        #ifdef _MSC_VER
        if(!_vsync) timeEndPeriod(1);
        #endif
        SDL_GL_DeleteContext(gl::maincontext);
        SDL_DestroyWindow(gl::window);
        SDL_Quit();
    }

    void setViewport() {
        //  no arguments resets the viewport to original
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        glm::vec2 scrRes = glm::vec2((float)drawWidth, (float)drawHeight);
        shaderSpriteSheet->setVec2("res", scrRes);
    }

    void setViewport(int x, int y, int w, int h)
    {
        //  set viewport to specified rectangle (inside draw area)
        //  need to calculate x and y based off of the existing draw area
        float scalex, scaley;
        scalex = (float)scrWidth / (float)drawWidth;
        scaley = (float)scrHeight / (float)drawHeight;

        if(scrMode == 1 || scrMode == 3) {
            float draw_ratio = (float)drawWidth / (float)drawHeight;
            float screen_ratio = (float)scrWidth / (float)scrHeight;
            if(draw_ratio > screen_ratio) {
                //  draw area is wider than screen
                scaley = scalex;
            } else if(draw_ratio < screen_ratio) {
                //  draw area is narrower than screen
                scalex = scaley;
            }
        }

        glViewport( viewport[0] + (int)(scalex * (float)x),
                    viewport[1] + (int)(scaley * (float)y),
                    (int)(scalex * (float)w),
                    (int)(scaley * (float)h));

        
        
        glm::vec2 scrRes = glm::vec2((float)w, (float)h);
        shaderSpriteSheet->setVec2("res", scrRes);
    }

}