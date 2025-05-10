#ifndef SCOREBOARDSCENE_HPP
#define SCOREBOARDSCENE_HPP

#include <vector>
#include <string>
#include <ctime>
#include "Engine/IScene.hpp"
#include "UI/Component/Label.hpp"

struct ScoreEntry {
    std::string name;
    int score;
    time_t createdTime;
};

class ScoreboardScene final : public Engine::IScene {
private:
    std::vector<ScoreEntry> scores;
    int currentPage;
    const int entriesPerPage = 10;
    bool scoreLoaded;
    Engine::Label** scoreLabels;
    Engine::Label* pageLabel;
    void LoadScores();
    void SaveScores();
    void SortScores();
    void UpdateScoreLabels();

public:
    explicit ScoreboardScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void BackOnClick(int stage);
    void PrevPageOnClick();
    void NextPageOnClick();
    void AddScore(const std::string& name, int score);
};

#endif // SCOREBOARDSCENE_HPP 