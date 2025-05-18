#include <allegro5/base.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <string>

#include "Bullet/FireBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Engine/LOG.hpp"
#include "NewTurret.hpp"
#include "Scene/PlayScene.hpp"

const int NewTurret::Price = 20; // 1.5x MachineGunTurret price
NewTurret::NewTurret(float x, float y)
    : Turret("play/tower-base.png", "play/turret-7.png", x, y, 200, Price, 0.125) { // 0.25 cooldown for double attack speed
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}

void NewTurret::CreateBullet() {
    placed = true;
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36, diff, rotation, this));
    AudioHelper::PlayAudio("gun.wav");
}

void NewTurret::Update(float deltaTime) {
    Turret::Update(deltaTime);
    if (!placed) return;
    auto playScene = getPlayScene();
    timeElapsed += deltaTime;
    if (timeElapsed > LIFETIME) {
        playScene->DeleteTurret(this, Position.y / PlayScene::BlockSize, Position.x / PlayScene::BlockSize);
    }
}

void NewTurret::Draw() const {
    Turret::Draw();
    if (placed) {
        // Draw countdown bar background (red)
        const float barWidth = 40.0f;
        const float barHeight = 4.0f;
        const float barY = Position.y - 40.0f;  // Position above the turret
        const float barX = Position.x - barWidth / 2.0f;  // Center the bar
        
        // Draw background (red)
        al_draw_filled_rectangle(barX, barY, barX + barWidth, barY + barHeight, al_map_rgba(255, 0, 0, 180));
        
        // Draw foreground (yellow) based on remaining time percentage
        float timePercentage = 1.0f - (timeElapsed / LIFETIME);
        al_draw_filled_rectangle(barX, barY, barX + barWidth * timePercentage, barY + barHeight, al_map_rgba(255, 255, 0, 180));
    }
}