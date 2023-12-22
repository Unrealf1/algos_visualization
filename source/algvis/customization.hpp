#include "parameters.hpp"
#include <maze/maze.hpp>


using edge_getter_t = std::vector<Maze::Node> (*) (const Maze&, const Maze::Node&);

edge_getter_t create_edge_getter(const ApplicationParams& params) {
    if (!params.allow_diagonals) {
        return [](const Maze& maze, const Maze::Node& node) {
            return maze.get_cross_neighboors(node);
        };
    }
    if (params.require_adjacent_for_diagonals) {
        return [](const Maze& maze, const Maze::Node& node) {
            return maze.get_sides_and_corners(node, true);
        };
    }
    return [](const Maze& maze, const Maze::Node& node) {
        return maze.get_sides_and_corners(node, false);
    };
}

