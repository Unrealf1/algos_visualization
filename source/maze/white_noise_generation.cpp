#include "maze_generation.hpp"

#include <util/random_utils.hpp>
#include <algorithm>

using Node = Maze::Node;
namespace rng = std::ranges;


Maze generate_white_noise(size_t width, size_t height, double wall_prob) {
    Maze maze(width, height);
    auto& rengine = get_rengine();
    std::uniform_real_distribution<double> distr(0, 1); // 0 - wall, others - empty
    rng::generate(maze.items, [&]{ return distr(rengine) < wall_prob 
            ? MazeObject::wall 
            : MazeObject::space; });

    // increases chances for generation with existing path
    for (const auto& node : { Maze::Node{0, 1}, { 1, 0 }, {1, 1} }) {
        maze.get_cell(node) = MazeObject::space;
    }

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
