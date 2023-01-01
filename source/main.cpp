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

    Maze maze(width, height);

    Maze::Node from = {0, 0};
    Maze::Node to{};
    auto& rengine = get_rengine();
    to.x = std::uniform_int_distribution<size_t>(0, width - 1)(rengine);
    to.y = std::uniform_int_distribution<size_t>(0, height - 1)(rengine);
    auto path = algos::BFSFindPath<Maze::Node>(
            from,
            to, 
            [&](const Maze::Node& node) {return maze.get_neighboors(node);}
    );
    for (size_t w = 0; w < width; ++w) {
        for (size_t h = 0; h < height; ++h) {
            if (w == from.x && h == from.y) {
                std::cout << 'f';
            } else if (w == to.x && h == to.y) {
                std::cout << 't';
            } else if (rng::find(path, Maze::Node{w, h}) != path.end()) {
                std::cout << '+';
            } else {
                std::cout << (maze.get_cell({w, h}) == MazeObject::wall ? '#' : ' ');
            }
        }
        std::cout << '\n';
    }

    visual::initialize();
    const double fps = 1.0;
    auto timer = al_create_timer(1.0 / fps);
    al_start_timer(timer);
    auto queue = al_create_event_queue();
    auto disp = al_create_display(320, 200);
    bool redraw = false;

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    visual::Grid grid(width, height);
    ALLEGRO_EVENT event;

    while (true) {

        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        if(event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            al_clear_to_color(al_map_rgb(0, 0, 0));

            grid.draw();

            al_flip_display();

            redraw = false;
        }
    }
}

