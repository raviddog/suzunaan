#include "engine/engine.h"

const int scrWidth = 640;
const int scrHeight = 480;

float vert[] = {
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f
};

uint32_t ind[] = {
    0, 1, 2,
    0, 1, 3
};
int main(int args, char** argv) {

    engine::init("test", scrWidth, scrHeight);
    // engine::gl::VAO *vao = new engine::gl::VAO();
    // engine::gl::VBO *vbo = new engine::gl::VBO();
    // engine::gl::Shader *shader = new engine::gl::Shader();
    // engine::gl::Texture *texture = new engine::gl::Texture();
    
    // vao->init();
    // vao->bind();

    // vbo->init();
    // vbo->bind();
    // vbo->bufferVerts(sizeof(vert), vert, sizeof(ind), ind);
    // vbo->createVertexAttribPointer(4, GL_FLOAT, 4*sizeof(float), (void*)0);

    // shader->load("./shaders/sprite.vert", "./shaders/sprite.frag");
    // shader->use();
    // shader->setInt("txUnit", 0);
    // glm::mat4 scrProjection = glm::ortho(0.0f, static_cast<float>(scrWidth), 0.0f, static_cast<float>(scrHeight), -1.0f, 1.0f);
    // shader->setMat4("scrProjection", scrProjection);

    // texture->init();
    // texture->bind();
    // texture->load("./data/pl00.png");

    // vao->unbind();

    engine::SpriteSheet *s = new engine::SpriteSheet();
    s->load("./data/pl00.png", 1);
    int frame = 0;
    while(!engine::quit) {
        engine::inputs();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        s->drawSprite(1, 10, 10);
        s->drawSprite(1, 200, 200);
        s->drawSprite(1, 0, 0);
        s->drawSprite(1, 2, 1);
        s->drawSprite(0, 400, 0);
        
        
        // vao->bind();
        // shader->use();
        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(1.0f, 1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3((float)texture->srcWidth, (float)texture->srcHeight, 1.0f));
        
        // shader->setMat4("scrModel", model);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // vao->unbind();

        SDL_GL_SwapWindow(engine::gl::window);
        // printf("frame %d\n", frame++);
    }
    engine::close();
    return 0;
}



