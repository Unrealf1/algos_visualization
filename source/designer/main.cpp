#include <visual/allegro_util.hpp>
#include <visual/grid.hpp>
#include <maze/maze.hpp>
#include <gui.hpp>
#include <spdlog/spdlog.h>
#include <visual/imgui_inc.hpp>
#include <algorithm>

namespace rng = std::ranges;

Maze create_maze(const GuiData& gui_data) {
    const size_t width = size_t(gui_data.maze_width);
    const size_t height = size_t(gui_data.maze_height);
    switch (gui_data.generation_algorithm) {
        case EMazeGenerationAlgorithm::noise: {
            const auto& params = gui_data.whiteNoseGenerationParameters;
            Maze maze = Maze::generate_simple(width, height, double(params.wall_prob.value));
            Maze::add_random_start_finish(maze);
            maze.add_slow_tiles(double(params.slow_prob));
            return maze;
        }
        case EMazeGenerationAlgorithm::random_dfs: {
            const auto& params = gui_data.randomDfsGenerationParameters;
            Maze maze = Maze::generate_random_dfs(width, height);
            maze.add_slow_tiles(double(params.slow_prob));
            return maze;
        }
        case EMazeGenerationAlgorithm::binary_tree: {
            const auto& params = gui_data.binaryTreeParameters;
            Maze maze = Maze::generate_binary_tree(height, width);
            maze.add_slow_tiles(double(params.slow_prob));
            return maze;
        }
    }
    throw std::logic_error("Unknown maze generation algorithm!");
}

int main() {
    visual::initialize();

    const auto display_dim = 600;
    const auto maze_dim = 20;
    auto& gui_data = get_gui_data();
    gui_data.maze_height = maze_dim;
    gui_data.maze_height = maze_dim;

    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    auto display = al_create_display(display_dim, display_dim);
    ImGui_ImplAllegro5_Init(display);

    Maze maze(maze_dim, maze_dim);
    visual::Grid grid(maze, display_dim, display_dim);
    grid.style().draw_lattice = true;

    auto queue = visual::EventReactor();

    auto gui_update_timer = visual::Timer(1.0 / 60.0);
    gui_update_timer.start();
    queue.register_source(gui_update_timer.event_source());

    auto process_gui_changes = [&] {
        auto& gui_data = get_gui_data();
        if (gui_data.maze_width != int(maze.width) || gui_data.maze_height != int(maze.height)) {
            maze.resize(size_t(gui_data.maze_width), size_t(gui_data.maze_height));
            grid.update(maze);
        }

        if (gui_data.fill_maze) {
            gui_data.fill_maze = false;
            rng::fill(maze.items, gui_data.draw_object);
            grid.update(maze);
        }

        if (gui_data.generate_maze) {
            gui_data.generate_maze = false;
            maze = create_maze(gui_data);
            grid.update(maze);
        }

        if (gui_data.save_data.do_save) {
            gui_data.save_data.do_save = false;
            maze.save(gui_data.save_data.file_path_name);
            spdlog::info("Saved maze to \"{}\"", gui_data.save_data.file_path_name);
        }

        if (gui_data.load_data.do_load) {
            gui_data.load_data.do_load = false;
            maze = Maze::load(gui_data.load_data.file_path_name);
            grid.update(maze);
            spdlog::info("Loaded maze from \"{}\"", gui_data.load_data.file_path_name);
        }
    };

    queue.add_reaction(gui_update_timer.event_source(), [&] (const auto&) {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        grid.draw(display);
        draw_gui();

        process_gui_changes();

        al_flip_display();
    });
    
    queue.register_source(al_get_mouse_event_source());
    queue.register_source(al_get_keyboard_event_source());
    queue.register_source(al_get_display_event_source(display));

    queue.add_reaction(al_get_mouse_event_source(), [&, last_mouse_pos = std::pair{-1, -1}](auto event) mutable {
        ImGui_ImplAllegro5_ProcessEvent(&event);
        if (ImGui::GetIO().WantCaptureMouse) {
            return;
        }
        ALLEGRO_MOUSE_STATE state;
        al_get_mouse_state(&state);
        const auto [screen_width, screen_height] = grid.get_dimentions();
        if (state.x < 0 || state.y < 0 || float(state.x) > screen_width || float(state.y) > screen_height) {
            return;
        }
        bool shouldChangeMaze = true;
        MazeObject type_to_set = [&] {
            if (al_mouse_button_down(&state, 1)) {
                return get_gui_data().draw_object;
            }
            if (al_mouse_button_down(&state, 2)) {
                return MazeObject::space;
            }
            shouldChangeMaze = false;
            return MazeObject::space;
        }();
        if (!shouldChangeMaze) {
            return;
        }
        
        auto [coords_x, coords_y] = grid.get_cell_under_cursor_coords(state.x, state.y);
        const auto brush_size = get_gui_data().brush_size;
        const auto offset = -brush_size / 2;
        const int base_x = offset + int(coords_x);
        const int base_y = offset + int(coords_y);
        for (int dx = 0; dx < brush_size; ++dx) {
            for (int dy = 0; dy < brush_size; ++dy) {
                int x = base_x + dx;
                int y = base_y + dy;
                if (x < 0 || x >= int(maze.width)) {
                    continue;
                }
                if (y < 0 || y >= int(maze.height)) {
                    continue;
                }
                const auto xsz = size_t(x);
                const auto ysz = size_t(y);
                auto& maze_cell = maze.get_cell({xsz, ysz});
                maze_cell = type_to_set;
                grid.set_cell(xsz, ysz, {.color = grid.style().color_map[maze_cell]});
            }
        }
        last_mouse_pos = std::pair{state.x, state.y};
    });

    queue.add_reaction(al_get_keyboard_event_source(), [&](auto event) mutable {
        ImGui_ImplAllegro5_ProcessEvent(&event);
        if (ImGui::GetIO().WantCaptureKeyboard) {
            return;
        }
    });

    queue.add_reaction(al_get_display_event_source(display), [&](auto event) {
        if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
            grid.set_dimentions(float(event.display.width), float(event.display.height));
        }
    });

    main_visual_loop(queue, display);

    al_destroy_display(display);
}
