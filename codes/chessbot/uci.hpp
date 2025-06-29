#ifndef UCI_HPP
#define UCI_HPP

#include <iostream>
#include "movedef.hpp"
#include "position.hpp"

Move parse_move(const std::string& move_str, Position position);

#endif