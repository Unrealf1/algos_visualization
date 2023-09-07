#pragma once

#include <vector>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <filesystem>


enum class MazeObject : uint8_t {
    space, wall, start, finish, slow
};

struct Maze {
    struct Node { 
        size_t x;
        size_t y;

        Node() = default;
        Node(size_t x, size_t y) : x(x), y(y) {}
        explicit Node(const auto& bindable) {
            auto [xx, yy] = bindable;
            x = xx;
            y = yy;
        }

        auto operator<=>(const Node&) const = default;
    };

    Maze(size_t width, size_t height, MazeObject default_tile = MazeObject::space);

    const size_t width;
    const size_t height;
    size_t from;
    size_t to;
    std::vector<MazeObject> items;
    
    static Maze generate_simple(size_t height, size_t width, double wall_prob);
    static Maze load(const std::filesystem::path&);
    void save(const std::filesystem::path&);
    static void add_random_start_finish(Maze&);
    void add_slow_tiles(double change_probability);
    MazeObject& get_cell(const Node& node);
    std::vector<Node> get_neighboors(const Node& node);
};

