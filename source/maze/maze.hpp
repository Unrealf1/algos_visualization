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

    size_t width;
    size_t height;
    size_t from;
    size_t to;
    std::vector<MazeObject> items;
    
    static Maze load(const std::filesystem::path&);
    static void add_random_start_finish(Maze&);
    void add_slow_tiles(double change_probability);
    void resize(size_t new_width, size_t new_height);

    void save(const std::filesystem::path&) const;
    MazeObject& get_cell(const Node& node);
    std::vector<Node> get_neighboors(const Node& node) const;
    std::vector<Node> get_cross_neighboors(const Node& node, size_t distance = 1) const;
    std::vector<Node> get_sides_and_corners(const Node& node, bool corners_require_adjacent, size_t distance = 1) const;

    bool is_valid(const Node& node) const;
};

