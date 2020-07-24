#include "bullet.hpp"
#include "enemy.hpp"
#include "player.hpp"

#include "testenemy.hpp"


namespace Game::Script::Test{
    Game::enemy_s *testenemy;

    void teststageload() {
        testenemy = new enemy_s(320.f, 20.f, 1, 200);
        testenemy->angle = 0.2f;
        testenemy->speed = 2.f;
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
