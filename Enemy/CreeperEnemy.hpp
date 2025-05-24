#ifndef CREEPERENEMY_HPP
#define CREEPERENEMY_HPP
#include "Enemy.hpp"
#include "Engine/Resources.hpp"
#include <memory>

class CreeperEnemy : public Enemy {
private:
    std::shared_ptr<ALLEGRO_BITMAP> shockwave;

public:
    CreeperEnemy(int x, int y);
    void OnExplode() override;
};

#endif // CREEPERENEMY_HPP 