#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>

#include "Bullet/Bullet.hpp"
#include "Enemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"
#include "Turret/Turret.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/ExplosionEffect.hpp"

PlayScene *Enemy::getPlayScene() const {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
void Enemy::OnExplode() {
    getPlayScene()->EffectGroup->AddNewObject(new ExplosionEffect(Position.x, Position.y));
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> distId(1, 3);
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 20);
    for (int i = 0; i < 10; i++) {
        // Random add 10 dirty effects.
        getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-" + std::to_string(distId(rng)) + ".png", dist(rng), Position.x, Position.y));
    }
}
Enemy::Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money) : Engine::Sprite(img, x, y), speed(speed), hp(hp), maxHp(hp), money(money), hasBeenDamaged(false), flashTimer(0) {
    CollisionRadius = radius;
    reachEndTime = 0;
    hpLabel = new Engine::Label(std::to_string((int)(hp)), "pirulen.ttf", 14, Position.x, Position.y, 255, 255, 255);
}
void Enemy::Hit(float damage) {
    hp -= damage;
    hasBeenDamaged = true;  // Set damaged flag when hit
    flashTimer = 0.1f;  // Set flash duration to 0.2 seconds
    hpLabel->Text = std::to_string((int)(hp));
    if (hp <= 0) {
        OnExplode();
        // Remove all turret's reference to target.
        for (auto &it : lockedTurrets)
            it->Target = nullptr;
        for (auto &it : lockedBullets)
            it->Target = nullptr;
        getPlayScene()->EarnMoney(money);
        getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
        AudioHelper::PlayAudio("explosion.wav");
    }
}
void Enemy::UpdatePath(const std::vector<std::vector<int>> &mapDistance) {
    int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
    int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
    if (x < 0) x = 0;
    if (x >= PlayScene::MapWidth) x = PlayScene::MapWidth - 1;
    if (y < 0) y = 0;
    if (y >= PlayScene::MapHeight) y = PlayScene::MapHeight - 1;
    Engine::Point pos(x, y);
    int num = mapDistance[y][x];
    if (num == -1) {
        num = 0;
        Engine::LOG(Engine::ERROR) << "Enemy path finding error";
    }
    path = std::vector<Engine::Point>(num + 1);
    while (num != 0) {
        std::vector<Engine::Point> nextHops;
        for (auto &dir : PlayScene::directions) {
            int x = pos.x + dir.x;
            int y = pos.y + dir.y;
            if (x < 0 || x >= PlayScene::MapWidth || y < 0 || y >= PlayScene::MapHeight || mapDistance[y][x] != num - 1)
                continue;
            nextHops.emplace_back(x, y);
        }
        // Choose arbitrary one.
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, nextHops.size() - 1);
        pos = nextHops[dist(rng)];
        path[num] = pos;
        num--;
    }
    path[0] = PlayScene::EndGridPoint;
}
void Enemy::Update(float deltaTime) {
    // Update flash timer
    if (flashTimer > 0) {
        flashTimer -= deltaTime;
    }
    
    // Pre-calculate the velocity.
    float remainSpeed = speed * deltaTime;
    if (getPlayScene()->slowCheatEnabled) {
        remainSpeed *= 0.5f;  // Slow down the enemy movement
    }
    while (remainSpeed != 0) {
        if (path.empty()) {
            // Reach end point.
            Hit(hp);
            getPlayScene()->Hit();
            reachEndTime = 0;
            return;
        }
        Engine::Point target = path.back() * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
        Engine::Point vec = target - Position;
        // Add up the distances:
        // 1. to path.back()
        // 2. path.back() to border
        // 3. All intermediate block size
        // 4. to end point
        reachEndTime = (vec.Magnitude() + (path.size() - 1) * PlayScene::BlockSize - remainSpeed) / speed;
        Engine::Point normalized = vec.Normalize();
        if (remainSpeed - vec.Magnitude() > 0) {
            Position = target;
            path.pop_back();
            remainSpeed -= vec.Magnitude();
        } else {
            Velocity = normalized * remainSpeed / deltaTime;
            remainSpeed = 0;
        }
    }
    Rotation = atan2(Velocity.y, Velocity.x);
    Sprite::Update(deltaTime);
    hpLabel->Position = Position + Engine::Point(0, -10);
}
void Enemy::Draw() const {
    PlayScene* scene = getPlayScene();
    ALLEGRO_BITMAP* bitmap = bmp.get();
    
    if (flashTimer > 0) {
        // When hit, show red flash with a slight blue tint if slow motion is active
        ALLEGRO_COLOR tint;
        if (scene && scene->slowCheatEnabled) {
            // Mix red flash with blue tint (more red, less blue)
            tint = al_map_rgba(255, 50, 50, 180);  // Red with slight blue influence
        } else {
            tint = al_map_rgba(255, 0, 0, 180);  // Pure red flash
        }
        al_draw_tinted_scaled_rotated_bitmap_region(
            bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap),
            tint,
            Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
            Position.x, Position.y, Size.x / GetBitmapWidth(), Size.y / GetBitmapHeight(),
            Rotation, 0
        );
    } else if (scene && scene->slowCheatEnabled) {
        // Normal blue tint for slow motion
        al_draw_tinted_scaled_rotated_bitmap_region(
            bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap),
            al_map_rgba(100, 200, 255, 200),  // Light blue tint with some transparency
            Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
            Position.x, Position.y, Size.x / GetBitmapWidth(), Size.y / GetBitmapHeight(),
            Rotation, 0
        );
    } else {
        Sprite::Draw();
    }
    
    if (hasBeenDamaged) {
        // Draw health bar background (red)
        const float barWidth = 40.0f;
        const float barHeight = 4.0f;
        const float barY = Position.y - 20.0f;  // Position above the enemy
        const float barX = Position.x - barWidth / 2.0f;  // Center the bar
        
        // Draw background (red)
        al_draw_filled_rectangle(barX, barY, barX + barWidth, barY + barHeight, al_map_rgba(255, 0, 0, 180));
        
        // Draw foreground (green) based on current HP percentage
        float hpPercentage = hp / maxHp;
        al_draw_filled_rectangle(barX, barY, barX + barWidth * hpPercentage, barY + barHeight, al_map_rgba(0, 255, 0, 180));
    }
    hpLabel->Draw();
    if (PlayScene::DebugMode) {
        // Draw collision radius.
        al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255, 0, 0), 2);
    }
}
