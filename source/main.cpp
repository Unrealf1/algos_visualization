#include <spdlog/spdlog.h>

#include "algos/BFS.hpp"
#include "visual/grid.hpp"

#include <random>
#include <algorithm>
#include <iostream>
#include <compare>

#include <visual/allegro_util.hpp>
#include <maze.hpp>
#include <random_utils.hpp>

namespace rng = std::ranges;

int main(int argc, char** argv) {
    if (argc < 3) {
        spdlog::error("Specify field height and width");
        return 1;
    }

    auto get_arg_int = [&](size_t index) {
        return size_t(std::stol(argv[index]));
    };

    const size_t height = get_arg_int(1);
    const size_t width = get_arg_int(2);

    Maze maze(width, height, 0.3);

    Maze::Node from = {0, 0};
    Maze::Node to{};
    auto& rengine = get_rengine();
    to.x = std::uniform_int_distribution<size_t>(0, width - 1)(rengine);
    to.y = std::uniform_int_distribution<size_t>(0, height - 1)(rengine);
    maze.get_cell(to) = MazeObject::space;
    spdlog::info("searching path to {}, {}", to.x, to.y);
    std::vector<Maze::Node> search_log;
    auto path = algos::BFSFindPath<Maze::Node>(
            from,
            to, 
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
    const double fps = 60.0;
    auto frame_timer = visual::Timer(1.0 / fps);
    frame_timer.start();
    auto queue = visual::EventQueue();
    auto display_width = 670;
    auto display_height = 670;
    auto disp = al_create_display(display_width, display_height);
    bool redraw = false;

    queue.register_source(al_get_keyboard_event_source());
    queue.register_source(al_get_display_event_source(disp));
    queue.register_source(frame_timer.event_source());

    visual::Grid grid(width, height, float(display_width), float(display_height));
    for (size_t h = 0; h < height; ++h) {
        for (size_t w = 0; w < width; ++w) {
            if (w == from.x && h == from.y) {
                //std::cout << 'f';
                grid.set_cell(w, h, {.color = al_map_rgb(20, 180, 80)});
            } else if (w == to.x && h == to.y) {
                //std::cout << 't';
                grid.set_cell(w, h, {.color = al_map_rgb(20, 60, 180)});
            } else if (rng::find(path, Maze::Node{w, h}) != path.end()) {
                //std::cout << '+';
                grid.set_cell(w, h, {.color = (maze.get_cell({w, h}) == MazeObject::wall 
                            ? al_map_rgb(0, 0, 0) 
                            : al_map_rgb(255, 255, 255))});
            } else {
                grid.set_cell(w, h, {.color = (maze.get_cell({w, h}) == MazeObject::wall 
                            ? al_map_rgb(0, 0, 0) 
                            : al_map_rgb(255, 255, 255))});
                //std::cout << (maze.get_cell({w, h}) == MazeObject::wall ? '#' : ' ');
            }
        }
        //std::cout << '\n';
    }

    const auto progress_step = std::min(std::max(frame_timer.get_rate() / 2, 8.0 / double(search_log.size())), 0.2);
    auto progress_timer = visual::Timer(progress_step);
    progress_timer.start();
    queue.register_source(progress_timer.event_source());
    auto draw_path = [&] {
        for (const auto& node : path) {
            grid.set_cell(node.x, node.y, {al_map_rgb(182, 102, 142)});
        }
    };
    auto search_visualizer = [&, cur_idx = size_t(0)] mutable {
        if (cur_idx == search_log.size()) {
            return;
        }
        if (cur_idx > 0) {
            auto last = search_log[cur_idx - 1];
            grid.set_cell(last.x, last.y, {.color = al_map_rgb(200, 200, 0)});
        }
        auto checked_cell = search_log[cur_idx];
        grid.set_cell(checked_cell.x, checked_cell.y, {.color = al_map_rgb(255, 0, 0)});

        ++cur_idx;
        if (cur_idx == search_log.size()) {
            spdlog::info(path.empty() ? "No way!" : "That is all!");
            draw_path();
        }
    };

    ALLEGRO_EVENT event;
    while (true) {
        queue.wait(event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        if(event.type == ALLEGRO_EVENT_TIMER) {
            if (event.timer.source == frame_timer.get_raw()) {
                redraw = true;
            } else if (event.timer.source == progress_timer.get_raw()) {
                search_visualizer();
            }
        }

        if (redraw && queue.empty()) {
            al_clear_to_color(al_map_rgb(0, 0, 0));

            grid.draw();

            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_display(disp);
}

