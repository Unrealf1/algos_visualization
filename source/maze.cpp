#include "util.hpp"
#include <maze.hpp>
#include <random_utils.hpp>
#include <ranges>
#include <algorithm>
#include <iterator>
#include <fstream>

namespace rng = std::ranges;


Maze::Maze(size_t width, size_t height, MazeObject default_tile)
    : width(width)
    , height(height)
    , from(0)
    , to(0)
    , items(width * height, default_tile) { }

Maze Maze::generate_simple(size_t width, size_t height, double wall_prob) {
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

void Maze::add_random_start_finish(Maze& maze) {
    Maze::Node from = {0, 0};
    Maze::Node to{};
    auto& rengine = get_rengine();
    to.x = std::uniform_int_distribution<size_t>(0, maze.width - 1)(rengine);
    to.y = std::uniform_int_distribution<size_t>(0, maze.height - 1)(rengine);
    maze.from = util::coords_to_idx(from.x, from.y, maze.width);
    maze.to = util::coords_to_idx(to.x, to.y, maze.width);
    maze.items[maze.from] = MazeObject::start;
    maze.items[maze.to] = MazeObject::finish;
}

void Maze::add_slow_tiles(double change_probability) {
    for (auto& cell : items) {
        if (cell == MazeObject::space && chance(change_probability)) {
            cell = MazeObject::slow;
        }
    }
}

Maze Maze::load(const std::filesystem::path& path) {
    std::fstream file(path, std::ios::in);
    size_t width;
    size_t height;
    file >> width >> height;
    Maze maze(width, height);

    using raw_t = std::underlying_type_t<MazeObject>;
    rng::transform(std::istream_iterator<raw_t>(file), std::istream_iterator<raw_t>(), maze.items.begin(), [](raw_t v) {
        return static_cast<MazeObject>(v);
    });
    auto start_it = rng::find(maze.items, MazeObject::start);
    auto finish_it = rng::find(maze.items, MazeObject::finish);
    // order of iterators in std::distance is relevant!
    maze.from = static_cast<size_t>(std::distance(maze.items.begin(), start_it));
    maze.to = static_cast<size_t>(std::distance(maze.items.begin(), finish_it));
    return maze;
}

void Maze::save(const std::filesystem::path& path) {
    std::ofstream file(path, std::ios::out);
    file << width << ' ' << height << ' ';
    using raw_t = std::underlying_type_t<MazeObject>;
    rng::transform(items, std::ostream_iterator<raw_t>(file), [](MazeObject v) {
        return static_cast<raw_t>(v);
    });
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
