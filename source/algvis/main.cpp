#include <spdlog/spdlog.h>

#include "algos/BFS.hpp"
#include "algos/DFS.hpp"
#include "algos/dijkstra.hpp"
#include "algos/a_star.hpp"
#include "visual/grid.hpp"

#include <stdexcept>
#include <visual/allegro_util.hpp>
#include <maze/maze.hpp>
#include "parameters.hpp"
#include <util/random_utils.hpp>
#include <thread>
#include <chrono>

namespace rng = std::ranges;


Maze create_maze(const ApplicationParams& params) {
    if (!params.load_file.empty()) {
        return Maze::load(params.load_file);
    }

    switch (params.generation_algorithm) {
        case ApplicationParams::EGenerationAlgorithm::noise: {
            const auto wall_probability = 0.4;
            Maze maze = Maze::generate_simple(params.maze_width, params.maze_height, wall_probability);
            Maze::add_random_start_finish(maze);
            maze.add_slow_tiles(params.slow_tile_chance);
            return maze;
        }
        case ApplicationParams::EGenerationAlgorithm::random_dfs: {
            return Maze::generate_random_dfs(params.maze_width, params.maze_height);
        }
    }
    throw std::logic_error("Unknown maze generation algorithm!");
}

int main() {
    auto startup_time = std::chrono::steady_clock::now();
    auto params = get_cached_application_params("config.json");
    spdlog::set_level(params.debug_level);
    set_random_seed(params.fixed_seed);

    Maze maze = create_maze(params);
    Maze::Node from {util::idx_to_coords(maze.from, maze.width)};
    Maze::Node to {util::idx_to_coords(maze.to, maze.width)};
    spdlog::info("searching path from {}, {} to {}, {}", from.x, from.y, to.x, to.y);
    if (!params.save_file.empty()) {
        maze.save(params.save_file);
    }

    std::vector<Maze::Node> search_log;
    std::vector<std::pair<Maze::Node, size_t>> discover_log;
    auto logging_edge_getter = [&](const Maze::Node& node) {
        auto neighboors = maze.get_neighboors(node);
        rng::transform(neighboors, std::back_inserter(discover_log), [&](const Maze::Node& n) {
            return std::pair{n, search_log.size()};
        });
        return neighboors;
    };
    auto random_logging_edge_getter = [&](const Maze::Node& node) {
        auto neighboors = logging_edge_getter(node);
        auto& rengine = get_rengine();
        std::shuffle(neighboors.begin(), neighboors.end(), rengine);
        return neighboors;
    };
    auto logging_searcher = [&](const Maze::Node& node) {
        search_log.push_back(node);
        return node == to;
    };
    auto weight_getter = [&](const Maze::Node&, const Maze::Node& to) {
        return maze.get_cell(to) == MazeObject::slow ? params.slow_tile_cost : 1.0;
    };

    clock_t start = clock();
    auto path = [&] {
        using namespace algos;
        switch (params.algorithm) {
            case ApplicationParams::EAlgorithm::BFS: {
                return BFSFindPath<Maze::Node>(from, logging_searcher, logging_edge_getter);
            }
            case ApplicationParams::EAlgorithm::DFS: {
                return DFSFindPath<Maze::Node>(from, logging_searcher, logging_edge_getter);
            }
            case ApplicationParams::EAlgorithm::RandomDFS: {
                return DFSFindPath<Maze::Node>(from, logging_searcher, random_logging_edge_getter);
            }
            case ApplicationParams::EAlgorithm::Dijkstra: {
                return DijkstraFindPath(from, logging_searcher, logging_edge_getter, weight_getter);
            }
            case ApplicationParams::EAlgorithm::AStar: {
                return AStarFindPath(from, logging_searcher, logging_edge_getter, weight_getter, [&](const Maze::Node& node) {
                    auto dx = node.x - to.x;
                    auto dy = node.y - to.y;
                    return std::sqrt(dx * dx + dy * dy);
                });
            }
        }
        // should not be reachable. Kept here for now because of gcc warning(end of non-void finction)
        throw std::logic_error("Unknown algorithm!");
    }();
    clock_t end = clock();
    spdlog::info("Processor time taken(ms): {}", (double(end - start)) * 1000.0 / CLOCKS_PER_SEC);
    spdlog::info("Checked {} nodes", search_log.size());

    visual::initialize();
    auto display = al_create_display(params.display_width, params.display_height);

    auto visual_start = std::chrono::steady_clock::now() + std::chrono::milliseconds(int64_t(1000.0 * params.wait_seconds));
    std::this_thread::sleep_until(visual_start);

    auto queue = visual::EventReactor();

    auto frame_timer = visual::Timer(1.0 / params.desired_fps);
    frame_timer.start();
    queue.register_source(frame_timer.event_source());

    using visual::Grid;
    Grid grid(maze, float(params.display_width), float(params.display_height));

    const double visualization_time = std::clamp(
        double(search_log.size()) * params.desireable_time_per_step,
        params.min_visualization_time,
        params.max_visualization_time
    );
    const auto progress_step = visualization_time / double(search_log.size());
    auto progress_timer = visual::Timer(progress_step);
    progress_timer.start();
    queue.register_source(progress_timer.event_source());
    queue.add_reaction(progress_timer.event_source(), [&, cur_idx = size_t(0), discover_idx = size_t(0)] (const auto&) mutable {
        if (cur_idx == search_log.size()) {
            progress_timer.stop();
            if (path.empty()) {
                spdlog::info("No way!");
            } else {
                spdlog::info("Path length: {}. Checked {} nodes", path.size(), search_log.size());
            }
            for (const auto& node : path) {
                grid.get_cell(node.x, node.y) = Grid::Cell{.color = grid.style().path_color};
            }
            queue.drop_all();
            return;
        }
        if (cur_idx > 0) {
            auto last = search_log[cur_idx - 1];
            grid.get_cell(last.x, last.y) = Grid::Cell{.color = grid.style().used_color};
            for (; discover_idx < discover_log.size() && discover_log[discover_idx].second <= cur_idx + 1; ++discover_idx) {
                const auto& discovered = discover_log[discover_idx].first;
                auto& cell = grid.get_cell(discovered.x, discovered.y);
                if (cell.color != grid.style().used_color) {
                    cell.color = grid.style().discovered_color;
                }
            }
        }
        auto checked_cell = search_log[cur_idx];
        grid.get_cell(checked_cell.x, checked_cell.y) = Grid::Cell {.color = grid.style().last_used_color};

        ++cur_idx;
    });
    queue.add_reaction(frame_timer.event_source(), [&](const auto&){
        al_clear_to_color(al_map_rgb(0, 0, 0));
        grid.draw();
        al_flip_display();
    });

    main_visual_loop(queue, display);
    al_destroy_display(display);
}

