#ifndef _ENGINE_H
#define _ENGINE_H

#include "engine_gl.hpp"

#include <vector>

namespace engine {

    //  set game controls here
    enum inputKeys {
        inputUp, inputDown, inputLeft, inputRight,
        inputFire, inputFocus, inputBomb, inputPause,
        inputQuit, inputRestart, inputSkip
    };
    bool checkKey(int key);
    bool checkKeyPressed(int key);

    bool init(const char *title, const char *settingsPath);
    void init(const char *title, int screenMode, bool vsync, int width, int height);
    void init(const char *title, int screenMode, bool vsync, int width_win, int height_win, int width_draw, int height_draw);
    void inputs();
    void flip();
    void close();

    void setViewport();
    void setViewport(int x, int y, int w, int h);


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
    //         void drawSpriteCentered(float, float);
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

            void load(const std::string &path, int nummSprites);
            void load(const std::string &path, int numSprites, size_t maxDraws);
        public:
            int numSprites;

            SpriteSheet(const std::string &path, int numSprites);
            SpriteSheet(const std::string &path, int numSprites, size_t maxDraws);
            ~SpriteSheet();

            void setSprite(int num, int x, int y, int width, int height);

            //  temp
            //  maybe not so temp after all
            void drawSprite(int num, float x, float y);
            void drawSprite(int num, float x, float y, float angle);
            void drawSprite(int num, float x, float y, float angle, float width, float height);
            void drawSpriteCentered(int num, float x, float y);
            void drawSpriteCentered(int num, float x, float y, float angle);
            void drawSpriteCentered(int num, float x, float y, float angle, float width, float height);
            void buffer();
            void draw();

            //  more temp or game specific
            static void useShaderInvert();
            static void useShaderNormal();
    };
}

#endif