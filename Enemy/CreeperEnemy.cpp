#include <cmath>
#include <string>
#include "CreeperEnemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include "Turret/Turret.hpp"
#include "UI/Animation/Plane.hpp"
#include <memory>

CreeperEnemy::CreeperEnemy(int x, int y)
    : Enemy("play/enemy-4.png", x, y, 20, 40, 80, 40) {
    // Speed: 40 (moderate speed)
    // HP: 80 (moderate HP)
    // Money reward: 40 (moderate reward)
    // Collision radius: 20 (standard)
    shockwave = std::shared_ptr<ALLEGRO_BITMAP>(al_load_bitmap("play/shockwave.png"), al_destroy_bitmap);
}

void CreeperEnemy::OnExplode() {
    // Call base class explosion effect
    Enemy::OnExplode();
    
    // Get the play scene
    auto playScene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!playScene) return;

    // Create shockwave effect
    auto shockwaveEffect = new Engine::Sprite("play/shockwave.png", Position.x, Position.y);
    shockwaveEffect->Size = Engine::Point(PlayScene::BlockSize * 2, PlayScene::BlockSize * 2);
    shockwaveEffect->CollisionRadius = PlayScene::BlockSize * 2;
    playScene->EffectGroup->AddNewObject(shockwaveEffect);

    // Calculate explosion radius in pixels (5 blocks)
    const float explosionRadius = PlayScene::BlockSize * 5;

    // Check all turrets in the scene
    for (auto& it : playScene->TowerGroup->GetObjects()) {
        Turret* turret = dynamic_cast<Turret*>(it);
        if (!turret) continue;

        // Calculate distance between creeper and turret
        float distance = (turret->Position - Position).Magnitude();
        
        // If turret is within explosion radius, remove it
        if (distance <= explosionRadius) {
            playScene->TowerGroup->RemoveObject(turret->GetObjectIterator());
        }
    }
} 