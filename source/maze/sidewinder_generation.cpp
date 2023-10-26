#include "maze_generation.hpp"

#include <random>
#include <util/util.hpp>
#include <util/random_utils.hpp>

using Node = Maze::Node;

Maze generate_sidewinder(size_t width, size_t height, float group_prob) {
    Maze maze(width, height, MazeObject::wall);

    Node from(0, 0);
    maze.from = util::coords_to_idx(from.x, from.y, width);
    maze.get_cell(from) = MazeObject::start;

    Node to { width - 1 , height - 1 };
    to.x -= to.x % 2;
    to.y -= to.y % 2;
    maze.to = util::coords_to_idx(to.x, to.y, width);
    maze.get_cell(to) = MazeObject::finish;

    auto& rengine = get_rengine();

    for (size_t h = 0; h < height; h += 2) {
        size_t group_start = 0;
        for (size_t w = 0; w < width; w += 2) {
            if (w == width - 1 && h == height - 1) {
                continue;
            }
            const bool add_to_group = [&] {
                if (w == width - 1) {
                    return false;  
                } else if (h == height - 1) {
                    return true;
                } else {
                    return chance(group_prob);
                }
            }();
            if (add_to_group) {
                maze.get_cell({w + 1, h}) = MazeObject::space;
            } else {
                const size_t break_offset = std::uniform_int_distribution<size_t>(
                    0, 
                    (w - group_start) / 2
                )(rengine);
                const size_t break_point = group_start + break_offset * 2;
                maze.get_cell({break_point, h + 1}) = MazeObject::space;
                group_start = w + 2;
            }
            auto& cur = maze.get_cell({w, h});
            if (cur == MazeObject::wall) {
                cur = MazeObject::space;
            }
        }

        const size_t break_offset = std::uniform_int_distribution<size_t>(
            0,
            (width + (width % 2) - group_start) / 2
        )(rengine);
        const size_t break_point = group_start + break_offset * 2;
        maze.get_cell({break_point, h + 1}) = MazeObject::space;
    }

    return maze;
}

