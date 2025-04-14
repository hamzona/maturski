#include "Game.h"
#include "Map.h"
#include "data.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

Game::Game() : window(sf::VideoMode(1200, 800), "test"), currentPlayerIndex(0)
{
    window.setFramerateLimit(60);
    tile.setSize(sf::Vector2f(fieldSize, fieldSize));
    tile.setFillColor(sf::Color::White);
    tile.setOutlineThickness(-1);
    tile.setOutlineColor(sf::Color::Black);
    pawnCircle.setRadius(fieldSize / 2 - 2);
    pawnCircle.setFillColor(sf::Color::Red);
    pawnCircle.setOutlineThickness(-1);
    pawnCircle.setOutlineColor(sf::Color::Black);
    numberOfPlayers = 4;

    for (int i = 0; i < numberOfPlayers; i++)
    {
        players.emplace_back(i, "Player " + std::to_string(i + 1));
    }
    changeState(STATE_START_GAME);
}

Game::~Game() {}

void Game::changeState(GameState newState)
{
    if (newState == STATE_QUESTION)
    {
        if (state == STATE_MOVE)
            currentPlayerIndex = (currentPlayerIndex == numberOfPlayers - 1) ? 0 : currentPlayerIndex + 1;
        else
            currentPlayerIndex = 0;
        rightAnswers = 0;
        questionNumber = 0;
        generateRandomIndexOfQuestion();
    }
    state = newState;
}

void Game::run()
{
    while (window.isOpen())
    {
        processEvents();
        update();
        render();
    }
}

void Game::update()
{
    if (currentPlayerIndex == -1)
        return;
    Player &currentPlayer = players[currentPlayerIndex];
    if (currentPlayer.getIsMoving())
    {
        if (currentPlayer.stepsRemaining == 0)
        {
            handleLastPawnStep(currentPlayer.getSelectedPawnPosition());
            currentPlayer.isMoving = false;
            if (state != STATE_WIN)
            {
                changeState(STATE_QUESTION);
            }
        }
        else if (moveClock.getElapsedTime().asMilliseconds() > 400)
        {
            currentPlayer.updateMovement();
            moveClock.restart();
        }
    }
}

void Game::render()
{
    window.clear();
    drawBoard();
    drawHomes();
    drawFinishZones();
    drawPawns();
    switch (state)
    {
    case STATE_SETUP:
        setup();
        break;
    case STATE_QUESTION:
        popup();
        break;
    case STATE_PLAY_AGAIN:
        play_again_popup();
        break;
    case STATE_START_GAME:
        start_game_popup();
        break;
    case STATE_WIN:
        win_popup();
        break;
    default:
        break;
    }
    window.display();
}

void Game::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();

        if (state == STATE_SETUP)
        {
            if (event.type == sf::Event::TextEntered)
            {
                if (event.text.unicode >= '2' && event.text.unicode <= '4')
                {
                    numberOfPlayers = int(static_cast<char>(event.text.unicode) - '0');
                    players.clear();
                    for (int i = 0; i < numberOfPlayers; i++)
                        players.emplace_back(i, "Player " + std::to_string(i + 1));
                }
            }
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f mousePos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
            if (state == STATE_SETUP)
            {
                float setupX = 650.f;
                float inputBoxY = 50.f + 24.f + 20.f;            // prompt (at y = 50) with character size 24 + margin
                float continueButtonY = inputBoxY + 50.f + 20.f; // inputBox height (50) plus margin (20)
                sf::FloatRect continueBounds(setupX + 50.f, continueButtonY, 200.f, 50.f);
                if (continueBounds.contains(mousePos))
                {
                    changeState(STATE_QUESTION);
                }
            }
            else if (state == STATE_WIN)
            {
                handleWinClick(mousePos);
            }
            else if (state == STATE_QUESTION)
                handleAnswerClick(mousePos);
            else if (state == STATE_MOVE)
                handlePawnClick(mousePos);
            else if (state == STATE_START_GAME)
            {
                sf::RectangleShape startButton(sf::Vector2f(150.f, 50.f));
                startButton.setPosition((window.getSize().x - 600.f) / 2.f + 50.f, (window.getSize().y - 400.f) / 2.f + 300.f);
                if (startButton.getGlobalBounds().contains(mousePos))
                    changeState(STATE_SETUP);
            }
            else if (state == STATE_PLAY_AGAIN)
            {
                sf::RectangleShape yesButton(sf::Vector2f(150.f, 50.f));
                yesButton.setPosition((window.getSize().x - 600.f) / 2.f + 50.f, (window.getSize().y - 400.f) / 2.f + 300.f);
                if (yesButton.getGlobalBounds().contains(mousePos))
                    changeState(STATE_QUESTION);
            }
        }
    }
}

void Game::drawBoard()
{
    for (auto &pos : mapPath)
    {
        tile.setPosition(pos.first * fieldSize, pos.second * fieldSize);
        window.draw(tile);
    }
}

void Game::drawHomes()
{
    for (auto &home : homes)
        for (auto &pos : home)
        {
            tile.setPosition(pos.first * fieldSize, pos.second * fieldSize);
            window.draw(tile);
        }
}

void Game::drawFinishZones()
{
    for (auto &zone : finish)
        for (auto &pos : zone)
        {
            tile.setPosition(pos.first * fieldSize, pos.second * fieldSize);
            window.draw(tile);
        }
}

void Game::drawPawns()
{
    for (int i = 0; i < players.size(); ++i)
    {
        pawnCircle.setFillColor(playerColors[i]);
        std::vector<std::vector<std::pair<int, int>>> groups = players[i].pawnsOnTheSameField();
        for (const auto &group : groups)
        {
            switch (group.size())
            {
            case 1:
                pawnCircle.setRadius(fieldSize / 2.5);
                pawnCircle.setPosition(group[0].first * fieldSize + fieldSize / 4, group[0].second * fieldSize + fieldSize / 4);
                window.draw(pawnCircle);
                break;
            case 2:
                for (int j = 0; j < 2; j++)
                {
                    pawnCircle.setRadius(fieldSize / 3);
                    float offset = (j == 0) ? 0.f : fieldSize / 2;
                    pawnCircle.setPosition(group[j].first * fieldSize + offset, group[j].second * fieldSize + offset);
                    window.draw(pawnCircle);
                }
                break;
            case 3:
                for (int j = 0; j < 3; j++)
                {
                    pawnCircle.setRadius(fieldSize / 3.2);
                    float x = group[j].first * fieldSize;
                    float y = group[j].second * fieldSize;
                    if (j == 0)
                        pawnCircle.setPosition(x + fieldSize / 2 - fieldSize / 6, y);
                    else if (j == 1)
                        pawnCircle.setPosition(x, y + fieldSize / 2);
                    else if (j == 2)
                        pawnCircle.setPosition(x + fieldSize / 2, y + fieldSize / 2);
                    window.draw(pawnCircle);
                }
                break;
            case 4:
                for (int j = 0; j < 4; j++)
                {
                    pawnCircle.setRadius(fieldSize / 3.5);
                    float x = group[j].first * fieldSize;
                    float y = group[j].second * fieldSize;
                    float offsetX = (j % 2 == 0) ? 0.f : fieldSize / 2;
                    float offsetY = (j < 2) ? 0.f : fieldSize / 2;
                    pawnCircle.setPosition(x + offsetX, y + offsetY);
                    window.draw(pawnCircle);
                }
                break;
            default:
                break;
            }
        }
    }
}

void Game::generateRandomIndexOfQuestion()
{
    static bool seeded = false;
    if (!seeded)
    {
        srand(static_cast<unsigned>(time(nullptr)));
        seeded = true;
    }
    if (usedQuestionIndex.size() == data.size())
        usedQuestionIndex.clear();
    int r;
    do
    {
        r = rand() % data.size();
    } while (std::find(usedQuestionIndex.begin(), usedQuestionIndex.end(), r) != usedQuestionIndex.end());
    usedQuestionIndex.push_back(r);
    indexOfQuestion = r;
}

void Game::setup()
{
    sf::Font font;
    font.loadFromFile("Arial.ttf");
    float setupX = 650.f;
    float setupWidth = window.getSize().x - setupX;
    sf::RectangleShape setupBackground(sf::Vector2f(setupWidth, window.getSize().y));
    setupBackground.setFillColor(sf::Color::White);
    setupBackground.setPosition(setupX, 0.f);
    window.draw(setupBackground);
    sf::Text prompt;
    prompt.setFont(font);
    prompt.setCharacterSize(24);
    prompt.setFillColor(sf::Color::Black);
    prompt.setString("Input the number of players (2, 3, or 4):");
    prompt.setPosition(setupX + 50.f, 50.f);
    window.draw(prompt);
    sf::RectangleShape inputBox(sf::Vector2f(300.f, 50.f));
    inputBox.setFillColor(sf::Color(255, 255, 255, 230));
    inputBox.setOutlineColor(sf::Color::Black);
    inputBox.setOutlineThickness(3.f);
    inputBox.setPosition(setupX + 50.f, prompt.getPosition().y + prompt.getLocalBounds().height + 20.f);
    window.draw(inputBox);
    sf::Text inputText;
    inputText.setFont(font);
    inputText.setCharacterSize(24);
    inputText.setFillColor(sf::Color::Black);
    inputText.setString(std::to_string(numberOfPlayers));
    inputText.setPosition(inputBox.getPosition().x + 10.f, inputBox.getPosition().y + 10.f);
    window.draw(inputText);
    sf::RectangleShape continueButton(sf::Vector2f(200.f, 50.f));
    continueButton.setFillColor(sf::Color(200, 200, 200));
    continueButton.setOutlineColor(sf::Color::Black);
    continueButton.setOutlineThickness(3.f);
    continueButton.setPosition(setupX + 50.f, inputBox.getPosition().y + inputBox.getSize().y + 20.f);
    window.draw(continueButton);
    sf::Text continueText;
    continueText.setFont(font);
    continueText.setCharacterSize(24);
    continueText.setFillColor(sf::Color::Black);
    continueText.setString("Continue");
    sf::FloatRect contBounds = continueText.getLocalBounds();
    continueText.setPosition(continueButton.getPosition().x + continueButton.getSize().x / 2.f - contBounds.width / 2.f,
                             continueButton.getPosition().y + continueButton.getSize().y / 2.f - contBounds.height / 2.f - contBounds.top);
    window.draw(continueText);
}

void Game::popup()
{
    sf::RectangleShape popup(sf::Vector2f(600.f, windowSize));
    popup.setFillColor(sf::Color::White);
    popup.setOutlineThickness(2.f);
    popup.setOutlineColor(sf::Color::Black);
    popup.setPosition(windowSize + 30.f, 0);
    static sf::Font font;
    static bool fontLoaded = false;
    if (!fontLoaded)
    {
        font.loadFromFile("Arial.ttf");
        fontLoaded = true;
    }
    sf::Text player_name(players[currentPlayerIndex].getName(), font, 30);
    player_name.setFillColor(sf::Color::Black);
    player_name.setPosition(popup.getPosition().x + 20.f, popup.getPosition().y + 10.f);
    sf::Text questionText(data.at(indexOfQuestion).question, font, 20);
    questionText.setFillColor(sf::Color::Black);
    questionText.setPosition(popup.getPosition().x + 20.f, popup.getPosition().y + 60.f);
    if (answers.empty())
    {
        for (int i = 0; i < 4; i++)
        {
            sf::RectangleShape rect(sf::Vector2f(360.f, 50.f));
            rect.setFillColor(sf::Color(230, 230, 230));
            rect.setOutlineColor(sf::Color::Black);
            rect.setOutlineThickness(1.f);
            rect.setPosition(popup.getPosition().x + 20.f, popup.getPosition().y + 60.f + i * 60.f + 40.f);
            sf::Text answerText(data.at(indexOfQuestion).answers[i], font, 18);
            answerText.setFillColor(sf::Color::Black);
            answerText.setPosition(rect.getPosition().x + 10.f, rect.getPosition().y + 10.f);
            answers.push_back(std::make_pair(rect, answerText));
        }
    }
    window.draw(popup);
    window.draw(questionText);
    window.draw(player_name);
    for (auto &answer : answers)
        window.draw(answer.first), window.draw(answer.second);
}

void Game::handleAnswerClick(const sf::Vector2f &mousePos)
{
    for (size_t i = 0; i < answers.size(); ++i)
    {
        if (answers[i].first.getGlobalBounds().contains(mousePos))
        {
            bool correct = (i == data.at(indexOfQuestion).rightAnswer);
            if (correct)
            {
                rightAnswers++;
                answers[i].first.setFillColor(sf::Color::Green);
            }
            else
            {
                answers[i].first.setFillColor(sf::Color::Red);
                answers[data.at(indexOfQuestion).rightAnswer].first.setFillColor(sf::Color::Green);
            }
            window.clear();
            drawBoard();
            drawHomes();
            drawFinishZones();
            drawPawns();
            popup();
            window.display();
            sf::sleep(sf::seconds(1.f));
            if (questionNumber == 5)
                changeState(STATE_MOVE);
            else
            {
                questionNumber++;
                generateRandomIndexOfQuestion();
            }
            answers.clear();
        }
    }
}

void Game::handleWinClick(const sf::Vector2f &mousePos)
{

    // Define win popup button bounds matching the ones in win_popup():
    sf::FloatRect playAgainBounds(
        (window.getSize().x - 600.f) / 2.f + 50.f,
        (window.getSize().y - 400.f) / 2.f + 400.f - 100.f,
        150.f, 50.f);
    sf::FloatRect quitBounds(
        (window.getSize().x - 600.f) / 2.f + 600.f - 200.f,
        (window.getSize().y - 400.f) / 2.f + 400.f - 100.f,
        150.f, 50.f);

    if (playAgainBounds.contains(mousePos))
    {
        std::cout << "Play Again clicked" << std::endl;
        // Transition to a new game state (for example, reset the game)
        changeState(STATE_START_GAME);
    }
    else if (quitBounds.contains(mousePos))
    {
        std::cout << "Quit clicked" << std::endl;
        window.close();
    }
}

void Game::handlePawnClick(const sf::Vector2f &mousePos)
{
    std::cout << currentPlayerIndex << std::endl;
    Player &currentPlayer = players[currentPlayerIndex];
    if (currentPlayer.getIsMoving())
        return;
    std::cout << currentPlayer.getName() << std::endl;
    if (currentPlayer.handlePawnClick(mousePos, fieldSize))
    {
        currentPlayer.startMoving(4);
        moveClock.restart();
    }
}

void Game::handleLastPawnStep(std::pair<int, int> location)
{
    if (players[currentPlayerIndex].isWin())
    {
        changeState(STATE_WIN);
        std::cout << "Win" << std::endl;
    }
    else
    {
        for (int i = 0; i < numberOfPlayers; i++)
        {
            if (i != currentPlayerIndex)
                players[i].eatingPawn(location);
        }
    }
}

void Game::play_again_popup()
{
    sf::RectangleShape popup(sf::Vector2f(600.f, 400.f));
    popup.setFillColor(sf::Color::White);
    popup.setOutlineThickness(2.f);
    popup.setOutlineColor(sf::Color::Black);
    popup.setPosition((window.getSize().x - popup.getSize().x) / 2.f,
                      (window.getSize().y - popup.getSize().y) / 2.f);
    static sf::Font font;
    static bool fontLoaded = false;
    if (!fontLoaded)
    {
        font.loadFromFile("Arial.ttf");
        fontLoaded = true;
    }
    sf::Text title("Play Again?", font, 30);
    title.setFillColor(sf::Color::Black);
    title.setPosition(popup.getPosition().x + 20.f, popup.getPosition().y + 20.f);
    sf::RectangleShape yesButton(sf::Vector2f(150.f, 50.f));
    yesButton.setFillColor(sf::Color(200, 200, 200));
    yesButton.setOutlineColor(sf::Color::Black);
    yesButton.setOutlineThickness(1.f);
    yesButton.setPosition(popup.getPosition().x + 50.f, popup.getPosition().y + popup.getSize().y - 100.f);
    sf::Text yesText("Yes", font, 20);
    yesText.setFillColor(sf::Color::Black);
    yesText.setPosition(yesButton.getPosition().x + 40.f, yesButton.getPosition().y + 10.f);
    sf::RectangleShape noButton(sf::Vector2f(150.f, 50.f));
    noButton.setFillColor(sf::Color(200, 200, 200));
    noButton.setOutlineColor(sf::Color::Black);
    noButton.setOutlineThickness(1.f);
    noButton.setPosition(popup.getPosition().x + popup.getSize().x - 200.f, popup.getPosition().y + popup.getSize().y - 100.f);
    sf::Text noText("No", font, 20);
    noText.setFillColor(sf::Color::Black);
    noText.setPosition(noButton.getPosition().x + 50.f, noButton.getPosition().y + 10.f);
    window.draw(popup);
    window.draw(title);
    window.draw(yesButton);
    window.draw(noButton);
    window.draw(yesText);
    window.draw(noText);
}

void Game::start_game_popup()
{
    sf::RectangleShape popup(sf::Vector2f(600.f, 400.f));
    popup.setFillColor(sf::Color::White);
    popup.setOutlineThickness(2.f);
    popup.setOutlineColor(sf::Color::Black);
    popup.setPosition((window.getSize().x - popup.getSize().x) / 2.f,
                      (window.getSize().y - popup.getSize().y) / 2.f);
    static sf::Font font;
    static bool fontLoaded = false;
    if (!fontLoaded)
    {
        font.loadFromFile("Arial.ttf");
        fontLoaded = true;
    }
    sf::Text title("Start Game", font, 30);
    title.setFillColor(sf::Color::Black);
    title.setPosition(popup.getPosition().x + 20.f, popup.getPosition().y + 20.f);
    sf::RectangleShape startButton(sf::Vector2f(150.f, 50.f));
    startButton.setFillColor(sf::Color(200, 200, 200));
    startButton.setOutlineColor(sf::Color::Black);
    startButton.setOutlineThickness(1.f);
    startButton.setPosition(popup.getPosition().x + 50.f, popup.getPosition().y + popup.getSize().y - 100.f);
    sf::Text startText("Start", font, 20);
    startText.setFillColor(sf::Color::Black);
    startText.setPosition(startButton.getPosition().x + 35.f, startButton.getPosition().y + 10.f);
    sf::RectangleShape quitButton(sf::Vector2f(150.f, 50.f));
    quitButton.setFillColor(sf::Color(200, 200, 200));
    quitButton.setOutlineColor(sf::Color::Black);
    quitButton.setOutlineThickness(1.f);
    quitButton.setPosition(popup.getPosition().x + popup.getSize().x - 200.f, popup.getPosition().y + popup.getSize().y - 100.f);
    sf::Text quitText("Quit", font, 20);
    quitText.setFillColor(sf::Color::Black);
    quitText.setPosition(quitButton.getPosition().x + 45.f, quitButton.getPosition().y + 10.f);
    window.draw(popup);
    window.draw(title);
    window.draw(startButton);
    window.draw(quitButton);
    window.draw(startText);
    window.draw(quitText);
}

void Game::win_popup()
{
    // Load font (in a real application consider caching this resource)
    sf::Font font;
    if (!font.loadFromFile("Arial.ttf"))
    {
        // Handle error if necessary
    }

    // Create a popup background rectangle
    sf::RectangleShape popup(sf::Vector2f(600.f, 400.f));
    popup.setFillColor(sf::Color::White);
    popup.setOutlineThickness(2.f);
    popup.setOutlineColor(sf::Color::Black);
    popup.setPosition((window.getSize().x - popup.getSize().x) / 2.f,
                      (window.getSize().y - popup.getSize().y) / 2.f);

    // Title text at the top of the popup
    sf::Text title("Congratulations!", font, 30);
    title.setFillColor(sf::Color::Black);
    title.setPosition(popup.getPosition().x + 20.f, popup.getPosition().y + 20.f);

    // Win message
    sf::Text message("Player " + std::to_string(currentPlayerIndex + 1) + " win", font, 24);
    message.setFillColor(sf::Color::Black);
    message.setPosition(popup.getPosition().x + 20.f, popup.getPosition().y + 70.f);

    // Create the "Play Again" button
    sf::RectangleShape playAgainButton(sf::Vector2f(150.f, 50.f));
    playAgainButton.setFillColor(sf::Color(200, 200, 200));
    playAgainButton.setOutlineColor(sf::Color::Black);
    playAgainButton.setOutlineThickness(1.f);
    playAgainButton.setPosition(popup.getPosition().x + 50.f, popup.getPosition().y + popup.getSize().y - 100.f);

    sf::Text playAgainText("Play Again", font, 20);
    playAgainText.setFillColor(sf::Color::Black);
    sf::FloatRect paBounds = playAgainText.getLocalBounds();
    playAgainText.setPosition(
        playAgainButton.getPosition().x + playAgainButton.getSize().x / 2.f - paBounds.width / 2.f,
        playAgainButton.getPosition().y + playAgainButton.getSize().y / 2.f - paBounds.height / 2.f - paBounds.top);

    // Create the "Quit" button
    sf::RectangleShape quitButton(sf::Vector2f(150.f, 50.f));
    quitButton.setFillColor(sf::Color(200, 200, 200));
    quitButton.setOutlineColor(sf::Color::Black);
    quitButton.setOutlineThickness(1.f);
    quitButton.setPosition(popup.getPosition().x + popup.getSize().x - 200.f, popup.getPosition().y + popup.getSize().y - 100.f);

    sf::Text quitText("Quit", font, 20);
    quitText.setFillColor(sf::Color::Black);
    sf::FloatRect qBounds = quitText.getLocalBounds();
    quitText.setPosition(
        quitButton.getPosition().x + quitButton.getSize().x / 2.f - qBounds.width / 2.f,
        quitButton.getPosition().y + quitButton.getSize().y / 2.f - qBounds.height / 2.f - qBounds.top);

    // Draw all the popup elements
    window.draw(popup);
    window.draw(title);
    window.draw(message);
    window.draw(playAgainButton);
    window.draw(playAgainText);
    window.draw(quitButton);
    window.draw(quitText);

    // Note: This function only draws the popup.
    // You must handle input events (e.g., clicks on the playAgainButton and quitButton)
    // in your main event loop. For example, you can store the bounds of these buttons in member variables
    // and check mouse clicks against them.
}
