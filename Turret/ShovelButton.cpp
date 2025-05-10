#include <allegro5/color.h>

#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include "Scene/PlayScene.hpp"
#include "ShovelButton.hpp"

PlayScene *ShovelButton::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

ShovelButton::ShovelButton(std::string img, std::string imgIn, float x, float y) : ImageButton(img, imgIn, x, y) {
}

void ShovelButton::Update(float deltaTime) {
    ImageButton::Update(deltaTime);
    // Shovel is always enabled
    Enabled = true;
}

void ShovelButton::Draw() const {
    ImageButton::Draw();
} 