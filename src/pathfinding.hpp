#pragma once

#include <algorithm>
#include <math.h>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <bits/stdc++.h>
#include <string>

template <typename T, typename priority_t>
struct PriorityQueue
{
    typedef std::pair<priority_t, T> PQElement;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> elements;

    inline bool empty() const
    {
        return elements.empty();
    }

    inline void put(T item, priority_t priority)
    {
        elements.emplace(priority, item);
    }

    T getAndDelete()
    {
        T lowest_value = elements.top().second;
        elements.pop();
        return lowest_value;
    }
};

struct GridLocation
{
    int x, y;
};

bool operator==(GridLocation a, GridLocation b);

bool operator!=(GridLocation a, GridLocation b);

bool operator<(GridLocation a, GridLocation b);

std::basic_iostream<char>::basic_ostream &operator<<(std::basic_iostream<char>::basic_ostream &out, const GridLocation &loc);

namespace std
{
    /* implement hash function so we can put GridLocation into an unordered_set */
    template <>
    struct hash<GridLocation>
    {
        std::size_t operator()(const GridLocation &id) const noexcept
        {
            // NOTE: better to use something like boost hash_combine
            return std::hash<int>()(id.x ^ (id.y << 16));
        }
    };

    std::string to_string(GridLocation location);
}

struct SquareGrid
{
    static std::array<GridLocation, 8> DIRS;

    int width, height;
    std::unordered_set<GridLocation> walls;

    SquareGrid(int width_, int height_)
        : width(width_), height(height_) {}

    bool in_bounds(GridLocation id) const
    {
        return 0 <= id.x && id.x < width && 0 <= id.y && id.y < height;
    }

    bool traversable(GridLocation id) const
    {
        return walls.find(id) == walls.end();
    }

    std::vector<GridLocation> neighbors(GridLocation id) const
    {
        std::vector<GridLocation> results;

        for (GridLocation dir : DIRS)
        {
            GridLocation next{id.x + dir.x, id.y + dir.y};
            if (in_bounds(next) && traversable(next))
            {
                if (traversable({id.x + dir.x, id.y}) && traversable({id.x, id.y + dir.y}))
                {
                    results.push_back(next);
                }
            }
        }

        return results;
    }
};

struct GridWithWeights : SquareGrid
{
    std::unordered_map<GridLocation, float> weights;
    GridWithWeights(int w, int h) : SquareGrid(w, h) {}

    void clear()
    {
        weights.clear();
        walls.clear();
        width = 0;
        height = 0;
    }

    void setCost(GridLocation node, float cost)
    {
        weights.insert_or_assign(node, cost);
    }

    double cost(GridLocation from_node, GridLocation to_node) const
    {
        // may only be called on nodes accessible via movements in DIRS
        bool isDiagonal = std::abs(from_node.x - to_node.x) + std::abs(from_node.y - to_node.y) == 2;
        int weight;
        if (weights.find(to_node) == weights.end())
        {
            weight = 1;
        }
        else
        {
            weight = weights.at(to_node);
        }
        return (isDiagonal ? 1.4 * weight : weight);
    }
};

inline double heuristic(GridLocation a, GridLocation b)
{
    int dX = std::abs(a.x - b.x);
    int dY = std::abs(a.y - b.y);
    //return dX+dY;
    if (dX > dY)
    {
        return 0.4 * dY + dX;
    }
    else
    {
        return 0.4 * dX + dY ;
    }
}

/* a star requires: double Graph::cost(Location, Location), from neighbouring nodes
                    std::vector<Location> Graph::neighbours(Location)
                    heuristic(Location, Location)
                    */

template <typename Location, typename Graph>
void a_star_search(Graph graph,
                   Location start,
                   Location goal,
                   std::unordered_map<Location, Location> &came_from,
                   std::unordered_map<Location, double> &cost_so_far)
{
    PriorityQueue<Location, double> frontier;
    frontier.put(start, 0);

    came_from[start] = start;
    cost_so_far[start] = 0;

    while (!frontier.empty())
    {
        Location current = frontier.getAndDelete();

        if (current == goal)
        {
            break;
        }

        for (Location next : graph.neighbors(current))
        {
            double new_cost = cost_so_far[current] + graph.cost(current, next);
            if (cost_so_far.find(next) == cost_so_far.end() || new_cost < cost_so_far[next])
            {
                // if the current path is more efficient or there is no score, update the score for that Location
                cost_so_far[next] = new_cost;
                double priority = new_cost + heuristic(next, goal);
                frontier.put(next, priority);
                came_from[next] = current;
            }
        }
    }
}

void addInvalidQueryToGameErrors();

template <typename Location>
std::vector<Location> getPathToLocation(std::unordered_map<Location, Location> trace, Location location)
{
    Location currentCheck = location;
    std::vector<Location> checked = {};

    if (trace.find(location) == trace.end()) {
        addInvalidQueryToGameErrors();
        return checked;
    }

    for (int i = 0; i < 10000; i++)
    {
        if (trace.find(currentCheck) != trace.end() && std::find(checked.begin(), checked.end(), currentCheck) == checked.end())
        {
            checked.push_back(currentCheck);
            currentCheck = trace[currentCheck];
        }
        else
        {
            break;
        }
    }
    std::reverse(checked.begin(), checked.end());
    return checked;
}

template <class Graph>
void draw_grid(const Graph &graph,
               std::unordered_map<GridLocation, double> *distances = nullptr,
               std::unordered_map<GridLocation, GridLocation> *point_to = nullptr,
               std::vector<GridLocation> *path = nullptr,
               GridLocation *start = nullptr,
               GridLocation *goal = nullptr)
{
    const int field_width = 3;
    std::cout << std::string(field_width * graph.width, '_') << '\n';
    for (int y = 0; y != graph.height; ++y)
    {
        for (int x = 0; x != graph.width; ++x)
        {
            GridLocation id{x, y};
            if (graph.walls.find(id) != graph.walls.end())
            {
                std::cout << std::string(field_width, '#');
            }
            else if (start && id == *start)
            {
                std::cout << " A ";
            }
            else if (goal && id == *goal)
            {
                std::cout << " Z ";
            }
            else if (path != nullptr && find(path->begin(), path->end(), id) != path->end())
            {
                std::cout << " @ ";
            }
            else if (point_to != nullptr && point_to->count(id))
            {
                GridLocation next = (*point_to)[id];
                if (next == GridLocation{x + 1, y + 1})
                {
                    std::cout << " \\ ";
                }
                else if (next == GridLocation{x - 1, y - 1})
                {
                    std::cout << " \\ ";
                }
                else if (next == GridLocation{x + 1, y - 1})
                {
                    std::cout << " / ";
                }
                else if (next == GridLocation{x - 1, y + 1})
                {
                    std::cout << " / ";
                }
                else if (next.x == x + 1) {
                    std::cout << " > ";
                }
                else if (next.x == x - 1)
                {
                    std::cout << " < ";
                }
                else if (next.y == y + 1)
                {
                    std::cout << " v ";
                }
                else if (next.y == y - 1)
                {
                    std::cout << " ^ ";
                }
                else
                {
                    std::cout << " * ";
                }
            }
            else if (distances != nullptr && distances->count(id))
            {
                std::cout << ' ' << std::left << std::setw(field_width - 1) << (*distances)[id];
            }
            else
            {
                std::cout << " . ";
            }
        }
        std::cout << '\n';
    }
    std::cout << std::string(field_width * graph.width, '~') << '\n';
}