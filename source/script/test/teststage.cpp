#include "bullet.hpp"
#include "enemy.hpp"
#include "player.hpp"

#include "testenemy.hpp"


namespace Game::Script::Test{
    Game::enemy_s *testenemy;

    void teststageload() {
        testenemy = new enemy_s();
        testenemy->x_pos = 320.f;
        testenemy->y_pos = 20.f;
        testenemy->angle = 0.2f;
        testenemy->speed = 2.f;
        testenemy->hp = 100;
        testenemy->run_instructions = Game::Script::Test::testenemyfunc;
    }

    void teststageunload() {
        delete testenemy;
    }

    void teststagefunc(int frames) {
        if(frames == 30) {
            testenemy->activate();
        }
    }
}
