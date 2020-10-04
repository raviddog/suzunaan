#include "engine/engine.hpp"
#include "engine/debug.hpp"
#include "state.hpp"


engine::gl::FrameBuffer *fbuffer;
engine::SpriteInstance *destrect;
engine::Model *backpack;

engine::Camera3D *camera;
float cameraSpeed = 0.05f;

float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f
    };

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
        camera->mov_up += cameraSpeed;
    }
    if (engine::checkKey(engine::inputBomb)) {
        camera->mov_up -= cameraSpeed;
    }
    camera->angle_h -= engine::mouseMoveX * cameraSpeed;
    camera->angle_v -= engine::mouseMoveY * cameraSpeed;
    camera->update();
    
}

void Test3D::draw() {
    engine::SetDrawmode(engine::Drawmode3D);
    glViewport(0, 0, 640, 480);
    fbuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 4.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    engine::shader3d->setMat4("model", model);
    backpack->draw();
    fbuffer->unbind();
    
    engine::SetDrawmode(engine::DrawmodeUI);
    engine::pshader->setInt("screenTexture", 0);
    glViewport(640, 480, 640, 480);
    destrect->bind();
    fbuffer->tex->bind();
    destrect->draw(2);
    destrect->unbind();

    engine::SetDrawmode(engine::Drawmode3D);
}

Test3D::Test3D() {
    engine::SetDrawmode(engine::Drawmode3D);
    backpack = new engine::Model("./data/model/backpack.obj");
    camera = new engine::Camera3D();
    camera->bind();
    engine::shader3d->setInt("texture_diffuse1", 0);
    
    fbuffer = new engine::gl::FrameBuffer(1280, 960);
    destrect = new engine::SpriteInstance();
    destrect->bind();
    destrect->bufferVerts(sizeof(quadVertices), quadVertices);
    destrect->unbind();

    engine::mouseCapture();
}

Test3D::~Test3D() {
    delete backpack;
    delete camera;
}