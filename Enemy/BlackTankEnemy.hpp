#ifndef BLACKTANKENEMY_HPP
#define BLACKTANKENEMY_HPP
#include "Enemy.hpp"

class BlackTankEnemy : public Enemy {
public:
    BlackTankEnemy(int x, int y);
    void OnExplode() override;
};
#endif   // BLACKTANKENEMY_HPP 