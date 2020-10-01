#include "engine/engine.hpp"
#include "engine/debug.hpp"
#include "state.hpp"


engine::gl::FrameBuffer *fbuffer;
engine::SpriteInstance *destrect;
engine::ObjModel *backpack;

engine::Camera *camera;
float cameraSpeed = 0.05f;


void Test3D::logic() {
    if(engine::checkKey(engine::inputUp)) {
        camera->mov_dir_fw += cameraSpeed;
    }
    if(engine::checkKey(engine::inputDown)) {
        camera->mov_dir_fw -= cameraSpeed;
    }
    if(engine::checkKey(engine::inputLeft)) {
        camera->mov_dir_lf += cameraSpeed;
    }
    if(engine::checkKey(engine::inputRight)) {
        camera->mov_dir_lf -= cameraSpeed;
    }
    if (engine::checkKey(engine::inputFire)) {
        camera->angle += cameraSpeed * 45;
    }
    if (engine::checkKey(engine::inputBomb)) {
        camera->angle -= cameraSpeed * 45;
    }
    camera->update();
    
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
    backpack = new engine::ObjModel("./data/model/backpack.obj");
    camera = new engine::Camera();
    engine::shader3d->setInt("texture_diffuse1", 0);
}

Test3D::~Test3D() {
    delete backpack;
    delete camera;
}