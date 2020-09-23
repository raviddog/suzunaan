#ifndef _ENGINE_H
#define _ENGINE_H

#include "engine_gl.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <vector>

namespace engine {

    //  set game controls here
    enum inputKeys {
        inputUp, inputDown, inputLeft, inputRight,
        inputFire, inputFocus, inputBomb, inputPause,
        inputQuit, inputRestart, inputSkip
    };

    enum Drawmode {
        DrawmodeSprite,
        DrawmodeUI,
        Drawmode3D
    };

    extern gl::Shader *shaderSpriteSheet, *shaderSpriteSheetInvert, *shaderUI, *pshader, *shader3d;

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

    // void InitialiseDrawmodes();
    void SetDrawmode(Drawmode dmode);



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

    //  need texture manip functions

    struct Sprite {
        float x;
        float y;
        float z;
        float w;
        float width;
        float height;
    };

    class RenderObject {
        public:
            virtual void bind() =0;
            // virtual void draw() =0;
            void unbind();


    };

    class ModelData {
        public:
            float *verts = nullptr;
            uint32_t *indices = nullptr;
            gl::Texture *tex = nullptr;

            size_t vsize, isize;
    };

    class SpriteData {
        public:
            gl::Texture *tex = nullptr;
            
    };

    class SpriteInstance : public RenderObject {
        //  single sprite/texture test
        public:
            SpriteData *data;

            gl::VAO *vao;
            gl::VBO *vbo;
            //  other offset/rendering things

            //  non resizeable for now
            SpriteInstance();
            ~SpriteInstance();

            void bufferVerts(size_t vertsize, float *verts);

            //  other parameters to set up bind function
            void bind();
            void draw(int triangles);
            static void unbind();
    };

    

    //  need a shader to draw to

    class ModelInstance : public RenderObject {
        public:
            ModelData *model;
            //  other offset/rendering things
            

            //  need other parameters to set up bind function
            void bind();
            void draw();
            static void unbind();
    };

    class ModelGroup : public RenderObject {
        public:
            ModelData *models;
            //  other offset/rendering things

            

            //  need other parameters to set up bind function
            void bind();
            void draw();
            static void unbind();
    };

    class Mesh {
        public:
            //  mesh data
            std::vector<gl::modelVertex> vertices;
            std::vector<uint32_t> indices;
            std::vector<gl::Texture*> textures;

            Mesh(std::vector<gl::modelVertex> vertices, std::vector<uint32_t> indices, std::vector<gl::Texture*> textures);
            void draw();    //  add shader
        private:
            //  render data
            gl::VAO *vao;
            gl::VBO *vbo;

            void setupMesh();
    };

    class Model {
        public:
            Model(char *path) {
                loadModel(path);
            }
            void draw();
        private:
            std::vector<Mesh*> meshes;
            std::string directory;
            std::vector<gl::Texture*> loadedTextures;

            void loadModel(std::string path);
            void processNode(aiNode *node, const aiScene *scene);
            Mesh* processMesh(aiMesh *mesh, const aiScene *scene);
            std::vector<gl::Texture*> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

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
            //  prepare stuff for drawing sprites
            // static void prepareDraw();
    };

    
}

#endif