#include <allegro5/base.h>
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
    // Engine::LOG(Engine::INFO) << "NewTurret timeElapsed: " << timeElapsed;
    if (timeElapsed > 10) {
        playScene->DeleteTurret(this, Position.y / PlayScene::BlockSize, Position.x / PlayScene::BlockSize);
    }
}