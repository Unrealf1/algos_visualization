#include "maze_generation.hpp"

#include <random>
#include <util/util.hpp>
#include <util/random_utils.hpp>

using Node = Maze::Node;

Maze generate_binary_tree(size_t width, size_t height, float horizontal_prob) {
    Maze maze(width, height, MazeObject::wall);

    Node from(0, 0);
    maze.from = util::coords_to_idx(from.x, from.y, width);
    maze.get_cell(from) = MazeObject::start;

    Node to { width - 1 , height - 1 };
    to.x -= to.x % 2;
    to.y -= to.y % 2;
    maze.to = util::coords_to_idx(to.x, to.y, width);
    maze.get_cell(to) = MazeObject::finish;

    for (size_t w = 0; w < width; w += 2) {
        for (size_t h = 0; h < height; h += 2) {
            if (w == width - 1 && h == height - 1) {
                continue;
            }
            const bool go_right = [&] {
                if (w == width - 1) {
                    return false;  
                } else if (h == height - 1) {
                    return true;
                } else {
                    return chance(horizontal_prob);
                }
            }();
            if (go_right) {
                maze.get_cell({w + 1, h}) = MazeObject::space;
            } else {
                maze.get_cell({w, h + 1}) = MazeObject::space;
            }
            auto& cur = maze.get_cell({w, h});
            if (cur == MazeObject::wall) {
                cur = MazeObject::space;
            }
        }
    }

    return maze;
}
