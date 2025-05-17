#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "UI/Component/Label.hpp"

class Bullet;
class PlayScene;
class Turret;
class Label;

class Enemy : public Engine::Sprite {
protected:
    std::vector<Engine::Point> path;
    float speed;
    float hp;
    float maxHp;  // Store max HP to calculate health bar percentage
    int money;
    bool hasBeenDamaged;  // Track if enemy has been damaged
    PlayScene *getPlayScene();
    virtual void OnExplode();

public:
    float reachEndTime;
    std::list<Turret *> lockedTurrets;
    std::list<Bullet *> lockedBullets;
    Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money);
    void Hit(float damage);
    void UpdatePath(const std::vector<std::vector<int>> &mapDistance);
    void Update(float deltaTime) override;
    void Draw() const override;
    Engine::Label* hpLabel;
};
#endif   // ENEMY_HPP
