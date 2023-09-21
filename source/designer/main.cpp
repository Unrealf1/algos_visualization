#include <visual/allegro_util.hpp>
#include <visual/grid.hpp>
#include <maze/maze.hpp>
#include <gui.hpp>
#include <spdlog/spdlog.h>
#include <visual/imgui_inc.hpp>


int main() {
    visual::initialize();

    const auto display_dim = 600;
    const auto maze_dim = 20;
    auto& gui_data = get_gui_data();
    gui_data.maze_height = maze_dim;
    gui_data.maze_height = maze_dim;

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
        if (gui_data.maze_width != maze.width || gui_data.maze_height != maze.height) {
            Maze new_maze(gui_data.maze_width, gui_data.maze_height);
            for (size_t x = 0; x < maze.width; ++x) {
                for (size_t y = 0; y < maze.height; ++y) {
                    if (x >= new_maze.width || y >= new_maze.height) {
                        continue;
                    }
                    new_maze.get_cell({x, y}) = maze.get_cell({x, y});
                }
            }
            maze = std::move(new_maze);
            grid = visual::Grid(maze, display_dim, display_dim);
            grid.style().draw_lattice = true;
        }

        if (gui_data.fill_maze) {
            gui_data.fill_maze = false;
            for (auto& item : maze.items) {
                item = gui_data.draw_object;
            }
            grid = visual::Grid(maze, display_dim, display_dim);
            grid.style().draw_lattice = true;
        }
    };

    queue.add_reaction(gui_update_timer.event_source(), [&] (const auto&) {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        grid.draw();
        draw_gui();

        process_gui_changes();

        al_flip_display();
    });
    
    queue.register_source(al_get_mouse_event_source());

    queue.add_reaction(al_get_mouse_event_source(), [&, last_mouse_pos = std::pair{-1, -1}, last_z = 0](auto event) mutable {
        ImGui_ImplAllegro5_ProcessEvent(&event);
        if (ImGui::GetIO().WantCaptureMouse) {
            return;
        }
        ALLEGRO_MOUSE_STATE state;
        al_get_mouse_state(&state);
        if (state.x < 0 || state.y < 0 || state.x > display_dim || state.y > display_dim) {
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
        
        const float cell_width = display_dim / float(maze.width);
        const float cell_height = display_dim / float(maze.height);
        const float cell_dimention = std::min(cell_width, cell_height);
        const float grid_width = cell_dimention * float(maze.width);
        const float grid_height = cell_dimention * float(maze.height);
        const float grid_offset_x = (display_dim - grid_width) / 2;
        const float grid_offset_y = (display_dim - grid_height) / 2;

        const auto coords_x = std::min(size_t((state.x - grid_offset_x) * maze.width / grid_width), size_t(maze.width - 1)); 
        const auto coords_y = std::min(size_t((state.y - grid_offset_y) * maze.height / grid_height), size_t(maze.height - 1)); 
        auto& maze_cell = maze.get_cell({coords_x, coords_y});
        maze_cell = type_to_set;
        grid.get_cell(coords_x, coords_y).color = grid.style().color_map[maze_cell];
        last_mouse_pos = std::pair{state.x, state.y};
    });

    main_visual_loop(queue, display);

    maze.save("designed.maze");

    al_destroy_display(display);
}
