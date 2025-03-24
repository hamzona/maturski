#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <utility>

class Player
{
private:
    std::vector<std::pair<int, int>> startingCoo;
    std::string name;
    std::vector<std::pair<int, int>> pawnPositions;
    std::vector<std::pair<int, int>> path;
    std::vector<std::pair<int, int>>::const_iterator start;
    bool isPawnHome();
    void returnPawnHome(int index);

public:
    int number;
    int selectedPawnIndex = -1;
    bool isMoving = false;
    int stepsRemaining = 0;
    Player(int number, std::string name);
    ~Player();
    std::vector<std::pair<int, int>> getPawnPositions() const;
    bool handlePawnClick(const sf::Vector2f &mousePos, float fieldSize);
    int getSelectedPawnIndex() const;
    std::pair<int, int> getSelectedPawnPosition();
    void startMoving(int steps);
    void updateMovement();
    bool getIsMoving() const;
    bool isWin();
    std::string getName();
    void eatingPawn(std::pair<int, int> location);
    std::vector<std::vector<std::pair<int, int>>> pawnsOnTheSameField();
};
