#include "engine_gl.hpp"

#include "debug.hpp"

namespace engine {
    double mouseX, mouseY, mouseXold = 0.0, mouseYold = 0.0, mouseMoveX, mouseMoveY;
    bool quit;
    int keyState[kb::KeycodesLength];
    bool keyPressed[kb::KeycodesLength];
    int keyStateTest[kb::KeycodesLength];

    void *currentShader = nullptr;
    
    void inputs() {        
        if(glfwWindowShouldClose(gl::window)) {
            quit = true;
        }

        glfwPollEvents();
        glfwGetCursorPos(gl::window, &mouseX, &mouseY);
        mouseMoveX = mouseX - mouseXold;
        mouseMoveY = mouseY - mouseYold;
        mouseXold = mouseX;
        mouseYold = mouseY;

        for(int i=0;i<kb::KeycodesLength;i++) {
            if(keyState[i] > 0 && keyStateTest[i] == 0) {
                keyPressed[i] = true;
            } else {
                keyPressed[i] = false;
            }
            keyStateTest[i] = keyState[i];
        }
    }

    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        keyState[key] = action;
    }

    namespace gl {
        GLFWwindow *window;
        // SDL_GLContext maincontext;

        

        VAO::VAO() {
            ID = new GLuint;
            glGenVertexArrays(1, ID);
        }

        VAO::~VAO() {
            glDeleteVertexArrays(1, ID);
            delete ID;
        }

        void VAO::bind() {
            glBindVertexArray(*ID);
        }

        void VAO::unbind() {
            glBindVertexArray(0);
        }

        VBO::VBO() {
            ID_VBO = new GLuint;
            ID_EBO = new GLuint;
            glGenBuffers(1, ID_VBO);
            glGenBuffers(1, ID_EBO);
            vertexAttribs = -1;
            bufferSizeVert = 0;
            bufferSizeInd = 0;
        }

        VBO::~VBO() {
            glDeleteBuffers(1, ID_VBO);
            glDeleteBuffers(1, ID_EBO);
            delete ID_VBO;
            delete ID_EBO;
        }

        void VBO::bind() {
            glBindBuffer(GL_ARRAY_BUFFER, *ID_VBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ID_EBO);
        }

        void VBO::unbind() {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        void VBO::createBuffer(size_t vertsize) {
            glBufferData(GL_ARRAY_BUFFER, vertsize, nullptr, GL_DYNAMIC_DRAW);
            bufferSizeVert = vertsize;
        }

        void VBO::createBuffer(size_t vertsize, size_t indsize) {
            glBufferData(GL_ARRAY_BUFFER, vertsize, nullptr, GL_DYNAMIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indsize, nullptr, GL_DYNAMIC_DRAW);
            bufferSizeVert = vertsize;
            bufferSizeInd = indsize;
        }        

        void VBO::bufferVerts(size_t vertsize, float *verts) {
            glBufferData(GL_ARRAY_BUFFER, vertsize, verts, GL_DYNAMIC_DRAW);
        }

        void VBO::bufferVerts(size_t vertsize, float *verts, size_t indsize, uint32_t *indices) {
            glBufferData(GL_ARRAY_BUFFER, vertsize, verts, GL_DYNAMIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indsize, indices, GL_DYNAMIC_DRAW);
        }

        void VBO::bufferVerts(size_t vertsize, modelVertex *verts) {
            glBufferData(GL_ARRAY_BUFFER, vertsize * sizeof(modelVertex), &verts[0], GL_DYNAMIC_DRAW);
        }

        void VBO::bufferVerts(size_t vertsize, modelVertex *verts, size_t indsize, uint32_t *indices) {
            glBufferData(GL_ARRAY_BUFFER, vertsize * sizeof(modelVertex), &verts[0], GL_DYNAMIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indsize * sizeof(uint32_t), indices, GL_DYNAMIC_DRAW);
        }

        void VBO::bufferSubVerts(size_t vertsize, float *verts) {
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertsize, verts);
        }

        void VBO::bufferSubVerts(size_t vertsize, float *verts, size_t indsize, uint32_t *indices) {
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertsize, verts);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indsize, indices);
        }

        void VBO::createVertexAttribPointer(size_t size, GLenum type, GLsizei stride, const void *pointer) {
            vertexAttribs += 1;
            glVertexAttribPointer(vertexAttribs, size, type, GL_FALSE, stride, pointer);
            glEnableVertexAttribArray(vertexAttribs);
        }

        void VBO::createVertexAttribPointerNormalized(size_t size, GLenum type, GLsizei stride, const void *pointer) {
            vertexAttribs += 1;
            glVertexAttribPointer(vertexAttribs, size, type, GL_TRUE, stride, pointer);
            glEnableVertexAttribArray(vertexAttribs);
        }

        FBO::FBO() {
            ID = new GLuint;
            glGenFramebuffers(1, ID);
        }

        FBO::~FBO() {
            glDeleteFramebuffers(1, ID);
            delete ID;
        }

        void FBO::bind() {
            glBindFramebuffer(GL_FRAMEBUFFER, *ID);
        }

        void FBO::unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            
        }

        RBO::RBO() {
            ID = new GLuint;
            glGenRenderbuffers(1, ID);
        }

        RBO::~RBO() {
            glDeleteRenderbuffers(1, ID);
            delete ID;
        }

        void RBO::bind() {
            glBindRenderbuffer(GL_RENDERBUFFER, *ID);
        }

        void RBO::unbind() {
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }

        FrameBuffer::FrameBuffer(int w, int h) {
            width = w;
            height = h;
            
            fbo = new FBO();
            rbo = new RBO();
            tex = new Texture();

            fbo->bind();
            rbo->bind();
            tex->bind();

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex->ID, 0);
            // tex->unbind();

            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo->ID);
            // rbo->unbind();
            fbo->unbind();
        }

        FrameBuffer::~FrameBuffer() {
            unbind();
            delete tex;
            delete rbo;
            delete fbo;
        }

        void FrameBuffer::bind() {
            glBindFramebuffer(GL_FRAMEBUFFER, *fbo->ID);
        }

        void FrameBuffer::unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        Texture::Texture() {
            ID = new GLuint;
            glGenTextures(1, ID);
            srcWidth = 0;
            srcHeight = 0;
            srcChannels = 0;
        }

        Texture::~Texture() {
            glDeleteTextures(1, ID);
            delete ID;
        }

        Texture::Texture(Texture&& t) {
            *this = std::move(t);
        }

        Texture& Texture::operator=(Texture&& t) {
            if(this != &t) {
                ID = t.ID;
                srcWidth = t.srcWidth;
                srcHeight = t.srcHeight;
                srcChannels = t.srcChannels;
                type = t.type;
                path = t.path;

                t.ID = new GLuint;
                t.path = std::string();
            }
            return *this;
        }

        void Texture::bind() {
            glBindTexture(GL_TEXTURE_2D, *ID);
        }

        void Texture::bind(int txUnit) {
            glActiveTexture(GL_TEXTURE0 + txUnit);
            glBindTexture(GL_TEXTURE_2D, *ID);
        }

        void Texture::activateUnit(int txUnit) {
            glActiveTexture(GL_TEXTURE0 + txUnit);
            // curTxUnit = txUnit;
        }

        // int Texture::getActiveUnit() {
        //     return curTxUnit;
        // }

        void Texture::unbind() {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        //  need to make more of these for different settings and shit
        void Texture::load(const std::string &path) {
            unsigned char *data = nullptr;
            if(loadFromZip) {
                assetsys_file_t file;
                assetsys_file(assets, path.c_str() + 1, &file);
                int size = assetsys_file_size(assets, file);
                if(size > 0) {
                    unsigned char *buffer = new unsigned char[size];
                    assetsys_file_load(assets, file, &size, (void*)buffer, size);
                    data = stbi_load_from_memory(buffer, size, &srcWidth, &srcHeight, &srcChannels, 0);
                    delete[] buffer;
                }
            } else {
                data = stbi_load(path.c_str(), &srcWidth, &srcHeight, &srcChannels, 0);
            }
            if(data) {
                switch(srcChannels) {
                    case 3:
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, srcWidth, srcHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                        break;
                    case 4:
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, srcWidth, srcHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                        break;
                }
                glGenerateMipmap(GL_TEXTURE_2D);
                stbi_image_free(data);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                //  this part in particular needs to be changed per thing
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                log_debug("loaded texture %s\n", path.c_str());
            } else {
                log_debug("failed to load texture %s\n", path.c_str());
            }
        }

        void Shader::load(const GLchar* vertexPath, const GLchar* fragmentPath)
        {
            const char* vShaderCode = nullptr;
            const char* fShaderCode = nullptr;
            bool fail = false;

            if(loadFromZip) {
                assetsys_file_t file;
                assetsys_file(assets, vertexPath + 1, &file);
                int size = assetsys_file_size(assets, file);
                if(size > 0) {
                    char *buffer = new char[size + 1];
                    assetsys_file_load(assets, file, &size, (void*)buffer, size);
                    buffer[size] = '\0';
                    vShaderCode = buffer;
                } else {
                    fail = true;
                }

                assetsys_file(assets, fragmentPath + 1, &file);
                size = assetsys_file_size(assets, file);
                if(size > 0) {
                    char *buffer = new char[size+1];
                    assetsys_file_load(assets, file, &size, (void*)buffer, size);
                    buffer[size] = '\0';
                    fShaderCode = buffer;
                } else {
                    fail = true;
                }
                if(fail) {
                    log_debug("failed to load shaders, unable to read files %s %s\n", vertexPath + 1, fragmentPath + 1);
                }
            } else {
                FILE *vFile;
                vFile = fopen(vertexPath, "rb");

                if(vFile) {
                    fseek(vFile, 0L, SEEK_END);
                    long size = ftell(vFile);
                    fseek(vFile, 0L, SEEK_SET);
                    char *buffer = new char[size + 1];
                    fread(buffer, size, 1, vFile);
                    buffer[size] = '\0';
                    fclose(vFile);
                    vShaderCode = buffer;
                } else {
                    log_debug("failed to load shader file %s\n", vertexPath);
                    fclose(vFile);
                }

                FILE *fFile;
                fFile = fopen(fragmentPath, "rb");

                if(fFile) {
                    fseek(fFile, 0L, SEEK_END);
                    long size = ftell(fFile);
                    fseek(fFile, 0L, SEEK_SET);
                    char *buffer = new char[size + 1];
                    fread(buffer, size, 1, fFile);
                    buffer[size] = '\0';
                    fclose(fFile);
                    fShaderCode = buffer;
                } else {
                    log_debug("failed to load shader file %s\n", fragmentPath);
                    fclose(fFile);
                }
            }

            if(!fail) {
                uint32_t vertex, fragment;
                int success;
                char infoLog[512];

                //vertex shader
                vertex = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vertex, 1, &vShaderCode, NULL);
                glCompileShader(vertex);

                glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
                if(!success) {
                    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
                    log_debug(infoLog);
                    //report error, infolog contains details
                };

                fragment = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fragment, 1, &fShaderCode, NULL);
                glCompileShader(fragment);

                glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
                if(!success) {
                    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
                    log_debug(infoLog);
                    //report error
                }

                ID = glCreateProgram();
                glAttachShader(ID, vertex);
                glAttachShader(ID, fragment);
                glLinkProgram(ID);

                glGetProgramiv(ID, GL_LINK_STATUS, &success);
                if(!success) {
                    // glGetProgramInfoLog(ID, 512, NULL, infoLog);
                    // printf("%s ", infoLog);
                }

                log_debug("loaded shaders %s %s\n", vertexPath, fragmentPath);

                glDeleteShader(vertex);
                glDeleteShader(fragment);
            }
        }

        void Shader::use()
        {
            if(currentShader != this) {
                glUseProgram(ID);
                currentShader = this;
                // log_debug("switched to shader %u\n", ID);
            }
            glUseProgram(ID);
        }

        void Shader::setBool(const std::string &name, bool value) const
        {         
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
        }
        void Shader::setInt(const std::string &name, int value) const
        { 
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
        }
        void Shader::setFloat(const std::string &name, float value) const
        { 
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 

        }
        void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
        { 
            glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
        }
        void Shader::setVec2(const std::string &name, float x, float y) const
        { 
            glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
        }
        // ------------------------------------------------------------------------
        void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
        { 
            glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
        }
        void Shader::setVec3(const std::string &name, float x, float y, float z) const
        { 
            glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
        }
        // ------------------------------------------------------------------------
        void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
        { 
            glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
        }
        void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
        { 
            glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
        }
        // ------------------------------------------------------------------------
        void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
        {
            glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        // ------------------------------------------------------------------------
        void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
        {
            glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        // ------------------------------------------------------------------------
        void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
        {
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        } 


    }
}