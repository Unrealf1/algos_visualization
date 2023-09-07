#pragma once

#include <vector>
#include <array>
#include <cstdint>
#include <cstdlib>


enum class MazeObject : uint8_t {
    space, wall, start, finish, slow
};

struct Maze {
    struct Node { 
        size_t x;
        size_t y;

        auto operator<=>(const Node&) const = default;
    };

    const size_t width;
    const size_t height;
    std::vector<MazeObject> items;
    
    static Maze generate_simple(size_t height, size_t width, double wall_prob);
    static std::pair<Maze::Node, Maze::Node> add_start_finish(Maze&);
    void add_slow_tiles(double change_probability);
    MazeObject& get_cell(const Node& node);
    std::vector<Node> get_neighboors(const Node& node);
};

