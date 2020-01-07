#include "engine_gl.hpp"


namespace engine {
    bool quit;
    bool keyState[83];
    bool keyPressed[83];
    bool keyStateTest[83];

    void inputs() {
        static SDL_Event e;
        
        if(SDL_GetWindowFlags(gl::window) && SDL_WINDOW_INPUT_FOCUS) {
            while(SDL_PollEvent(&e) != 0)
            {
                static bool curState = false;

                if(e.type == SDL_QUIT) {
                    quit = true;
                } else if(e.type == SDL_KEYDOWN) {
                    curState = true;
                } else if(e.type == SDL_KEYUP) {
                    curState = false;
                }


                switch(e.key.keysym.sym)
                {
                    case SDLK_LEFT:
                        keyState[kbLeft] = curState;
                        break;
                    case SDLK_RIGHT:
                        keyState[kbRight] = curState;
                        break;
                    case SDLK_UP:
                        keyState[kbUp] = curState;
                        break;
                    case SDLK_DOWN:
                        keyState[kbDown] = curState;
                        break;
                    case SDLK_ESCAPE:
                        keyState[kbEscape] = curState;
                        break;
                    case SDLK_F1:
                        keyState[kbF1] = curState;
                        break;
                    case SDLK_F2:
                        keyState[kbF2] = curState;
                        break;
                    case SDLK_F3:
                        keyState[kbF3] = curState;
                        break;
                    case SDLK_F4:
                        keyState[kbF4] = curState;
                        break;
                    case SDLK_F5:
                        keyState[kbF5] = curState;
                        break;
                    case SDLK_F6:
                        keyState[kbF6] = curState;
                        break;
                    case SDLK_F7:
                        keyState[kbF7] = curState;
                        break;
                    case SDLK_F8:
                        keyState[kbF8] = curState;
                        break;
                    case SDLK_F9:
                        keyState[kbF9] = curState;
                        break;
                    case SDLK_F10:
                        keyState[kbF10] = curState;
                        break;
                    case SDLK_F11:
                        keyState[kbF11] = curState;
                        break;
                    case SDLK_F12:
                        keyState[kbF12] = curState;
                        break;
                    case SDLK_BACKQUOTE:
                        keyState[kbTilde] = curState;
                        break;
                    case SDLK_1:
                        keyState[kb1] = curState;
                        break;
                    case SDLK_2:
                        keyState[kb2] = curState;
                        break;
                    case SDLK_3:
                        keyState[kb3] = curState;
                        break;
                    case SDLK_4:
                        keyState[kb4] = curState;
                        break;
                    case SDLK_5:
                        keyState[kb5] = curState;
                        break;
                    case SDLK_6:
                        keyState[kb6] = curState;
                        break;
                    case SDLK_7:
                        keyState[kb7] = curState;
                        break;
                    case SDLK_8:
                        keyState[kb8] = curState;
                        break;
                    case SDLK_9:
                        keyState[kb9] = curState;
                        break;
                    case SDLK_0:
                        keyState[kb0] = curState;
                        break;
                    case SDLK_MINUS:
                        keyState[kbMinus] = curState;
                        break;
                    case SDLK_EQUALS:
                        keyState[kbEquals] = curState;
                        break;
                    case SDLK_BACKSPACE:
                        keyState[kbBackspace] = curState;
                        break;
                    case SDLK_TAB:
                        keyState[kbTab] = curState;
                        break;
                    case SDLK_q:
                        keyState[kbQ] = curState;
                        break;
                    case SDLK_w:
                        keyState[kbW] = curState;
                        break;
                    case SDLK_e:
                        keyState[kbE] = curState;
                        break;
                    case SDLK_r:
                        keyState[kbR] = curState;
                        break;
                    case SDLK_t:
                        keyState[kbT] = curState;
                        break;
                    case SDLK_y:
                        keyState[kbY] = curState;
                        break;
                    case SDLK_u:
                        keyState[kbU] = curState;
                        break;
                    case SDLK_i:
                        keyState[kbI] = curState;
                        break;
                    case SDLK_o:
                        keyState[kbO] = curState;
                        break;
                    case SDLK_p:
                        keyState[kbP] = curState;
                        break;
                    case SDLK_LEFTBRACKET:
                        keyState[kbLBracket] = curState;
                        break;
                    case SDLK_RIGHTPAREN:
                        keyState[kbRBracket] = curState;
                        break;
                    case SDLK_BACKSLASH:
                        keyState[kbBackslash] = curState;
                        break;
                    case SDLK_a:
                        keyState[kbA] = curState;
                        break;
                    case SDLK_s:
                        keyState[kbS] = curState;
                        break;
                    case SDLK_d:
                        keyState[kbD] = curState;
                        break;
                    case SDLK_f:
                        keyState[kbF] = curState;
                        break;
                    case SDLK_g:
                        keyState[kbG] = curState;
                        break;
                    case SDLK_h:
                        keyState[kbH] = curState;
                        break;
                    case SDLK_j:
                        keyState[kbJ] = curState;
                        break;
                    case SDLK_k:
                        keyState[kbK] = curState;
                        break;
                    case SDLK_l:
                        keyState[kbL] = curState;
                        break;
                    case SDLK_SEMICOLON:
                        keyState[kbColon] = curState;
                        break;
                    case SDLK_QUOTE:
                        keyState[kbQuote] = curState;
                        break;
                    case SDLK_RETURN:
                        keyState[kbEnter] = curState;
                        break;
                    case SDLK_LSHIFT:
                        keyState[kbLShift] = curState;
                        break;
                    case SDLK_z:
                        keyState[kbZ] = curState;
                        break;
                    case SDLK_x:
                        keyState[kbX] = curState;
                        break;
                    case SDLK_c:
                        keyState[kbC] = curState;
                        break;
                    case SDLK_v:
                        keyState[kbV] = curState;
                        break;
                    case SDLK_b:
                        keyState[kbB] = curState;
                        break;
                    case SDLK_n:
                        keyState[kbN] = curState;
                        break;
                    case SDLK_m:
                        keyState[kbM] = curState;
                        break;
                    case SDLK_COMMA:
                        keyState[kbComma] = curState;
                        break;
                    case SDLK_PERIOD:
                        keyState[kbPeriod] = curState;
                        break;
                    case SDLK_SLASH:
                        keyState[kbSlash] = curState;
                        break;
                    case SDLK_RSHIFT:
                        keyState[kbRShift] = curState;
                        break;
                    case SDLK_LCTRL:
                        keyState[kbLCtrl] = curState;
                        break;
                    case SDLK_LGUI:
                        keyState[kbLWin] = curState;
                        break;
                    case SDLK_LALT:
                        keyState[kbLAlt] = curState;
                        break;
                    case SDLK_SPACE:
                        keyState[kbSpace] = curState;
                        break;
                    case SDLK_RALT:
                        keyState[kbRAlt] = curState;
                        break;
                    case SDLK_RGUI:
                        keyState[kbRWin] = curState;
                        break;
                    case SDLK_MENU:
                        keyState[kbMenu] = curState;
                        break;
                    case SDLK_RCTRL:
                        keyState[kbRCtrl] = curState;
                        break;
                    case SDLK_INSERT:
                        keyState[kbInsert] = curState;
                        break;
                    case SDLK_HOME:
                        keyState[kbHome] = curState;
                        break;
                    case SDLK_PAGEUP:
                        keyState[kbPgup] = curState;
                        break;
                    case SDLK_DELETE:
                        keyState[kbDelete] = curState;
                        break;
                    case SDLK_END:
                        keyState[kbEnd] = curState;
                        break;
                    case SDLK_PAGEDOWN:
                        keyState[kbPgdn] = curState;
                        break;
                    default:
                        break;
                }
            }
            
            for(int i=0;i<82;i++) {
                if(keyState[i] && !(keyStateTest[i])) {
                    keyPressed[i] = true;
                } else {
                    keyPressed[i] = false;
                }
                keyStateTest[i] = keyState[i];
            }
        } else {
            for(int i=0;i<82;i++) {
                keyState[i] = false;
                keyPressed[i] = false;
                keyStateTest[i] = false;;
            }
        }
    } 

    void *currentShader = NULL;

    namespace gl {
        SDL_Window *window;
        SDL_GLContext maincontext;

        VAO::VAO() {
            ID = new GLuint();
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
            ID_VBO = new GLuint();
            ID_EBO = new GLuint();
            glGenBuffers(1, ID_VBO);
            glGenBuffers(1, ID_EBO);
            vertexAttribs = -1;            
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

        void VBO::createBuffer(int vertsize) {
            glBufferData(GL_ARRAY_BUFFER, vertsize, nullptr, GL_DYNAMIC_DRAW);
            bufferSizeVert = vertsize;
        }

        void VBO::createBuffer(int vertsize, int indsize) {
            glBufferData(GL_ARRAY_BUFFER, vertsize, nullptr, GL_DYNAMIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indsize, nullptr, GL_DYNAMIC_DRAW);
            bufferSizeVert = vertsize;
            bufferSizeInd = indsize;
        }        

        void VBO::bufferVerts(int vertsize, float *verts) {
            glBufferData(GL_ARRAY_BUFFER, vertsize, verts, GL_DYNAMIC_DRAW);
        }

        void VBO::bufferVerts(int vertsize, float *verts, int indsize, uint32_t *indices) {
            glBufferData(GL_ARRAY_BUFFER, vertsize, verts, GL_DYNAMIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indsize, indices, GL_DYNAMIC_DRAW);
        }

        void VBO::bufferSubVerts(int vertsize, float *verts) {
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertsize, verts);
        }

        void VBO::bufferSubVerts(int vertsize, float *verts, int indsize, uint32_t *indices) {
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertsize, verts);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indsize, indices);
        }

        void VBO::createVertexAttribPointer(int size, GLenum type, GLsizei stride, const void *pointer) {
            vertexAttribs += 1;
            glVertexAttribPointer(vertexAttribs, size, type, GL_FALSE, stride, pointer);
            glEnableVertexAttribArray(vertexAttribs);
        }

        void VBO::createVertexAttribPointerNormalized(int size, GLenum type, GLsizei stride, const void *pointer) {
            vertexAttribs += 1;
            glVertexAttribPointer(vertexAttribs, size, type, GL_TRUE, stride, pointer);
            glEnableVertexAttribArray(vertexAttribs);
        }

        Texture::Texture() {
            ID = new GLuint();
            glGenTextures(1, ID);
        }

        Texture::~Texture() {
            glDeleteTextures(1, ID);
            delete ID;
        }

        void Texture::bind() {
            glBindTexture(GL_TEXTURE_2D, *ID);
        }

        void Texture::unbind() {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void Texture::load(const std::string &path) {
            unsigned char *data = stbi_load(path.c_str(), &srcWidth, &srcHeight, &srcChanels, 0);
            if(data) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, srcWidth, srcHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
                stbi_image_free(data);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                printf("loaded texture %s\n", path.c_str());
            } else {
                printf("failed to load texture %s\n", path.c_str());
            }
        }

        void Shader::load(const GLchar* vertexPath, const GLchar* fragmentPath)
        {
            std::string vertexCode;
            std::string fragmentCode;
            std::ifstream vShaderFile;
            std::ifstream fShaderFile;

            //ensure ifstream objects can throw exceptions
            vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
            fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

            try
            {
                //open files
                vShaderFile.open(vertexPath);
                fShaderFile.open(fragmentPath);
                std::stringstream vShaderStream, fShaderStream;

                //read file's buffer contents into streams
                vShaderStream << vShaderFile.rdbuf();
                fShaderStream << fShaderFile.rdbuf();

                //close file handlers
                vShaderFile.close();
                fShaderFile.close();

                //convert stream into string
                vertexCode = vShaderStream.str();
                fragmentCode = fShaderStream.str();
            }
            catch(std::ifstream::failure e)
            {
                printf("failed to load shaders, unable to read files %s %s\n", vertexPath, fragmentPath);
            }

            const char* vShaderCode = vertexCode.c_str();
            const char* fShaderCode = fragmentCode.c_str();

            uint32_t vertex, fragment;
            int success;
            char infoLog[512];

            //vertex shader
            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &vShaderCode, NULL);
            glCompileShader(vertex);

            glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
            if(!success) {
                //glGetShaderInfoLog(vertex, 512, NULL, infoLog);
                //report error, infolog contains details
            };

            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &fShaderCode, NULL);
            glCompileShader(fragment);

            glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
            if(!success) {
                //glGetShaderInfoLog(fragment, 512, NULL, infoLog)
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

            printf("loaded shaders %s %s\n", vertexPath, fragmentPath);

            glDeleteShader(vertex);
            glDeleteShader(fragment);
        }

        void Shader::use()
        {
            if(currentShader != this) {
                glUseProgram(ID);
                currentShader = this;
                printf("switched to shader %d\n", ID);
            }
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