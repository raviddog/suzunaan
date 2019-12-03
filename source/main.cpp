#include "engine/engine.h"

const int scrWidth = 640;
const int scrHeight = 480;

float vert[] = {
    -0.3f, 0.3f, 0.0f, 1.0f,
    -0.3f, -0.3f, 0.0f, 0.0f,
    0.3f, -0.3f, 1.0f, 0.0f,
    0.3f, 0.3f, 1.0f, 1.0f
};

uint32_t ind[] = {
    0, 1, 2,
    0, 3, 2
};

int main(int args, char** argv) {

    engine::init("test", scrWidth, scrHeight);
    engine::gl::VAO *vao = new engine::gl::VAO();
    engine::gl::VBO *vbo = new engine::gl::VBO();
    engine::gl::Shader *shader = new engine::gl::Shader();
    engine::gl::Texture *texture = new engine::gl::Texture();
    
    vao->init();
    vao->bind();

    vbo->init();
    vbo->bind();
    vbo->bufferVerts(sizeof(vert), vert, sizeof(ind), ind);
    vbo->createVertexAttribPointer(2, GL_FLOAT, 4*sizeof(float), (void*)0);
    vbo->createVertexAttribPointer(2, GL_FLOAT, 4*sizeof(float), (void*)(2*sizeof(float)));

    shader->load("./shaders/flat.vert", "./shaders/flat.frag");
    shader->use();
    shader->setInt("tx", 0);

    texture->init();
    texture->bind();
    texture->load("./data/pl00.png");

    // engine::gl::VAO::unbind();
    vao->unbind();

    while(!engine::quit) {
        engine::inputs();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        vao->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(engine::gl::window);
    }

    return 0;
}



