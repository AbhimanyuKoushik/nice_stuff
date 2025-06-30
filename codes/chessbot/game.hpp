#ifndef GAME_HPP
#define GAME_HPP

#include "position.hpp"
#include "types.hpp"
#include "movedef.hpp"
#include <string>

class Game {
public:
    Position currposition;
    Color BotColor;
    bool GameEnded;
    int Winner;  // -1 = draw, 0 = White wins, 1 = Black wins, -2 = undefined
    Moves moves_played;

    Game() : BotColor(Black), GameEnded(false), Winner(-2) {}

    Move findbestmove(const Position &position);
    bool isDrawByInsufficientMaterial(const Position &position);
    bool isGameEnded(const Position &position);
    int getWinner(const Position &position);
    void Startplaying(Color YourColor, bool bot_vs_bot = false);
};

#endif // GAME_HPP
