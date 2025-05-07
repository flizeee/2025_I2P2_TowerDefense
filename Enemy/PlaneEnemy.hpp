#ifndef PLANEENEMY_HPP
#define PLANEENEMY_HPP

#include "Enemy.hpp"

class PlaneEnemy : public Enemy {
public:
    PlaneEnemy(int x, int y);
    void Update(float deltaTime) override;
};

#endif // PLANEENEMY_HPP 