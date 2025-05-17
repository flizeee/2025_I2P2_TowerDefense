#include <functional>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "WinScene.hpp"
#include "ScoreboardScene.hpp"

void WinScene::Initialize() {
    ticks = 0;
    playerName = "";
    CalculateScore();

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    AddNewObject(new Engine::Image("win/benjamin-sad.png", halfW, h / 5 * 3, 300, 300, 0.5, 0.5));
    AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 48, halfW, halfH / 4 - 10, 255, 255, 255, 255, 0.5, 0.5));
    
    // Add score display
    AddNewObject(new Engine::Label("Score: " + std::to_string(score), "pirulen.ttf", 36, halfW, halfH / 4 + 50, 255, 255, 255, 255, 0.5, 0.5));
    
    // Add name input prompt
    AddNewObject(new Engine::Label("Enter your name:", "pirulen.ttf", 36, halfW, halfH / 2, 255, 255, 255, 255, 0.5, 0.5));
    
    // Add border for name input
    const int borderWidth = 300;
    const int borderHeight = 60;
    AddNewObject(new Engine::Image("win/dirt.png", halfW, halfH / 2 + 50, borderWidth, borderHeight, 0.5, 0.5));
    
    nameInputLabel = new Engine::Label(playerName + "_", "pirulen.ttf", 36, halfW, halfH / 2 + 50, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(nameInputLabel);

    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    bgmId = AudioHelper::PlayAudio("win.wav");
}

void WinScene::Terminate() {
    IScene::Terminate();
    AudioHelper::StopBGM(bgmId);
}

void WinScene::Update(float deltaTime) {
    ticks += deltaTime;
    if (ticks > 4 && ticks < 100 &&
        dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
        ticks = 100;
        bgmId = AudioHelper::PlayBGM("happy.ogg");
    }
}

void WinScene::OnKeyDown(int keyCode) {
    if (keyCode == ALLEGRO_KEY_BACKSPACE && !playerName.empty()) {
        playerName.pop_back();
        nameInputLabel->Text = playerName + "_";
    }
    else if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z && playerName.length() < 10) {
        playerName += static_cast<char>('A' + (keyCode - ALLEGRO_KEY_A));
        nameInputLabel->Text = playerName + "_";
    }
}

void WinScene::CalculateScore() {
    PlayScene* playScene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"));
    if (playScene) {
        int remainingLife = playScene->lives;
        int remainingMoney = playScene->GetMoney();
        float usedTime = playScene->ticks;
        score = (100 * remainingLife) + (1 * remainingMoney) - (2 * static_cast<int>(usedTime));
    }
}

void WinScene::BackOnClick(int stage) {
    if (playerName.empty()) return;

    ScoreboardScene* scoreboard = dynamic_cast<ScoreboardScene*>(Engine::GameEngine::GetInstance().GetScene("scoreboard"));
    if (scoreboard) {
        scoreboard->AddScore(playerName, score);
    }

    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
