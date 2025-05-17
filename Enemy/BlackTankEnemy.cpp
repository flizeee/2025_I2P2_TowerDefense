#include <string>
#include "BlackTankEnemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/SoldierEnemy.hpp"

BlackTankEnemy::BlackTankEnemy(int x, int y)
    : Enemy("play/enemy-7.png", x, y, 20, 20, 200, 75) {
} 

void BlackTankEnemy::OnExplode() {
    Enemy::OnExplode();
    auto playScene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (playScene) {
        for (int i = 0; i < 2; i++) {
            auto enemy = new SoldierEnemy(Position.x, Position.y);
            playScene->EnemyGroup->AddNewObject(enemy);
            enemy->UpdatePath(playScene->mapDistance);
            enemy->Update(i * 0.5f);
        }
    }
}