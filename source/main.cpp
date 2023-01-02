#include <spdlog/spdlog.h>

#include "algos/BFS.hpp"
#include "algos/DFS.hpp"
#include "visual/grid.hpp"

#include <visual/allegro_util.hpp>
#include <maze.hpp>

namespace rng = std::ranges;


int main(int argc, char** argv) {
    if (argc < 4) {
        spdlog::error("Specify grid height, width and the algorithm (0 = bfs, 1 = dfs)");
        return 1;
    }

    auto get_arg_int = [&](size_t index) {
        return size_t(std::stol(argv[index]));
    };
    const size_t height = get_arg_int(1);
    const size_t width = get_arg_int(2);
    const size_t algorithm = get_arg_int(3);

    const auto wall_probability = 0.4;
    Maze maze = Maze::generate_simple(width, height, wall_probability);
    auto [from, to] = Maze::add_start_finish(maze);
    spdlog::info("searching path to {}, {}", to.x, to.y);
    std::vector<Maze::Node> search_log;
    auto path = algorithm == 0 
        ? algos::BFSFindPath<Maze::Node>(
            from, to, 
            [&](const Maze::Node& node) {
                search_log.push_back(node);
                auto n = maze.get_neighboors(node);
                return maze.get_neighboors(node);
            }
        )
        : algos::DFSFindPath<Maze::Node>(
            from, to, 
            [&](const Maze::Node& node) {
                search_log.push_back(node);
                auto n = maze.get_neighboors(node);
                return maze.get_neighboors(node);
            }
        );
    if (!path.empty()) {
        search_log.push_back(to);
    }

    visual::initialize();
    const auto display_width = 670;
    const auto display_height = 670;
    auto display = al_create_display(display_width, display_height);

    auto queue = visual::EventReactor();

    const double fps = 60.0;
    auto frame_timer = visual::Timer(1.0 / fps);
    frame_timer.start();
    queue.register_source(frame_timer.event_source());

    using visual::Grid;
    Grid grid(maze, float(display_width), float(display_height));

    const auto progress_step = std::min(
        std::max(frame_timer.get_rate() / 10, 10.0 / double(search_log.size())), 
        0.2
    );
    auto progress_timer = visual::Timer(progress_step);
    progress_timer.start();
    queue.register_source(progress_timer.event_source());
    queue.add_reaction(progress_timer.event_source(), [&, cur_idx = size_t(0)] (const auto&) mutable {
        if (cur_idx == search_log.size()) {
            progress_timer.stop();
            spdlog::info(path.empty() ? "No way!" : "That is all!");
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
    auto system_events_queue = visual::EventReactor();
    system_events_queue.register_source(al_get_keyboard_event_source());
    system_events_queue.register_source(al_get_display_event_source(display));
    while (true) {
        queue.wait_and_react();
        
        if (system_events_queue.empty()) {
            continue;
        }
        ALLEGRO_EVENT event;
        system_events_queue.get(event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }
    }
    al_destroy_display(display);
}

