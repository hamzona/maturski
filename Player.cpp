#include "Player.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include "Map.h"

Player::Player(int number, std::string name)
    : number(number), name(name)
{
    switch (number)
    {
    case 0:
        startingCoo = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
        path = {{4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, {11, 4}, {12, 4}, {13, 4}, {14, 4}, {15, 4}, {16, 4}, {16, 5}, {16, 6}, {16, 7}, {16, 8}, {16, 9}, {16, 10}, {16, 11}, {16, 12}, {16, 13}, {16, 14}, {16, 15}, {16, 16}, {15, 16}, {14, 16}, {13, 16}, {12, 16}, {11, 16}, {10, 16}, {9, 16}, {8, 16}, {7, 16}, {6, 16}, {5, 16}, {4, 16}, {4, 15}, {4, 14}, {4, 13}, {4, 12}, {4, 11}, {4, 10}, {5, 10}, {6, 10}, {7, 10}, {8, 10}};
        break;
    case 1:
        startingCoo = {{19, 0}, {19, 1}, {18, 0}, {18, 1}};
        path = {{16, 4}, {16, 5}, {16, 6}, {16, 7}, {16, 8}, {16, 9}, {16, 10}, {16, 11}, {16, 12}, {16, 13}, {16, 14}, {16, 15}, {16, 16}, {15, 16}, {14, 16}, {13, 16}, {12, 16}, {11, 16}, {10, 16}, {9, 16}, {8, 16}, {7, 16}, {6, 16}, {5, 16}, {4, 16}, {4, 15}, {4, 14}, {4, 13}, {4, 12}, {4, 11}, {4, 10}, {4, 9}, {4, 8}, {4, 7}, {4, 6}, {4, 5}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, {10, 5}, {10, 6}, {10, 7}, {10, 8}};
        break;
    case 2:
        startingCoo = {{19, 19}, {18, 19}, {19, 18}, {18, 18}};
        path = {{16, 16}, {15, 16}, {14, 16}, {13, 16}, {12, 16}, {11, 16}, {10, 16}, {9, 16}, {8, 16}, {7, 16}, {6, 16}, {5, 16}, {4, 16}, {4, 15}, {4, 14}, {4, 13}, {4, 12}, {4, 11}, {4, 10}, {4, 9}, {4, 8}, {4, 7}, {4, 6}, {4, 5}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, {11, 4}, {12, 4}, {13, 4}, {14, 4}, {15, 4}, {16, 4}, {16, 5}, {16, 6}, {16, 7}, {16, 8}, {16, 9}, {16, 10}, {15, 10}, {14, 10}, {13, 10}, {12, 10}};
        break;
    case 3:
        startingCoo = {{0, 19}, {0, 18}, {1, 19}, {1, 18}};
        path = {{4, 16}, {4, 15}, {4, 14}, {4, 13}, {4, 12}, {4, 11}, {4, 10}, {4, 9}, {4, 8}, {4, 7}, {4, 6}, {4, 5}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, {11, 4}, {12, 4}, {13, 4}, {14, 4}, {15, 4}, {16, 4}, {16, 5}, {16, 6}, {16, 7}, {16, 8}, {16, 9}, {16, 10}, {16, 11}, {16, 12}, {16, 13}, {16, 14}, {16, 15}, {16, 16}, {15, 16}, {14, 16}, {13, 16}, {12, 16}, {11, 16}, {10, 16}, {10, 15}, {10, 14}, {10, 13}, {10, 12}};
        break;
    }
    start = std::find(mapPath.begin(), mapPath.end(), std::make_pair(4, 4));
    if (start == mapPath.end())
        std::cerr << "Start position {4, 4} not found in mapPath!" << std::endl;

    pawnPositions = startingCoo;
}

Player::~Player() {}

std::vector<std::pair<int, int>> Player::getPawnPositions() const
{
    return pawnPositions;
}

bool Player::handlePawnClick(const sf::Vector2f &mousePos, float fieldSize)
{
    for (size_t i = 0; i < pawnPositions.size(); ++i)
    {
        sf::Vector2f pawnPixelPos = {pawnPositions[i].first * fieldSize, pawnPositions[i].second * fieldSize};
        sf::FloatRect pawnBounds(pawnPixelPos.x, pawnPixelPos.y, fieldSize, fieldSize);
        if (pawnBounds.contains(mousePos))
        {
            selectedPawnIndex = static_cast<int>(i);
            return true;
        }
    }
    return false;
}

int Player::getSelectedPawnIndex() const
{
    return selectedPawnIndex;
}

bool Player::isPawnHome()
{
    if (selectedPawnIndex >= 0 && selectedPawnIndex < static_cast<int>(pawnPositions.size()))
    {
        std::pair<int, int> currentPos = pawnPositions[selectedPawnIndex];
        return std::find(startingCoo.begin(), startingCoo.end(), currentPos) != startingCoo.end();
    }
    return false;
}

void Player::startMoving(int steps)
{
    stepsRemaining = steps;
    isMoving = true;
}

void Player::updateMovement()
{
    if (!isMoving || stepsRemaining == 0)
        return;
    if (isPawnHome())
    {
        pawnPositions[selectedPawnIndex] = path[0];
    }
    else
    {
        std::pair<int, int> current = pawnPositions[selectedPawnIndex];
        auto it = std::find(path.begin(), path.end(), current);
        if (it != path.end())
        {
            auto nextIt = it + 1;

            if (nextIt == path.end())
            {
                stepsRemaining = 0;
            }
            else
            {
                pawnPositions[selectedPawnIndex] = *nextIt;
            }
        }
    }
    if (stepsRemaining != 0)
        stepsRemaining--;
}

bool Player::getIsMoving() const
{
    return isMoving;
}

std::string Player::getName()
{
    return name;
}

bool Player::isWin()
{
    std::vector<std::pair<int, int>> winningPath;
    for (int i = 0; i < 4; i++)
        winningPath.push_back(path.at(path.size() - 1 - i));
    for (const auto &pawnPos : pawnPositions)
    {
        if (std::find(winningPath.begin(), winningPath.end(), pawnPos) == winningPath.end())
            return false;
    }
    return true;
}

void Player::eatingPawn(std::pair<int, int> location)
{
    for (int i = 0; i < pawnPositions.size(); ++i)
    {
        if (pawnPositions[i].first == location.first && pawnPositions[i].second == location.second)
        {
            returnPawnHome(i);
            break;
        }
    }
}

void Player::returnPawnHome(int pawnIndex)
{
    for (const auto &homeCoord : startingCoo)
    {
        bool occupied = false;
        for (size_t i = 0; i < pawnPositions.size(); ++i)
        {
            if (i == pawnIndex)
                continue;
            if (pawnPositions[i].first == homeCoord.first && pawnPositions[i].second == homeCoord.second)
            {
                occupied = true;
                break;
            }
        }
        if (!occupied)
        {
            pawnPositions[pawnIndex] = homeCoord;
            return;
        }
    }
}

std::pair<int, int> Player::getSelectedPawnPosition()
{
    if (selectedPawnIndex < 0 || selectedPawnIndex >= pawnPositions.size())
        return std::make_pair(-1, -1);
    return pawnPositions[selectedPawnIndex];
}

std::vector<std::vector<std::pair<int, int>>> Player::pawnsOnTheSameField()
{
    std::vector<std::vector<std::pair<int, int>>> result;
    std::vector<std::pair<int, int>> positions;
    for (const auto &pos : pawnPositions)
    {
        if (std::find(positions.begin(), positions.end(), pos) == positions.end())
            positions.push_back(pos);
    }
    for (const auto &uniquePos : positions)
    {
        std::vector<std::pair<int, int>> sameFieldPawns;
        for (const auto &pawn : pawnPositions)
        {
            if (pawn == uniquePos)
                sameFieldPawns.push_back(pawn);
        }
        result.push_back(sameFieldPawns);
    }
    return result;
}
