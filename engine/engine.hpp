#ifndef _ENGINE_H
#define _ENGINE_H

#include "gl.hpp"
#include "input.hpp"
#include "engine_helper.hpp"
#include "util.h"

#include <vector>
#include <memory>
#include <unordered_map>

namespace engine {
  

    enum Drawmode {
        DrawmodeSprite,
        DrawmodeSpriteInvert,
        DrawmodeUI,
        Drawmode3D
    };

    

    extern gl::Shader *shaderSpriteSheet, *shaderSpriteSheetInvert, *shaderUI, *pshader, *shader3d;

    extern double deltatime;


    bool init(const char *title, int flags, int width, int height, const char *settingsPath);
    void init(const char *title, int flags, int width, int height);
    void init(const char *title, int flags, int width, int height, int dwidth, int dheight);
    void inputs();
    void flip();
    void close();

    void setViewport();
    void setViewport(int x, int y, int w, int h);
    void setDrawsize(int w, int h);


    //  imgui stuff
    //  accompanying text, pointer to variable, editable, window
    void registerDebugWindow(std::string);
    void registerDebugVariable(std::string, float*, bool);
    void registerDebugVariable(std::string, float*, bool, size_t);
    void registerDebugVariable(std::string, int*, bool);
    void registerDebugVariable(std::string, int*, bool, size_t);
    void registerDebugVariable(std::string, bool*, bool);
    void registerDebugVariable(std::string, bool*, bool, size_t);
    void registerDebugVariable(std::string, double*, bool);
    void registerDebugVariable(std::string, double*, bool, size_t);

    void removeDebugWindow(std::string);

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
            bool realloc;

            void load(const std::string &path, int numSprites);
            void load(const std::string &path, int numSprites, size_t maxDraws);
        public:
        //  TODO this is here coz of font, seems wrong, clean up later
            gl::Texture *tex;
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

    class BitmapFont {
        protected:
            //  store or calculate char info
            //  store for now

            struct Charinfo {
                int w;
                int h;
            } charinfo;

            SpriteSheet *s;
        
        public:
            
            
            BitmapFont(std::string);          //  font file
            // BitmapFont(std::string, int);     //  font file, optional font size
                                                //  dont need this on a bitmap font
            ~BitmapFont();



            void Write(std::string, float x, float y);
            void Write(std::string, float x, float y, int w); //  specify char width in pixels
            void WriteCentered(std::string, float x, float y);
            void WriteCentered(std::string, float x, float y, int w); //  specify char width in pixels
            void Dimensions(std::string, int*, int*);   //  get the expected dimensions to draw string
            void Dimensions(std::string, int*, int*, int*);   //  get the expected dimensions to draw string

            void buffer();
            void draw();
        

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