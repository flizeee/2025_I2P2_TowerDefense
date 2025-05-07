#include "PlaneEnemy.hpp"
#include "Scene/PlayScene.hpp"

PlaneEnemy::PlaneEnemy(int x, int y) : Enemy("play/enemy-2.png", x, y, 20, 100, 50, 20) {
    // Plane enemy is faster but has less HP and gives less money
    // Speed: 100 (faster than SoldierEnemy)
    // HP: 50 (less than SoldierEnemy)
    // Money reward: 20 (less than SoldierEnemy)
    // Collision radius: 20 (same as SoldierEnemy)
}

void PlaneEnemy::Update(float deltaTime) {
    // Use the base class's Update method for movement
    Enemy::Update(deltaTime);
} 