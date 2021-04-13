#ifndef _ENGINE_H
#define _ENGINE_H

#include "engine_gl.hpp"
#include "engine_helper.hpp"

#include <vector>
#include <memory>
#include <unordered_map>

namespace engine {

    //  set game controls here
    enum inputKeys {
        inputUp, inputDown, inputLeft, inputRight,
        inputFire, inputFocus, inputBomb, inputPause,
        inputQuit, inputRestart, inputSkip
    };

    enum Drawmode {
        DrawmodeSprite,
        DrawmodeSpriteInvert,
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

    void mouseCapture();
    void mouseRelease();

    // void InitialiseDrawmodes();
    void SetDrawmode(Drawmode dmode);

    //  need texture manip functions

    struct Sprite {
        float x;
        float y;
        float z;
        float w;
        float width;
        float height;
    };

    class SpriteData {
        public:
            gl::Texture *tex = nullptr;
            
    };

    class SpriteInstance {
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

    class Model {
        public:
            struct Material_t {
                std::string name;
                glm::vec3 colourAmbient;    //  ambient colour
                glm::vec3 colourDiffuse;    //  diffuse colour
                glm::vec3 colourSpecular;   //  specular colour
                
                float specularExponent;
                float opticalDensity;
                float dissolve;
                int illumination;
                
                std::string texname_ambientMap;
                std::string texname_diffuseMap;
                std::string texname_specularMap;
                std::string texname_alpha;
                std::string texname_bump;
            };

            struct Mesh_t {
                std::string name;
                std::shared_ptr<gl::VAO> vao;
                std::shared_ptr<gl::VBO> vbo;
                size_t indices;
                Material_t *material;
            };

            std::vector<Material_t> *materials;
            std::vector<Mesh_t> *meshes;
            std::unordered_map<std::string, gl::Texture*> *textures;
            
            std::string path;

            Model(const char *rawpath);
            ~Model();
            void draw();
    };

    class ModelInstance {
        public:
            glm::mat4 modelmat;
            Model* model;

            ModelInstance();
            void bind();
            void translate(float x, float y, float z);
            void scale(float x, float y, float z);
            void rotate(float degrees, float axis_x, float axis_y, float axis_z);
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
            //  static void prepareDraw();
    };

    class Camera3D {
        private:
            static Camera3D *bound;
            glm::vec3 dir_x, dir_y, dir_z;
            glm::vec3 eye, direction;
            glm::mat4 projection, view;
        public:
            //  need to store a look direction and a world space direction
            float angle_h, angle_v;
            float mov_fw, mov_up, mov_lf;
            float mov_dir_fw, mov_dir_lf;

            //  hyper TEMP TEMP TEMP TEMP TEMP TEMP TEMP TEMP
            void updateShaderPosition();

            void update();
            void bind();

            Camera3D();
    };

    Model* LoadModel(const std::string&);
    void UnloadModel(const std::string&);
    void UnloadModel(Model*);
}

#endif