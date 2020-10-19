#include "enemy.hpp"

#include <cmath>
#include <vector>

#define PI 3.14159265


namespace Game{
    // bullet_s* (*enemy_s::getBullet)();
    extern std::map<int, std::shared_ptr<bullet_script>> *bullet_scripts;
    extern std::map<int, std::shared_ptr<enemy_script>> *enemy_scripts;
    extern std::vector<enemy_s*> *spawn_enemies;
    // std::vector<enemy_s*> *enemy_s::enemy_draw = nullptr;

    //  #FIX 1
    float   enem_bounds_x = -8.f,
            enem_bounds_y = 0.f,
            enem_bounds_xmax = 392.f,
            enem_bounds_ymax = 448.f;
    

    //  #FIX 51
    //  create a common functions engine module and put these in it later
    template <class T>
    T between(T min, T max);
    float toRadians(float deg);
    float toDegrees(float rad);

    enemy_s::enemy_s() {
        active_instructions = new std::vector<int>();
        cust_triggers = new std::multimap<int, int>();
        cancel_cust_triggers = new std::set<int>();
        listener_triggers = nullptr;

        active = false;
        owned = false;
        moveDir = 0;
        id = -1;
        owner = -1;
        type = -1;
        animFrame = 0;
        animDelay = 0;
        frames = 0;
        hp = 1;
        x_pos = 320.f;
        y_pos = 0.f;
        speed = 0.f;
        angle = 0.f;
        accel = 0.f;
        angle_change = 0.f;
        draw_angle = 0.f;
    }

    enemy_s::~enemy_s() {
        delete active_instructions;
        delete cust_triggers;
        delete cancel_cust_triggers;
        delete listener_triggers;
    }

    void enemy_s::activate() {
        if(instructions) {
            //  run init trigger
            listener_triggers = new auto(*(instructions->listeners));
            if(listener_triggers->count(2u) > 0) {
                auto range = listener_triggers->equal_range(2u);
                auto it = range.first;
                while(it != range.second) {
                    //  activate all
                    active_instructions->push_back(it->second.second);
                    it = listener_triggers->erase(it);
                }
            }
        }
        run_instructions();
        active = true;
        enemy_draw->push_back(this);
    }

    void enemy_s::update() {
        if(active) {            
            if(hp > 0) {
                if(instructions) {
                    if(instructions->frame_triggers->count(frames) > 0) {
                        //  add instructions from the vector to the active instructions
                        std::vector<int> *ins = instructions->frame_triggers->at(frames);
                        for(size_t i = 0; i < ins->size(); i++) {
                            active_instructions->push_back(ins->at(i));
                        }
                    }
                    //  check listener triggers
                    if(listener_triggers) {
                        //  dunno yet
                    }
                }

                //  run custom frame triggers
                if(cust_triggers->count(frames) > 0) {
                    //  could've used auto lol
                    std::pair<  std::multimap<int, int>::iterator,
                                std::multimap<int, int>::iterator> range = cust_triggers->equal_range(frames);
                    for(auto it = range.first; it != range.second; ++it) {
                        //  check if this trigger is to be canceled
                        if(cancel_cust_triggers->count(it->second) > 0) {
                            //  don't run it
                            //  since it's a frame trigger, it can't be reached again, so there's no need to delete it
                            //  remove the cancel though, so that this trigger can be created again if so
                            cancel_cust_triggers->erase(it->second);
                        } else {
                            active_instructions->push_back(it->second);
                        }
                    }
                }

                
            } else {
                active = false;
                if(listener_triggers) {
                    if(listener_triggers->count(3u) > 0) {
                        auto range = listener_triggers->equal_range(3u);
                        auto it = range.first;
                        while(it != range.second) {
                            //  activate all
                            active_instructions->push_back(it->second.second);
                            it = listener_triggers->erase(it);
                        }
                    }
                }
            }

            run_instructions();

            speed += accel;
            angle += angle_change;
            if(angle > 360.f) angle -= 360.f;
            if(angle < 0.f) angle += 360.f;
            x_pos += std::sin(angle * PI / 180.f) * speed;
            y_pos += std::cos(angle * PI / 180.f) * speed;

            //  time delay for animation update
            animDelay += 1;
            if(animDelay >= 4) {
                animDelay = 0;
                animFrame += 1;
            }

            if(angle < 355.f && angle > 185.f && moveDir != 1) {
                //  switch to moving left
                animDelay = 0;
                animFrame = 8;
                moveDir = 1;

            } else if(angle < 175.f && angle > 5.f && moveDir != 2) {
                //  switch to moving right
                animDelay = 0;
                animFrame = 16;
                moveDir = 2;
            } else if((angle < 5.f || (angle > 175.f && angle < 185.f) || angle > 355.f) && moveDir != 0) {
                //  stop moving side
                animDelay = 0;
                animFrame = 0;
                moveDir = 0;
            }

            switch(type) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                    switch(moveDir) {
                        case 0:
                            if(animFrame >= 4) {
                                animFrame = 0;
                            }
                            break;
                        case 1:
                        case 2:
                            if(animFrame >= 12) {
                                animFrame = 8;
                            }
                            break;
                    }
                default:
                    break;
            };

            if(hp <= 0) active = false;


            //  self disable, hardcoded for now
            //  not sure how to actually do this, maybe ill just make the enemy take damage when its outside the screen
            //  because i might find myself storing enemies offscreen for a while and stuff
            //  before they enter the screen later
            if(x_pos > enem_bounds_xmax) active = false;
            if(x_pos < enem_bounds_x) active = false;
            if(y_pos > enem_bounds_ymax) active = false;
            if(y_pos < enem_bounds_y) active = false;

            ++frames;
        }
    }

     void enemy_s::run_instructions() {
        for(size_t i = 0; i < active_instructions->size();) {
            int num = active_instructions->at(i);
            //  get active script instruction from instruction list
            if(instructions->instructions->count(num) > 0) {
                //  instruction does exist
                script_instruction *current = instructions->instructions->at(num);
                //  in case the instruction deletes itself with stop()
                bool deleted = false;
                //  run each instruction
                for(size_t j = 0; j < current->instruct->size(); j++) {
                    script_args args = current->val->at(j);
                    switch(current->instruct->at(j)) {
                        case 1:
                            instr_speed(args.type_1);
                            break;
                        case 2:
                            instr_angle(args.type_1);
                            break;
                        case 3:
                            instr_accel(args.type_1);
                            break;
                        case 4:
                            instr_bullet(args.type_2);
                            break;
                        case 5:
                            instr_setHP(args.type_2);
                            break;
                        case 6:
                        {
                            //  if this removes an element before the current instruction, need to decrement i
                            size_t result = instr_stop(args.type_2);
                            if(result != active_instructions->size()) {
                                if(result < i) {
                                    i--;
                                } else if(result == i) {
                                    //  removed the instruction we're currently dealing with
                                    //  the instruction is already gone, so we can skip the selfdestruct check
                                    //  we dont want to permanently modify the instruction though
                                    deleted = true;
                                }
                            }
                            break;   
                        }
                        case 7:
                            instr_start(args.type_2);
                            break;
                        case 8:
                        {
                            std::pair<int, int> val = script_getIntInt(args.type_4);
                            instr_frameTrigger(val.first, val.second);
                            break;
                        }
                        case 9:
                        {
                            std::pair<int, int> val = script_getIntInt(args.type_4);
                            instr_frameTriggerOffset(val.first, val.second);
                            break;
                        }
                        case 10:
                            instr_stopInterval(args.type_2);
                            break;
                        case 11:
                            instr_enemy(args.type_2);
                            break;
                        case 12:
                            instr_angle_change(args.type_1);
                            break;
                        case 13:
                        {
                            std::pair<int, int> val = script_getIntInt(args.type_4);
                            instr_bullet_ring(val.first, val.second);
                            break;
                        }
                        default:
                            break;
                    }
                }
                //  iterate i if the instruction isn't self destructing, else remove
                if(!deleted) {
                    if(current->selfdestruct) {
                        auto loc = active_instructions->begin() + i;
                        active_instructions->erase(loc);
                    } else {
                        i++;
                    }
                }
            } else {
                //  instruction doesn't exist, remove it from active
                auto loc = active_instructions->begin() + i;
                active_instructions->erase(loc);
            }
        }
    }

    void enemy_s::instr_speed(float val) {
        speed = val;
    }

    void enemy_s::instr_angle(float val) {
        angle = val;
    }

    void enemy_s::instr_accel(float val) {
        accel = val;
    }

    void enemy_s::instr_bullet(int spawnID) {
        if(instructions->bullet_spawns->count(spawnID) > 0) {
            bullet_spawn bs = instructions->bullet_spawns->at(spawnID);
            bullet_s *bullet = getBullet();
            if(bullet) {
                //  initialise bullet with spawn values
                bullet->type = bs.type;
                bullet->x_pos = x_pos + bs.x_offset;
                bullet->y_pos = y_pos + bs.y_offset;
                bullet->speed = bs.speed;
                bullet->angle = bs.angle;
                bullet->accel = bs.accel;
                bullet->angle_change = bs.angle_change;
                bullet->instructions = nullptr;
                bullet->owner = id;

                if(bs.scriptID > 0) {
                    if(bullet_scripts->count(bs.scriptID) > 0) {
                        bullet->instructions = (bullet_scripts->at(bs.scriptID)).get();
                    }
                }
            }
        }
    }

    void enemy_s::instr_bullet_ring(int spawnID, int num) {
        if(instructions->bullet_spawns->count(spawnID) > 0) {
            bullet_spawn bs = instructions->bullet_spawns->at(spawnID);
            float initAngle = this->angle;
            float angleStep = 360.f / (float)num;
            for(int i = 0; i < num; i++) {
                bullet_s *bullet = getBullet();
                if(bullet) {
                    //  initialise bullet with spawn values
                    bullet->type = bs.type;
                    bullet->x_pos = x_pos + bs.x_offset;
                    bullet->y_pos = y_pos + bs.y_offset;
                    bullet->speed = bs.speed;
                    bullet->angle = bs.angle + (i * angleStep);
                    bullet->accel = bs.accel;
                    bullet->angle_change = bs.angle_change;
                    bullet->instructions = nullptr;
                    bullet->owner = id;

                    if(bs.scriptID > 0) {
                        if(bullet_scripts->count(bs.scriptID) > 0) {
                            bullet->instructions = (bullet_scripts->at(bs.scriptID)).get();
                        }
                    }
                }
            }
        }
    }

    void enemy_s::instr_setHP(int newhp) {
        hp = newhp;
    }

    size_t enemy_s::instr_stop(int val) {
        size_t i = 0;
        for(;i < active_instructions->size(); i++) {
            //  found instruction
            if(active_instructions->at(i) == val) break;
        }
        if(i != active_instructions->size()) {
            //  instruction is active, remove it
            auto loc = active_instructions->begin() + i;
            active_instructions->erase(loc);
            return i;
        } else {
            //  return last element + 1
            return active_instructions->size();
        }
    }

    void enemy_s::instr_start(int val) {
        //  first check if instruction is a real instruction
        if(instructions->instructions->count(val) > 0) {
            //  don't duplicate instructions, search active array to check if instruction is already running
            auto i = active_instructions->begin();
            for(; i != active_instructions->end(); i++) {
                //  found instruction
                if(*i == val) break;
            }
            if(i == active_instructions->end()) {
                //  not found, activate
                active_instructions->push_back(val);
            }
        }
    }

    void enemy_s::instr_frameTrigger(int id, int frame) {
        cust_triggers->insert({frame, id});
    }

    void enemy_s::instr_frameTriggerOffset(int id, int frame) {
        cust_triggers->insert({frame + frames, id});
    }

    void enemy_s::instr_stopInterval(int id) {
        //  can only have 1 of each element in a set, so no need to check if it already exists
        cancel_cust_triggers->insert(id);
    }

    void enemy_s::instr_enemy(int spawnID) {
        if(instructions->bullet_spawns->count(spawnID) > 0) {
            enemy_spawn es = instructions->enemy_spawns->at(spawnID);
            enemy_s *enemy = new enemy_s();
            if(enemy) { 
                enemy->type = es.type;
                enemy->x_pos = x_pos + es.x_offset;
                enemy->y_pos = y_pos + es.y_offset;
                enemy->speed = es.speed;
                enemy->angle = es.angle;
                enemy->accel = es.accel;
                enemy->angle_change = 0.f;
                enemy->instructions = nullptr;
                enemy->owner = id;
                enemy->id = -1;

                if(es.scriptID > 0) {
                    if(enemy_scripts->count(es.scriptID) > 0) {
                        enemy->instructions = (enemy_scripts->at(es.scriptID)).get();
                    }
                }
                spawn_enemies->push_back(enemy);
            }
        }
    }

    void enemy_s::instr_angle_change(float val) {
        angle += val;
    }
}