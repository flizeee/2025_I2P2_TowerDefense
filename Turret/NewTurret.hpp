#ifndef NEWTURRET_HPP
#define NEWTURRET_HPP
#include "Turret.hpp"

class NewTurret : public Turret {
public:
    static const int Price;
    NewTurret(float x, float y);
    void CreateBullet() override;
    void Update(float deltaTime) override;
    float timeElapsed = 0;
    bool placed = 0;
};
#endif   // NEWTURRET_HPP 