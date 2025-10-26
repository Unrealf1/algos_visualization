#include "maze_generation.hpp"

#include <algos/DFS.hpp>
#include <util/util.hpp>
#include <util/random_utils.hpp>
#include <algorithm>

#include <spdlog/spdlog.h>
#include <sstream>

namespace rng = std::ranges;

using Node = Maze::Node;

Maze generate_random_dfs(size_t width, size_t height, float random_wall_break_prob) {
    Maze maze(width, height);

    Node from(0, 0);
    maze.from = util::coords_to_idx(from.x, from.y, width);

    Node to { width / 2, height / 2 };
    if (to.x % 2 != 0) {
        --to.x;
    }
    if (to.y % 2 != 0) {
        --to.y;
    }
    maze.to = util::coords_to_idx(to.x, to.y, width);

    maze.get_cell(from) = MazeObject::start;
    maze.get_cell(to) = MazeObject::finish;

    auto searcher = [&to](const Maze::Node& node) {
        return node == to;
    };

    auto edge_getter = [&](const Maze::Node& node) {
        auto neighboors = maze.get_cross_neighboors(node, 2);
        std::shuffle(neighboors.begin(), neighboors.end(), get_rengine());
        return neighboors;
    };

    auto reconstructor = [&](const Maze::Node&, const std::vector<algos::ReconstructionItem<Maze::Node>>& parents) -> algos::NodePath<Node> {
        for (auto& obj : maze.items) {
            if (obj == MazeObject::finish || obj == MazeObject::start) {
                continue;
            }
            obj = MazeObject::wall;
        }
        for (const auto& item : parents) {
            const auto& cur = item.child;
            const auto& par = parents[item.parent_index].child;
            if (maze.get_cell(cur) == MazeObject::wall) {
                maze.get_cell(cur) = MazeObject::space;
            }
            Maze::Node middle (
                (cur.x + par.x) / 2,
                (cur.y + par.y) / 2
            );
            if (maze.get_cell(middle) == MazeObject::wall) {
                maze.get_cell(middle) = MazeObject::space;
            }
        }
        return {};
    };

    algos::DFSFindPath<Maze::Node>(from, searcher, edge_getter, reconstructor);

    if (random_wall_break_prob > 0.0f) {
      for (size_t x = 0; x < width - 2; x += 2) {
        for (size_t y = 0; y < height - 2; y += 2) {
          auto cur = maze.get_cell({x, y});
          auto& right1 = maze.get_cell({x + 1, y});
          auto right2 = maze.get_cell({x + 2, y});
          if (Maze::is_walkable(cur)
              && !Maze::is_walkable(right1)
              && Maze::is_walkable(right2)
              && chance(random_wall_break_prob)) {
            right1 = MazeObject::space;
          }
          auto& bot1 = maze.get_cell({x, y + 1});
          auto bot2 = maze.get_cell({x, y + 2});
          if (Maze::is_walkable(cur)
              && !Maze::is_walkable(bot1)
              && Maze::is_walkable(bot2)
              && chance(random_wall_break_prob)) {
            bot1 = MazeObject::space;
          }
        }
      }
    }

    return maze;
}
