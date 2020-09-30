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

#include "OBJ_Loader.h"

namespace engine {
    

    int scrWidth, scrHeight, drawWidth, drawHeight;
    int viewport[4], scrMode;
    float scalex, scaley;
    
    uint32_t controls[inputSkip + 1];

    //  framerate stuff
    static bool _vsync;
    uint32_t ticks, fps, frameTimeTicks;
    std::chrono::high_resolution_clock::time_point cur_time, next_time;
    #define _ENGINE_NOVSYNC_DELAY_MICROSECONDS 16666

    gl::Shader *shaderSpriteSheet, *shaderSpriteSheetInvert, *shaderUI, *pshader, *shader3d;

    static Drawmode currentDrawmode;

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    ObjModel::ObjModel(const char *rawpath) {
        materials = new std::vector<Material_t>();
        meshes = new std::vector<Mesh_t>();
        //  texture map
        textures = new std::unordered_map<std::string, std::shared_ptr<gl::Texture>>();

        std::string path = rawpath;
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
                    textures->insert(std::make_pair(material.texname_ambientMap, std::shared_ptr<gl::Texture>(tex)));
                }
                if(material.texname_diffuseMap != "" && textures->count(material.texname_diffuseMap) == 0) {
                    gl::Texture *tex = new gl::Texture();
                    tex->bind();
                    tex->load(directory + material.texname_diffuseMap);
                    textures->insert(std::make_pair(material.texname_diffuseMap, std::shared_ptr<gl::Texture>(tex)));
                }
                if(material.texname_specularMap != "" && textures->count(material.texname_specularMap) == 0) {
                    gl::Texture *tex = new gl::Texture();
                    tex->bind();
                    tex->load(directory + material.texname_specularMap);
                    textures->insert(std::make_pair(material.texname_specularMap, std::shared_ptr<gl::Texture>(tex)));
                }
                if(material.texname_alpha != "" && textures->count(material.texname_alpha) == 0) {
                    gl::Texture *tex = new gl::Texture();
                    tex->bind();
                    tex->load(directory + material.texname_alpha);
                    textures->insert(std::make_pair(material.texname_alpha, std::shared_ptr<gl::Texture>(tex)));
                }
                if(material.texname_bump != "" && textures->count(material.texname_bump) == 0) {
                    gl::Texture *tex = new gl::Texture();
                    tex->bind();
                    tex->load(directory + material.texname_bump);
                    textures->insert(std::make_pair(material.texname_bump, std::shared_ptr<gl::Texture>(tex)));
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
                int matCount = 0;
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

    void ObjModel::draw() {
        for(auto i = meshes->begin(); i != meshes->end(); i++) {
            i->vao->bind();
            if(i->material) {
                textures->at(i->material->texname_diffuseMap)->bind(0);
            }
            glDrawElements(GL_TRIANGLES, i->indices, GL_UNSIGNED_INT, (void*)0);
        }
    }
/*
    Mesh::Mesh(std::vector<gl::modelVertex> vertices, std::vector<uint32_t> indices, std::vector<gl::Texture*> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        setupMesh();
    }

    void Mesh::setupMesh() {
        vao = new gl::VAO();
        vbo = new gl::VBO();

        vao->bind();
        vbo->bind();

        vbo->bufferVerts(vertices.size(), vertices.data(), indices.size(), indices.data());

        vbo->createVertexAttribPointer(3, GL_FLOAT, sizeof(gl::modelVertex), (void*)0);
        vbo->createVertexAttribPointer(3, GL_FLOAT, sizeof(gl::modelVertex), (void*)offsetof(gl::modelVertex, Normal));
        vbo->createVertexAttribPointer(2, GL_FLOAT, sizeof(gl::modelVertex), (void*)offsetof(gl::modelVertex, TexCoords));

        vao->unbind();
    }

    void Mesh::draw() {
        //  something about bind textures
        //  optional bunch of textures for like different shit
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string name = textures[i]->type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++);
            
            //  3d shader
            shader3d->setInt((name + number).c_str(), i);
            textures[i]->bind();
        }
        glActiveTexture(GL_TEXTURE0);
        
        vao->bind();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }

    void Model::draw() {
        for(int i = 0; i < meshes.size(); i++) {
            meshes[i]->draw();
        }
    }

    void Model::loadModel(std::string path) {
        Assimp::Importer importer;
        // const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            printf("ERROR::ASSIMP:: %s", importer.GetErrorString());
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));
        directory += '/';
        
        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode *node, const aiScene *scene) {
        //  process all node's meshes (if any)
        for(int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        //  call recursively for all children
        for(int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh* Model::processMesh(aiMesh *mesh, const aiScene *scene) {
        std::vector<gl::modelVertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<gl::Texture*> textures;

        //  process vertices
        for(int i = 0; i < mesh->mNumVertices; i++) {
            gl::modelVertex vertex;

            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector; 
            

            if(mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);  
            }
         
            vertices.push_back(vertex);
        }

        for(int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for(int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        if(mesh->mMaterialIndex >= 0) {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<gl::Texture*> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            std::vector<gl::Texture*> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }

        return new Mesh(vertices, indices, textures);
    }

    std::vector<gl::Texture*> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
        std::vector<gl::Texture*> textures;
        for(int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for(int j = 0; j < loadedTextures.size(); j++) {
                if(std::strcmp(loadedTextures[j]->path.data(), str.C_Str()) == 0) {
                    textures.push_back(loadedTextures[j]);
                    skip = true;
                    break;
                }
            }
            if(!skip) {
                gl::Texture *texture = new gl::Texture();
                texture->bind();
                texture->load(directory + str.C_Str());
                texture->type = typeName;
                texture->path = str.C_Str();
                textures.push_back(texture);
                loadedTextures.push_back(texture);
            }
        }
        return textures;
    }
*/
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
        shaderSpriteSheetInvert->use();
    }

    void SpriteSheet::useShaderNormal() {
        shaderSpriteSheet->use();
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
                case Drawmode3D:
                    shader3d->use();
                    currentDrawmode = Drawmode3D;
                    glEnable(GL_DEPTH_TEST);
                    break;
                case DrawmodeUI:
                    pshader->use();
                    currentDrawmode = DrawmodeUI;
                default:
                    break;
            }
        }
    }

    //  configure the resolution setting of the current drawmode
    void ConfigureDrawmodeSpriteResolution(int x, int y) {
        glm::vec2 scrRes = glm::vec2((float)drawWidth, (float)drawHeight);
        shaderSpriteSheet->setVec2("res", scrRes);
    }

    void ConfigureDrawmodeSpriteTexture(int txunit) {
        shaderSpriteSheet->setInt("txUnit", txunit);
    }

    void ConfigureDrawmodeUITexture(int txunit) {
        pshader->setInt("screenTexture", txunit);
    }

    
    

    









    //  load settings from file
    bool init(const char *title, const char *settingsPath) {
        debug_init();

        //  default controls if there's none in config
        controls[inputUp] = 2;
        controls[inputDown] = 3;
        controls[inputLeft] = 0;
        controls[inputRight] = 1;
        controls[inputFire] = 58;
        controls[inputFocus] = 57;
        controls[inputBomb] = 59;
        controls[inputPause] = 4;
        controls[inputQuit] = 32;
        controls[inputRestart] = 35;
        controls[inputSkip] = 69;

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

        std::ifstream settings(settingsPath, std::ifstream::in);
        if(settings.good()) {
            
            //  settings
            int screenMode = 0;
            bool vsync = true;
            int width_win = 640, height_win = 480;
            const int width_draw = 640, height_draw = 480;

            while(settings.peek() != EOF) {
                if(settings.peek() == '[' || settings.peek() == ';') {
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
                    } else {
                        //  check if its an input setting
                        std::getline(settings, id);
                        int x = 0;
                        while(x < inputSkip && id != inputStrings[x]) {
                            ++x;
                        }

                        if(x < inputSkip) {
                            //  found
                            controls[x] = stoul(id);
                        }
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

        SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO);
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

        // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glEnable(GL_BLEND);
        //glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // stbi_set_flip_vertically_on_load(true);  // don't need this because the shader i wrote accounts for it
        
        InitialiseDrawmodes(); 
        SetDrawmode(DrawmodeSprite);
    }

    void flip() {       

        using namespace std::chrono;
        //  flip buffers
        SDL_GL_SwapWindow(gl::window);

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
            timespec delayt, delayr;
            nanoseconds delaym = duration_cast<nanoseconds>(next_time - high_resolution_clock::now());
            delayt.tv_sec = 0;
            delayt.tv_nsec = delaym.count();
            delayr.tv_nsec = 0;
            do {
                nanosleep(&delayt, &delayr);
            } while (delayr.tv_nsec > 0);
            #else
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

            next_time = high_resolution_clock::now() + microseconds(_ENGINE_NOVSYNC_DELAY_MICROSECONDS);
            // next_time += microseconds(_ENGINE_NOVSYNC_DELAY_MICROSECONDS);
            
            
        }

        #ifdef _MSG_DEBUG_ENABLED_FPS
        //  print debug fps data
        uint32_t temp_ticks = SDL_GetTicks();
        if(temp_ticks > ticks + 1000) {
            if(!_vsync) {
                log_debug("slept for %u ms ", slept);
                log_debug("spun %d times ", temp);
            }
            log_debug("frame time: %ums, ", temp_ticks - frameTimeTicks);
            log_debug("fps: %u\n", fps);
            fps = 0u;
            ticks = temp_ticks;
            temp = 0u;
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
        glViewport( viewport[0] + (int)(scalex * (float)x),
                    viewport[1] + (int)(scaley * (float)y),
                    (int)(scalex * (float)w),
                    (int)(scaley * (float)h));
        glm::vec2 scrRes = glm::vec2((float)(w - x), (float)(h - y));
        shaderSpriteSheet->setVec2("res", scrRes);
    }

    bool checkKey(int key) {
        return keyState[controls[key]];
    }

    bool checkKeyPressed(int key) {
        return keyPressed[controls[key]];
    }

}