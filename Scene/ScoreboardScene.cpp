#include <fstream>
#include <algorithm>
#include <functional>
#include <sstream>
#include <iomanip>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/LOG.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "ScoreboardScene.hpp"

void ScoreboardScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    // Add title
    AddNewObject(new Engine::Label("Scoreboard", "pirulen.ttf", 48, halfW, halfH / 4, 255, 255, 255, 255, 0.5, 0.5));
    pageLabel = new Engine::Label("Page 0 of 0", "pirulen.ttf", 32, halfW, halfH / 4 + 50, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(pageLabel);

    // Add column headers
    AddNewObject(new Engine::Label("Rank", "pirulen.ttf", 32, halfW - 300, halfH / 2, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Name", "pirulen.ttf", 32, halfW - 100, halfH / 2, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Score", "pirulen.ttf", 32, halfW + 100, halfH / 2, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Date", "pirulen.ttf", 32, halfW + 300, halfH / 2, 255, 255, 255, 255, 0.5, 0.5));

    // Create score labels
    scoreLabels = new Engine::Label*[entriesPerPage * 4];
    for (int i = 0; i < entriesPerPage; i++) {
        int y = halfH / 2 + 50 + i * 40;
        Engine::Label* rankLabel = new Engine::Label("", "pirulen.ttf", 24, halfW - 300, y, 255, 255, 255, 255, 0.5, 0.5);
        Engine::Label* nameLabel = new Engine::Label("", "pirulen.ttf", 24, halfW - 100, y, 255, 255, 255, 255, 0.5, 0.5);
        Engine::Label* scoreLabel = new Engine::Label("", "pirulen.ttf", 24, halfW + 100, y, 255, 255, 255, 255, 0.5, 0.5);
        Engine::Label* dateLabel = new Engine::Label("", "pirulen.ttf", 24, halfW + 300, y, 255, 255, 255, 255, 0.5, 0.5);
        
        scoreLabels[i * 4] = rankLabel;
        scoreLabels[i * 4 + 1] = nameLabel;
        scoreLabels[i * 4 + 2] = scoreLabel;
        scoreLabels[i * 4 + 3] = dateLabel;
        
        AddNewObject(rankLabel);
        AddNewObject(nameLabel);
        AddNewObject(scoreLabel);
        AddNewObject(dateLabel);
    }

    // Add navigation buttons
    Engine::ImageButton* btn;
    
    // Back button
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 150, h * 0.85, 300, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, h * 0.85 + 50, 0, 0, 0, 255, 0.5, 0.5));

    // Prev Page button
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 700, h * 0.85, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::PrevPageOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Prev Page", "pirulen.ttf", 32, halfW - 500, h * 0.85 + 50, 0, 0, 0, 255, 0.5, 0.5));

    // Next Page button
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW + 300, h * 0.85, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::NextPageOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Next Page", "pirulen.ttf", 32, halfW + 500, h * 0.85 + 50, 0, 0, 0, 255, 0.5, 0.5));

    // Load scores from file
    if (!scoreLoaded) {
        LoadScores();
    }
    currentPage = 0;
    UpdateScoreLabels();
}

void ScoreboardScene::Terminate() {
    IScene::Terminate();
}

void ScoreboardScene::Update(float deltaTime) {
    IScene::Update(deltaTime);
}

void ScoreboardScene::Draw() const {
    IScene::Draw();
}

void ScoreboardScene::UpdateScoreLabels() {
    int startIdx = currentPage * entriesPerPage;
    for (int i = 0; i < entriesPerPage; i++) {
        int labelIdx = i * 4;
        if (startIdx + i < scores.size()) {
            const ScoreEntry& entry = scores[startIdx + i];
            int rank = startIdx + i + 1;
            
                std::stringstream ss;
                ss << std::put_time(std::localtime(&entry.createdTime), "%Y-%m-%d %H:%M:%S");

            scoreLabels[labelIdx]->Text = std::to_string(rank);
            scoreLabels[labelIdx + 1]->Text = entry.name;
            scoreLabels[labelIdx + 2]->Text = std::to_string(entry.score);
            scoreLabels[labelIdx + 3]->Text = ss.str();
        } else {
            scoreLabels[labelIdx]->Text = "";
            scoreLabels[labelIdx + 1]->Text = "";
            scoreLabels[labelIdx + 2]->Text = "";
            scoreLabels[labelIdx + 3]->Text = "";
        }
    }
    pageLabel->Text = "Page " + std::to_string(currentPage + 1) + " of " + std::to_string(scores.size() / entriesPerPage);
}

void ScoreboardScene::LoadScores() {
    Engine::LOG(Engine::INFO) << "LoadScores()";
    std::ifstream file("scores.txt");
    if (!file.is_open()) return;

    scores.clear();
    std::string name;
    int score;
    time_t time;
    while (file >> name >> score >> time) {
        scores.push_back({name, score, time});
    }
    file.close();
    SortScores();
    scoreLoaded = true;
}

void ScoreboardScene::SaveScores() {
    std::ofstream file("scores.txt");
    if (!file.is_open()) return;

    for (const auto& entry : scores) {
        file << entry.name << " " << entry.score << " " << entry.createdTime << "\n";
    }
    file.close();
}

void ScoreboardScene::SortScores() {
    std::sort(scores.begin(), scores.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        if (a.score != b.score) return a.score > b.score;
        return a.createdTime < b.createdTime;
    });
}

void ScoreboardScene::AddScore(const std::string& name, int score) {
    scores.push_back({name, score, std::time(nullptr)});
    SortScores();
    SaveScores();
    // UpdateScoreLabels();
}

void ScoreboardScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

void ScoreboardScene::PrevPageOnClick() {
    if (currentPage > 0) {
        currentPage--;
        UpdateScoreLabels();
    }
}

void ScoreboardScene::NextPageOnClick() {
    if ((currentPage + 1) * entriesPerPage < scores.size()) {
        currentPage++;
        UpdateScoreLabels();
    }
} 