#ifndef NEWTURRET_HPP
#define NEWTURRET_HPP
#include "Turret.hpp"

class NewTurret : public Turret {
public:
    float timeElapsed = 0;
    bool placed = false;
    const float LIFETIME = 10.0f;  // Total lifetime in seconds

    static const int Price;
    NewTurret(float x, float y);
    void CreateBullet() override;
    void Update(float deltaTime) override;
    void Draw() const override;  // Add virtual Draw function
};
#endif   // NEWTURRET_HPP 