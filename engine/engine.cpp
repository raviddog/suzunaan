#include "engine.hpp"

#include "debug.hpp"

#include <chrono>
#include <thread>
#include <string>
#include <fstream>
#include <unordered_map>

#ifdef _MSC_VER
#include "windows.h"
#include "timeapi.h"
#endif

#ifdef __GNUG__
#include "pthread.h"
#endif

#include "OBJ_Loader.h"

namespace engine {
    Camera3D *Camera3D::bound = nullptr;

    int scrWidth, scrHeight, drawWidth, drawHeight;
    int viewport[4], scrMode;
    float scalex, scaley;
    
    uint32_t controls[inputSkip + 1];

    //  framerate stuff
    static bool _vsync;
    uint32_t fps;
    double ticks, frameTimeTicks;
    std::chrono::high_resolution_clock::time_point cur_time, next_time;
    #define _ENGINE_NOVSYNC_DELAY_MICROSECONDS 16666

    gl::Shader *shaderSpriteSheet, *shaderSpriteSheetInvert, *shaderUI, *pshader, *shader3d;

    static Drawmode currentDrawmode;


//  managed model loading stuff
    class ManagedModel {
        public:
            int count = 1;
            Model *model;

            ManagedModel(const std::string&);
            ~ManagedModel();
    };

    ManagedModel::ManagedModel(const std::string &path) {
        model = new Model(path.c_str());
        count = 1;
    }

    ManagedModel::~ManagedModel() {
        delete model;
    }

    std::unordered_map<std::string, ManagedModel*> *loadedModels;

    Model* LoadModel(const std::string &path) {
        if(loadedModels->count(path)) {
            ManagedModel *tm = loadedModels->at(path);
            tm->count++;
            return tm->model;
        } else {
            ManagedModel *tm = new ManagedModel(path);
            loadedModels->insert(std::make_pair(path, tm));
            return tm->model;
        }
    }

    void UnloadModel(const std::string &path) {
        if(loadedModels->count(path)) {
            ManagedModel *tm = loadedModels->at(path);
            tm->count--;
            if(tm->count <= 0) {
                loadedModels->erase(path);
                delete tm;
            }
        } 
    }

    void UnloadModel(Model* md) {
        UnloadModel(md->path);
    }

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    Model::Model(const char *rawpath) {
        path = std::string(rawpath);

        materials = new std::vector<Material_t>();
        meshes = new std::vector<Mesh_t>();
        //  texture map
        textures = new std::unordered_map<std::string, gl::Texture*>();

        std::string directory = path.substr(0, path.find_last_of('/'));
        directory += '/';

        objl::Loader loader;
        if(loader.LoadFile(path.c_str())) {

            //  loop through and pull loaded materials
            //  also load textures as we go
            for(auto loadedMat = loader.LoadedMaterials.begin(); loadedMat != loader.LoadedMaterials.end(); loadedMat++) {
                //  load material
                Material_t material;
                material.name = loadedMat->name;
                material.colourAmbient = glm::vec3(loadedMat->Ka.X, loadedMat->Ka.Y, loadedMat->Ka.Z);
                material.colourDiffuse = glm::vec3(loadedMat->Kd.X, loadedMat->Kd.Y, loadedMat->Kd.Z);
                material.colourSpecular = glm::vec3(loadedMat->Ks.X, loadedMat->Ks.Y, loadedMat->Ks.Z);
                material.specularExponent = loadedMat->Ns;
                material.opticalDensity = loadedMat->Ni;
                material.dissolve = loadedMat->d;
                material.illumination = loadedMat->illum;
                material.texname_ambientMap = loadedMat->map_Ka;
                material.texname_diffuseMap = loadedMat->map_Kd;
                material.texname_specularMap = loadedMat->map_Ks;
                material.texname_alpha = loadedMat->map_d;
                material.texname_bump = loadedMat->map_bump;

                //  check if textures are loaded and if not, load them
                if(material.texname_ambientMap != "" && textures->count(material.texname_ambientMap) == 0) {
                    gl::Texture *tex = new gl::Texture();
                    tex->bind();
                    tex->load(directory + material.texname_ambientMap);
                    textures->insert(std::make_pair(material.texname_ambientMap, tex));
                }
                if(material.texname_diffuseMap != "" && textures->count(material.texname_diffuseMap) == 0) {
                    gl::Texture *tex = new gl::Texture();
                    tex->bind();
                    tex->load(directory + material.texname_diffuseMap);
                    textures->insert(std::make_pair(material.texname_diffuseMap, tex));
                }
                if(material.texname_specularMap != "" && textures->count(material.texname_specularMap) == 0) {
                    gl::Texture *tex = new gl::Texture();
                    tex->bind();
                    tex->load(directory + material.texname_specularMap);
                    textures->insert(std::make_pair(material.texname_specularMap, tex));
                }
                if(material.texname_alpha != "" && textures->count(material.texname_alpha) == 0) {
                    gl::Texture *tex = new gl::Texture();
                    tex->bind();
                    tex->load(directory + material.texname_alpha);
                    textures->insert(std::make_pair(material.texname_alpha, tex));
                }
                if(material.texname_bump != "" && textures->count(material.texname_bump) == 0) {
                    gl::Texture *tex = new gl::Texture();
                    tex->bind();
                    tex->load(directory + material.texname_bump);
                    textures->insert(std::make_pair(material.texname_bump, tex));
                }
                
                materials->push_back(material);
            }

            //  loop through meshes and do stuff?
            // for(auto loadedMesh = loader.LoadedMeshes.begin(); loadedMesh != loader.LoadedMeshes.end(); loadedMesh++) {
            for(uint32_t i = 0u; i < loader.LoadedMeshes.size(); i++) {
                auto loadedMesh = &loader.LoadedMeshes[i];

                Mesh_t mesh;
                mesh.name = loadedMesh->MeshName;
                mesh.vao = std::shared_ptr<gl::VAO>(new gl::VAO);
                mesh.vao->bind();
                mesh.vbo = std::shared_ptr<gl::VBO>(new gl::VBO);
                mesh.vbo->bind();
                mesh.vbo->createVertexAttribPointer(3, GL_FLOAT, 8 * sizeof(float), (void*)0);
                mesh.vbo->createVertexAttribPointer(3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
                mesh.vbo->createVertexAttribPointer(2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

                //  build vector of floats to buffer
                std::vector<float> vertices;
                std::vector<uint32_t> indices;
                // for(auto vertex = loadedMesh->Vertices.begin(); vertex != loadedMesh->Vertices.end(); vertex++) {
                for(uint32_t j = 0u; j < loadedMesh->Vertices.size(); j++) {
                    auto vertex = &loadedMesh->Vertices[j];
                    vertices.push_back(vertex->Position.X);
                    vertices.push_back(vertex->Position.Y);
                    vertices.push_back(vertex->Position.Z);
                    vertices.push_back(vertex->Normal.X);
                    vertices.push_back(vertex->Normal.Y);
                    vertices.push_back(vertex->Normal.Z);
                    vertices.push_back(vertex->TextureCoordinate.X);
                    vertices.push_back(vertex->TextureCoordinate.Y);

                    indices.push_back(j);
                }
                
                mesh.vbo->bufferVerts(  sizeof(float) * vertices.size(),
                                        vertices.data(),
                                        sizeof(uint32_t) * indices.size(),
                                        indices.data());

                mesh.vao->unbind();
                //  save number of indices
                mesh.indices = indices.size();
                //  find material name in vector
                size_t matCount = 0;
                for(; matCount < materials->size() && materials->at(matCount).name != loadedMesh->MeshMaterial.name; matCount++);
                if(matCount < materials->size()) {
                    //  material found
                    mesh.material = &materials->at(matCount);
                } else {
                    mesh.material = nullptr;
                }
                
                meshes->push_back(mesh);
            }
            gl::VAO::unbind();


        } else {
            //  load failed, error something
        }
    }

    void Model::draw() {
        for(auto i = meshes->begin(); i != meshes->end(); i++) {
            i->vao->bind();
            if(i->material) {
                if(textures->at(i->material->texname_diffuseMap)) {
                    textures->at(i->material->texname_diffuseMap)->bind(0);
                }
                if(textures->at(i->material->texname_specularMap)) {
                    textures->at(i->material->texname_specularMap)->bind(1);
                }
                // if(textures->at(i->material->texname_ambientMap)) {
                //     textures->at(i->material->texname_ambientMap)->bind(2);
                // }
                gl::Texture::activateUnit(0);
            }
            glDrawElements(GL_TRIANGLES, i->indices, GL_UNSIGNED_INT, (void*)0);
        }
    }

    Model::~Model() {
        gl::VAO::unbind();
        delete meshes;
        delete materials;
        emptyMap(textures);
        delete textures;
    }

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
        #ifdef _MSG_DEBUG_ENABLED_SPRITE
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

            verts->insert(verts->end(), std::begin(v), std::end(v));
            indices->insert(indices->end(), std::begin(ind), std::end(ind));
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

            verts->insert(verts->end(), std::begin(v), std::end(v));
            indices->insert(indices->end(), std::begin(ind), std::end(ind));
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

            verts->insert(verts->end(), std::begin(v), std::end(v));
            indices->insert(indices->end(), std::begin(ind), std::end(ind));
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

            verts->insert(verts->end(), std::begin(v), std::end(v));
            indices->insert(indices->end(), std::begin(ind), std::end(ind));
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
        SetDrawmode(DrawmodeSpriteInvert);
    }

    void SpriteSheet::useShaderNormal() {
        SetDrawmode(DrawmodeSprite);
    }

    SpriteInstance::SpriteInstance() {
        vao = new gl::VAO();
        vbo = new gl::VBO();

        vao->bind();
        vbo->bind();
        vbo->createVertexAttribPointer(2, GL_FLOAT, 4*sizeof(float), (void*)0);
        vbo->createVertexAttribPointer(2, GL_FLOAT, 4*sizeof(float), (void*)(2*sizeof(float)));
        vbo->unbind();
        vao->unbind();

        data = new SpriteData();
    }

    SpriteInstance::~SpriteInstance() {
        delete data;
        delete vbo;
        delete vao;
    }

    void SpriteInstance::bind() {
        //  what's this for?
        vao->bind();
    }

    void SpriteInstance::unbind() {
        gl::VAO::unbind();
    }

    void SpriteInstance::bufferVerts(size_t vertsize, float *verts) {
        vao->bind();
        vbo->bind();
        vbo->bufferVerts(vertsize, verts);
        vbo->unbind();
    }

    void SpriteInstance::draw(int triangles) {
        glDrawArrays(GL_TRIANGLES, 0, triangles * 3);
    }




    

    void InitialiseDrawmodes() {
        //  load draw modes (shaders)
        //  static gl::Shader *shaderSpriteSheet, *shaderSpriteSheetInvert, *shaderUI;
        
        glm::vec2 scrRes = glm::vec2((float)drawWidth, (float)drawHeight);

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

        shader3d = new gl::Shader();
        shader3d->load("./shaders/model.vert", "shaders/model.frag");
        
        pshader = new gl::Shader();
        pshader->load("./shaders/test.vert", "./shaders/test.frag");
        pshader->use();
        pshader->setInt("screenTexture", 0);


        currentDrawmode = DrawmodeUI;

    }

    void SetDrawmode(Drawmode dmode) {
        if(dmode != currentDrawmode) {
            switch(dmode) {
                case DrawmodeSprite:
                    shaderSpriteSheet->use();
                    currentDrawmode = DrawmodeSprite;
                    glDisable(GL_DEPTH_TEST);
                    break;
                case DrawmodeSpriteInvert:
                    shaderSpriteSheetInvert->use();
                    currentDrawmode = DrawmodeSpriteInvert;
                    glDisable(GL_DEPTH_TEST);
                    break;
                case Drawmode3D:
                    shader3d->use();
                    currentDrawmode = Drawmode3D;
                    glEnable(GL_DEPTH_TEST);
                    break;
                case DrawmodeUI:
                    pshader->use();
                    currentDrawmode = DrawmodeUI;
                    glDisable(GL_DEPTH_TEST);
                    break;
                default:
                    break;
            }
        }
    }

    //  configure the resolution setting of the current drawmode
    void ConfigureDrawmodeSpriteResolution(int x, int y) {
        glm::vec2 scrRes = glm::vec2((float)x, (float)y);
        shaderSpriteSheet->setVec2("res", scrRes);
    }

    void ConfigureDrawmodeSpriteTexture(int txunit) {
        shaderSpriteSheet->setInt("txUnit", txunit);
    }

    void ConfigureDrawmodeUITexture(int txunit) {
        pshader->setInt("screenTexture", txunit);
    }

    Camera3D::Camera3D() {
        angle_h = 0.f;
        angle_v = 0.f;
        mov_fw = 0.f;
        mov_up = 0.f;
        mov_lf = 0.f;
        mov_dir_fw = 0.f;
        mov_dir_lf = 0.f;

        dir_x = glm::vec3(1.f, 0.f, 0.f);
        dir_y = glm::vec3(0.f, 1.f, 0.f);
        dir_z = glm::vec3(0.f, 0.f, 1.f);

        //  todo add settings to this shit
        projection = glm::perspective(glm::radians(90.f), 640.f / 480.f, 0.1f, 100.f);
        eye = glm::vec3(0.f, 0.f, 0.f);
        direction = glm::vec3(sin(glm::radians(angle_h)), sin(glm::radians(angle_v)), cos(glm::radians(angle_h)));
        direction = glm::normalize(direction);
        view = glm::lookAt(eye, eye + direction, dir_y);
    }

    void Camera3D::update() {
        eye += dir_x * mov_fw;
        eye += dir_y * mov_up;
        eye += dir_z * mov_lf;

        //  cap up and down so far
        //  need to do trig on this shit for the up and down looking, fuck
        //  do it later
        if(angle_v > 90.f) angle_v = 90.f;
        if(angle_v < -90.f) angle_v = -90.f;

        direction = glm::vec3(sin(glm::radians(angle_h)), sin(glm::radians(angle_v)), cos(glm::radians(angle_h)));
        direction = glm::normalize(direction);

        eye += direction * mov_dir_fw;
        eye += glm::vec3(direction.z, 0.f, -direction.x) * mov_dir_lf;

        view = glm::lookAt(eye, eye + direction, dir_y);

        mov_fw = 0.f;
        mov_up = 0.f;
        mov_lf = 0.f;

        mov_dir_fw = 0.f;
        mov_dir_lf = 0.f;

        if(bound == this) {
            shader3d->setMat4("view", view);
            engine::shader3d->setVec3("viewPos", eye.x, eye.y, eye.z);
        }
    }

    void Camera3D::bind() {
        bound = this;
        // shader3d temp
        shader3d->setMat4("projection", projection);
        shader3d->setMat4("view", view);
    }

    void Camera3D::updateShaderPosition() {
        engine::shader3d->setVec3("viewPos", eye.x, eye.y, eye.z);
    }

    ModelInstance::ModelInstance() {
        modelmat = glm::mat4(1.f);
    }

    void ModelInstance::translate(float x, float y, float z) {
        modelmat = glm::translate(modelmat, glm::vec3(x, y, z));
    }

    void ModelInstance::scale(float x, float y, float z) {
        modelmat = glm::scale(modelmat, glm::vec3(x, y, z));
    }

    void ModelInstance::rotate(float degrees, float axis_x, float axis_y, float axis_z) {
        modelmat = glm::rotate(modelmat, glm::radians(degrees), glm::vec3(axis_x, axis_y, axis_z));
    }

    void ModelInstance::bind() {
        shader3d->setMat4("model", modelmat);
    }

    //  load settings from file
    bool init(const char *title, const char *settingsPath) {
        debug_init();

        //  default controls if there's none in config
        controls[inputUp] = 82;
        controls[inputDown] = 81;
        controls[inputLeft] = 80;
        controls[inputRight] = 79;
        controls[inputFire] = 29;
        controls[inputFocus] = 225;
        controls[inputBomb] = 27;
        controls[inputPause] = 41;
        controls[inputQuit] = 20;
        controls[inputRestart] = 21;
        controls[inputSkip] = 224;

        std::string inputStrings[] = {
            "Up",
            "Down",
            "Left",
            "Right",
            "Fire",
            "Focus",
            "Bomb",
            "Pause",
            "Quit",
            "Restart",
            "Skip"
        };

        std::ifstream file;//(settingsPath, std::ifstream::in);
        std::string settings;
        //file.exceptions(std::ifstream::failbit | std::ifstream::badbit);


        try {
            file.open(settingsPath);
            std::stringstream filestream;
            filestream << file.rdbuf();
            file.close();
            settings = filestream.str();
        } catch(std::ifstream::failure &ex) {
            engine::log_debug("failed to open file, %s\n%s",  strerror(errno), ex.what());
            file.close();
            return false;
        }
        //  settings
        int screenMode = 0;
        bool vsync = true;
        int width_win = 640, height_win = 480;
        const int width_draw = 640, height_draw = 480;  //  modify for non-static resolution shmup stuff


        size_t next = 0, offset = 0, length;
        length = settings.length();


        while(next < length) {
            if(settings[next] == '[' || settings[next] == ';') {
                while(next < length && settings[next] != '\n') {
                    next++;
                }
            }
            if(settings[next] == '\n') {
                next++;
            } else {
                offset = 0;
                while(next + offset < length && settings[next + offset] != '=') {
                    offset++;
                }
                std::string id = settings.substr(next, offset);
                next += offset + 1;
                offset = 0;

                while(next + offset < length && settings[next + offset] != '\n') {
                    offset++;
                }
                std::string value = settings.substr(next, offset);
                next += offset + 1;
                offset = 0;

                if(id == "Width") {
                    width_win = std::strtol(value.c_str(), nullptr, 0);
                } else if(id == "Height") {
                    height_win = std::strtol(value.c_str(), nullptr, 0);
                } else if(id == "Vsync") {
                    vsync = value == "true" ? true : false;
                } else if(id == "Screenmode") {
                    if(value == "windowed") screenMode = 4;
                    if(value == "borderless") screenMode = 1;
                    if(value == "fullscreen") screenMode = 2;
                } else {
                    //  check if its an input setting
                    int x = 0;
                    while(x < inputSkip && id != inputStrings[x]) {
                        ++x;
                    }

                    if(x < inputSkip) {
                        //  found
                        //  https://wiki.libsdl.org/SDLScancodeLookup
                        controls[x] = stoul(value);
                    }
                }

            }

        }
        init(title, screenMode, vsync, width_win, height_win, width_draw, height_draw);
        return true;
    }

    void init(const char *title, int screenMode, bool vsync, int width, int height) {
        init(title, screenMode, vsync, width, height, width, height);
    }

    void init(const char *title, int screenMode, bool vsync, int width_win, int height_win, int width_draw, int height_draw) {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //add option for this later

        for(int i = 0; i < kb::KeycodesLength; i++) {
            keyState[i] = 0;
        }


        drawWidth = width_draw;
        drawHeight = height_draw;
        scrMode = screenMode;
        fps = 0u;
        ticks = glfwGetTime();
        frameTimeTicks = ticks;

        //  for borderless fullscreen calculation
        const GLFWvidmode *dmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        switch(screenMode) {
            case 1:
                //  borderless fullscreen
                glfwWindowHint(GLFW_RED_BITS, dmode->redBits);
                glfwWindowHint(GLFW_GREEN_BITS, dmode->greenBits);
                glfwWindowHint(GLFW_BLUE_BITS, dmode->blueBits);
                glfwWindowHint(GLFW_REFRESH_RATE, dmode->refreshRate);
                gl::window = glfwCreateWindow(dmode->width, dmode->height, title, glfwGetPrimaryMonitor(), NULL);
                scrWidth = dmode->width;
                scrHeight = dmode->height;
                break;
            case 2:
                //  fullscreen
                gl::window = glfwCreateWindow(width_draw, height_draw, title, glfwGetPrimaryMonitor(), NULL);
                scrWidth = width_draw;
                scrHeight = height_draw;
                break;
            case 3:
                //  test, fullscreen but draw canvas mapped to screen res
                gl::window = glfwCreateWindow(width_draw, height_draw, title, glfwGetPrimaryMonitor(), NULL);
                scrWidth = width_draw;
                scrHeight = height_draw;
                break;
            case 0:
            default:
                //  normal windowed
                gl::window = glfwCreateWindow(width_win, height_win, title, NULL, NULL);
                scrWidth = width_win;
                scrHeight = height_win;
                break;
        }
        
        glfwMakeContextCurrent(gl::window);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        glfwSetKeyCallback(gl::window, key_callback);

        switch(screenMode) {
            case 3:
            case 1:
            {
                float draw_ratio = (float)width_draw / (float)height_draw;
                float screen_ratio = (float)dmode->width / (float)dmode->height;
                if(draw_ratio > screen_ratio) {
                    //  draw area is wider than screen
                    float y_scale = (float)dmode->width / (float)width_draw;
                    float height = (float)height_draw * y_scale;
                    int offset = (dmode->height - (int)height) / 2;
                    glViewport(0, offset, dmode->width, (int)height);
                    viewport[0] = 0;
                    viewport[1] = offset;
                    viewport[2] = dmode->width;
                    viewport[3] = (int)height;
                    break;
                } else if(draw_ratio < screen_ratio) {
                    //  draw area is narrower than screen
                    float x_scale = (float)dmode->height / (float)height_draw;
                    float width = (float)width_draw * x_scale;
                    int offset = (dmode->width - (int)width) / 2;
                    glViewport(offset, 0, (int)width, dmode->height);
                    viewport[0] = offset;
                    viewport[1] = 0;
                    viewport[2] = (int)width;
                    viewport[3] = dmode->height;
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

        //  precalculate stuff for setviewport
        //  set viewport to specified rectangle (inside draw area)
        //  need to calculate x and y based off of the existing draw area
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

        if(vsync) {
            _vsync = true;
            glfwSwapInterval(1);
        } else {
            _vsync = false;
            glfwSwapInterval(0);

            next_time = std::chrono::high_resolution_clock::now() + std::chrono::microseconds(_ENGINE_NOVSYNC_DELAY_MICROSECONDS);
            // next_time = std::chrono::high_resolution_clock::now();
            #ifdef _MSC_VER
            timeBeginPeriod(1);
            #endif
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // stbi_set_flip_vertically_on_load(true);  // don't need this because the shader i wrote accounts for it
        
        InitialiseDrawmodes(); 
        SetDrawmode(DrawmodeSprite);

        loadedModels = new std::unordered_map<std::string, ManagedModel*>();
    }

    void flip() {       

        using namespace std::chrono;
        //  flip buffers
        glfwSwapBuffers(gl::window);

        //  clear new buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        #ifdef _MSG_DEBUG_ENABLED_FPS
        uint32_t slept;
        #endif
        //  if vsync disabled, cap fps
        int temp = 0;
        if(!_vsync) {
            //  wait
            #ifdef _MSG_DEBUG_ENABLED_FPS
            high_resolution_clock::time_point sleep = high_resolution_clock::now();
            #endif
            #ifdef __GNUG__
            next_time += microseconds(_ENGINE_NOVSYNC_DELAY_MICROSECONDS);
            timespec delayt, delayr;
            nanoseconds delaym = duration_cast<nanoseconds>(next_time - high_resolution_clock::now());
            delayt.tv_sec = 0;
            delayt.tv_nsec = delaym.count();
            delayr.tv_nsec = 0;
            do {
                nanosleep(&delayt, &delayr);
            } while (delayr.tv_nsec > 0);
            #else
            next_time = high_resolution_clock::now() + microseconds(_ENGINE_NOVSYNC_DELAY_MICROSECONDS);
            std::this_thread::sleep_until(next_time);
            #endif

            #ifdef _MSG_DEBUG_ENABLED_FPS
            slept = duration_cast<milliseconds>(high_resolution_clock::now() - sleep).count();
            #endif

            // auto wake_time = steady_clock::now();
            while(high_resolution_clock::now() < next_time) {
                //  spin
                temp++;
            }
        }

        #ifdef _MSG_DEBUG_ENABLED_FPS
        //  print debug fps data
        double temp_ticks = glfwGetTime();
        if(temp_ticks > ticks + 1.0) {
            std::stringstream d;
            d << "Frame time: " << temp_ticks - frameTimeTicks << "ms | ";
            d << "FPS: " << fps;
            if(!_vsync) {
                d << " | Slept: " << slept << "ms | ";
                d << "Spun: " << temp;
            }
            glfwSetWindowTitle(gl::window, d.str().c_str());
            fps = 0u;
            ticks = temp_ticks;
            temp = 0u;
        }
        frameTimeTicks = temp_ticks;
        ++fps;
        #endif

        
    }

    void close() {
        #ifdef _MSC_VER
        if(!_vsync) timeEndPeriod(1);
        #endif
        glfwDestroyWindow(gl::window);
        glfwTerminate();
    }

    void setViewport() {
        //  no arguments resets the viewport to original
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        glm::vec2 scrRes = glm::vec2((float)drawWidth, (float)drawHeight);
        shaderSpriteSheet->setVec2("res", scrRes);
    }

    void setViewport(int x, int y, int w, int h)
    {
        glViewport( viewport[0] + (int)(scalex * (float)x),
                    viewport[1] + (int)(scaley * (float)y),
                    (int)(scalex * (float)w),
                    (int)(scaley * (float)h));
        glm::vec2 scrRes = glm::vec2((float)(w - x), (float)(h - y));
        shaderSpriteSheet->setVec2("res", scrRes);
    }

    bool checkKey(int key) {
        return keyState[controls[key]] == GLFW_PRESS || keyState[controls[key]] == GLFW_REPEAT;
    }

    bool checkKeyPressed(int key) {
        return keyPressed[controls[key]];
    }

    void mouseCapture() {
        glfwSetInputMode(gl::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if(glfwRawMouseMotionSupported()) {
            glfwSetInputMode(gl::window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
    }

    void mouseRelease() {
        glfwSetInputMode(gl::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

}