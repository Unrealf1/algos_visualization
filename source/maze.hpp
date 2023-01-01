#pragma once

#include <vector>
#include <array>
#include <cstdint>
#include <cstdlib>


enum class MazeObject : uint8_t {
    space, wall
};

struct Maze {
    struct Node { 
        size_t x;
        size_t y;

        auto operator<=>(const Node&) const = default;
    };

    const size_t width;
    const size_t height;
    const double wall_prob;
    std::vector<MazeObject> maze;

    Maze(size_t height, size_t width, double wall_prob = 0.2);
    MazeObject& get_cell(const Node& node);
    std::vector<Node> get_neighboors(const Node& node);
};
