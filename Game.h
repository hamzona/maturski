#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

enum GameState
{
    STATE_QUESTION,
    STATE_MOVE,
    STATE_PLAY_AGAIN,
    STATE_START_GAME
};

class Game
{
private:
    sf::Clock moveClock;
    sf::RenderWindow window;
    sf::RectangleShape tile;
    sf::CircleShape pawnCircle;
    std::vector<int> usedQuestionIndex;
    const int windowSize = 600;
    const int gridSize = 20;
    const int fieldSize = windowSize / gridSize;
    int currentPlayerIndex;
    GameState state;
    int indexOfQuestion = 0;
    int rightAnswers;
    int questionNumber;
    std::vector<Player> players;
    sf::Color playerColors[4] = {sf::Color::Red, sf::Color::Blue, sf::Color::Green, sf::Color::Yellow};
    std::vector<std::pair<sf::RectangleShape, sf::Text>> answers;
    void render();
    void processEvents();
    void update();
    void drawBoard();
    void drawHomes();
    void drawFinishZones();
    void drawPawns();
    void popup();
    void handleAnswerClick(const sf::Vector2f &mousePos);
    void generateRandomIndexOfQuestion();
    void handlePawnClick(const sf::Vector2f &mousePos);
    void handleLastPawnStep(std::pair<int, int> location);
    void changeState(GameState newState);
    void play_again_popup();
    void start_game_popup();

public:
    Game();
    ~Game();
    void run();
};
