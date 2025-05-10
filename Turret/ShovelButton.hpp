#ifndef SHOVELBUTTON_HPP
#define SHOVELBUTTON_HPP
#include <string>

#include "Engine/Sprite.hpp"
#include "UI/Component/ImageButton.hpp"

class PlayScene;

class ShovelButton : public Engine::ImageButton {
protected:
    PlayScene *getPlayScene();

public:
    ShovelButton(std::string img, std::string imgIn, float x, float y);
    void Update(float deltaTime) override;
    void Draw() const override;
};
#endif   // SHOVELBUTTON_HPP 