#include "util.hpp"
#include <iterator>
#include <maze.hpp>
#include <random_utils.hpp>

#include <ranges>
#include <algorithm>

namespace rng = std::ranges;

Maze Maze::generate_simple(size_t width, size_t height, double wall_prob) {
    Maze maze = {
        .width = width,
        .height = height,
        .items = {}
    };
    maze.items.resize(width * height);
    auto& rengine = get_rengine();
    std::uniform_real_distribution<double> distr(0, 1); // 0 - wall, others - empty
    rng::generate(maze.items, [&]{ return distr(rengine) < wall_prob 
            ? MazeObject::wall 
            : MazeObject::space; });

    // clean up empty spaces/walls surrounded by others for cleaner picture
    for (size_t h = 1; h < height - 1; ++h) {
        for (size_t w = 1; w < width - 1; ++w) {
            std::array nodes_to_check = {
                Node{w + 1, h},
                Node{w, h + 1},
                Node{w - 1, h},
                Node{w, h - 1}
            };
            if (rng::all_of(nodes_to_check, [&](const auto& node) { return maze.get_cell(node) != maze.get_cell({w, h}); })) {
                maze.get_cell({w, h}) = maze.get_cell({w + 1, h});
            }
        }
    }
    return maze;
}

std::pair<Maze::Node, Maze::Node> Maze::add_start_finish(Maze& maze) {
    Maze::Node from = {0, 0};
    Maze::Node to{};
    auto& rengine = get_rengine();
    to.x = std::uniform_int_distribution<size_t>(0, maze.width - 1)(rengine);
    to.y = std::uniform_int_distribution<size_t>(0, maze.height - 1)(rengine);
    maze.get_cell(to) = MazeObject::finish;
    maze.get_cell(from) = MazeObject::start;
    return {from, to};
}

MazeObject& Maze::get_cell(const Node& node) {
    auto idx = util::coords_to_idx(node.x, node.y, width);
    return items[idx];
}

std::vector<Maze::Node> Maze::get_neighboors(const Node& node) {
    auto [x, y] = node;
    std::array nodes_to_check = {
        Node{x + 1, y},
        Node{x, y + 1},
        Node{x - 1, y},
        Node{x, y - 1}
    };
    std::vector<Node> res;
    res.reserve(nodes_to_check.size());

    rng::copy_if(nodes_to_check, std::back_inserter(res), [&](const Node& node) {
        bool valid = node.x < width && node.y < height;
        if (valid) {
            auto index = util::coords_to_idx(node.x, node.y, width);
            return items[index] != MazeObject::wall;
        }
        return false;
    });

    return res;
}
