#include "engine/engine.hpp"
#include "engine/debug.hpp"
#include "state.hpp"


engine::gl::FrameBuffer *fbuffer;
engine::SpriteInstance *destrect;
engine::Model *backpack;

glm::mat4 shader3d_projection, shader3d_view;
glm::vec3 shader3d_eye, shader3d_up, shader3d_direction;
float shader3d_angle = 0.0f;
float cameraSpeed = 0.05f;

float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
};


void Test3D::logic() {
    if(engine::checkKey(engine::inputUp)) {
        shader3d_eye += shader3d_direction * cameraSpeed;
    }
    if(engine::checkKey(engine::inputDown)) {
        shader3d_eye -= shader3d_direction * cameraSpeed;
    }
    if(engine::checkKey(engine::inputLeft)) {
        shader3d_eye += glm::vec3(shader3d_direction.z, 0.0f, -shader3d_direction.x) * cameraSpeed;
    }
    if(engine::checkKey(engine::inputRight)) {
        shader3d_eye -= glm::vec3(shader3d_direction.z, 0.0f, -shader3d_direction.x) * cameraSpeed;
    }
    if (engine::checkKey(engine::inputFire)) {
        shader3d_angle += cameraSpeed * 45;
    }
    if (engine::checkKey(engine::inputBomb)) {
        shader3d_angle -= cameraSpeed * 45;
    }
    shader3d_direction = glm::vec3(sin(glm::radians(shader3d_angle)), 0.0f, cos(glm::radians(shader3d_angle)));
    shader3d_direction = glm::normalize(shader3d_direction);
    shader3d_view = glm::lookAt(shader3d_eye, shader3d_eye + shader3d_direction, shader3d_up);

    engine::shader3d->setMat4("projection", shader3d_projection);
    engine::shader3d->setMat4("view", shader3d_view);
}

void Test3D::draw() {
    engine::SetDrawmode(engine::Drawmode3D);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    engine::shader3d->setMat4("model", model);
    backpack->draw();
}

Test3D::Test3D() {
    backpack = new engine::Model("./data/model/backpack.obj");
    shader3d_projection = glm::perspective(glm::radians(90.0f), (float)640 / (float)480, 0.1f, 100.0f);
    shader3d_up = glm::vec3(0.0f, 1.0f, 0.0f);
    shader3d_eye = glm::vec3(0.0f, 0.0f, 2.0f);
    shader3d_direction = glm::vec3(sin(glm::radians(shader3d_angle)), 0.0f, cos(glm::radians(shader3d_angle)));
    shader3d_direction = glm::normalize(shader3d_direction);
    shader3d_view = glm::lookAt(shader3d_eye, shader3d_eye + shader3d_direction, shader3d_up);

    engine::shader3d->setMat4("projection", shader3d_projection);
    engine::shader3d->setMat4("view", shader3d_view);
}

Test3D::~Test3D() {

}