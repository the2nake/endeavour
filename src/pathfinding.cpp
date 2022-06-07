#include "pathfinding.hpp"
#include "Game.hpp"

bool operator==(GridLocation a, GridLocation b)
{
    return a.x == b.x && a.y == b.y;
}

bool operator!=(GridLocation a, GridLocation b)
{
    return !(a == b);
}

bool operator<(GridLocation a, GridLocation b)
{
    return std::tie(a.x, a.y) < std::tie(b.x, b.y);
}

std::basic_iostream<char>::basic_ostream &operator<<(std::basic_iostream<char>::basic_ostream &out, const GridLocation &loc)
{
    out << '(' << loc.x << ',' << loc.y << ')';
    return out;
}

std::string std::to_string(GridLocation loc) {
    return (std::string)("(") + std::to_string(loc.x) + ", " + std::to_string(loc.y) + ")";
}

std::array<GridLocation, 8> SquareGrid::DIRS = {
    /* East, West, North, South */
    GridLocation{1, 0}, GridLocation{-1, 0},
    GridLocation{0, -1}, GridLocation{0, 1},
    /* SE, NE, SW, NW */
    GridLocation{1, 1}, GridLocation{1, -1},
    GridLocation{-1, 1}, GridLocation{-1, -1}
};

void addInvalidQueryToErrors() {
    Game::add_error("Invalid query: location is not accessible.");
}