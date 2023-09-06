#include <spdlog/spdlog.h>

#include "algos/BFS.hpp"
#include "algos/DFS.hpp"
#include "algos/dijkstra.hpp"
#include "algos/a_star.hpp"
#include "visual/grid.hpp"

#include <stdexcept>
#include <visual/allegro_util.hpp>
#include <maze.hpp>
#include <parameters.hpp>
#include <random_utils.hpp>


int main() {
    auto params = get_cached_application_params("config.json");
    spdlog::set_level(params.debug_level);
    set_random_seed(params.fixed_seed);

    const auto wall_probability = 0.4;
    Maze maze = Maze::generate_simple(params.maze_width, params.maze_height, wall_probability);
    auto [from, to] = Maze::add_start_finish(maze);
    // increases chances for generation with existing path
    for (const auto& node : { Maze::Node{0, 1}, { 1, 0 }, {1, 1} }) {
        maze.get_cell(node) = MazeObject::space;
    }
    spdlog::info("searching path from {}, {} to {}, {}", from.x, from.y, to.x, to.y);
    std::vector<Maze::Node> search_log;
    clock_t start = clock();
    auto path = [&] {
        auto edge_getter = [&](const Maze::Node& node) {
            return maze.get_neighboors(node);
        };
        auto logging_searcher = [&](const Maze::Node& node) {
            search_log.push_back(node);
            return node == to;
        };
        using algos::Equals;
        using namespace algos;
        switch (params.algorithm) {
            case ApplicationParams::EAlgorithm::BFS: {
                return BFSFindPath<Maze::Node>(from, logging_searcher, edge_getter);
            }
            case ApplicationParams::EAlgorithm::DFS: {
                return DFSFindPath<Maze::Node>(from, logging_searcher, edge_getter);
            }
            case ApplicationParams::EAlgorithm::Dijkstra: {
                return DijkstraFindPath(from, logging_searcher, edge_getter, [](const auto&, const auto&) {
                    return 1.0;
                });
            }
            case ApplicationParams::EAlgorithm::AStar: {
                return AStarFindPath(from, logging_searcher, edge_getter, [](const auto&, const auto&) {
                    return 1.0;
                }, [&](const Maze::Node& node) {
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

    visual::initialize();
    auto display = al_create_display(params.display_width, params.display_height);

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
    queue.add_reaction(progress_timer.event_source(), [&, cur_idx = size_t(0)] (const auto&) mutable {
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

