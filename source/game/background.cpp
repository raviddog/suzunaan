#include "background.hpp"
#include "engine/engine.hpp"

#include <vector>

namespace Game {

    class Background {
        private:
            std::vector<engine::ModelInstance*> *draw;
            engine::Camera3D *camera;
        public:
            Background(int, int);
            ~Background();

            engine::gl::FrameBuffer *fbuffer;

            void Bind();
            void Unbind();
            void Draw();
    };

    Background::Background(int w, int h) {
        draw = new std::vector<engine::ModelInstance*>();
        camera = new engine::Camera3D();
        fbuffer = new engine::gl::FrameBuffer(w, h);
    }
}