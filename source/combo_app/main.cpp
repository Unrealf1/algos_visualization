#include <visual/allegro_util.hpp>
#include <visual/grid.hpp>
#include <visual/imgui_inc.hpp>
#include <spdlog/spdlog.h>
#include <gui.hpp>
#include <app_actions.hpp>
#include <algorithm>

namespace rng = std::ranges;

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


auto init_display() {
  al_set_new_display_flags(ALLEGRO_RESIZABLE);
#ifdef __EMSCRIPTEN__
  double w;
  double h;
  emscripten_get_element_css_size("#canvas", &w, &h );
  auto display = al_create_display(int(w), int(h));
#else
  ALLEGRO_MONITOR_INFO monitorInfo;
  int w = 500;
  int h = 500;
  if (al_get_monitor_info(0, &monitorInfo)) {
    w = int(float(monitorInfo.x2 - monitorInfo.x1) * 0.7f);
    h = int(float(monitorInfo.y2 - monitorInfo.x1) * 0.7f);
  }
  auto display = al_create_display(w, h);
#endif

  ImGui_ImplAllegro5_Init(display);
  return display;
}


int main() {
  visual::initialize();
  auto display = init_display();

  auto& config = combo_app_gui::get_data();
  
  Maze maze(config.m_creation_data.maze_width, config.m_creation_data.maze_height);

  auto displayWidth = al_get_display_width(display);
  auto displayHeight = al_get_display_height(display);
  visual::Grid grid(maze, displayWidth, displayHeight);

  auto queue = visual::EventReactor();

  auto gui_update_timer = visual::Timer(1.0 / 60.0);
  gui_update_timer.start();
  queue.register_source(gui_update_timer.event_source());


  auto react_to_gui = [&] {
    if (grid.style().draw_lattice != config.m_creation_data.draw_grid) {
      grid.style().draw_lattice = config.m_creation_data.draw_grid;
      grid.request_full_redraw();
    }
    if (config.m_creation_data.maze_width != int(maze.width) || config.m_creation_data.maze_height != int(maze.height)) {
      maze.resize(size_t(config.m_creation_data.maze_width), size_t(config.m_creation_data.maze_height));
      grid.update(maze);
    }

    if (config.m_creation_data.fill_maze) {
      config.m_creation_data.fill_maze = false;
      rng::fill(maze.items, config.m_creation_data.draw_object);
      grid.update(maze);
    }

    if (config.m_creation_data.generate_maze) {
      config.m_creation_data.generate_maze = false;
      maze = create_maze(config.m_creation_data);
      grid.update(maze);
    }
  };

  queue.add_reaction(gui_update_timer.event_source(), [&] (const auto&) {
    react_to_gui();

    al_clear_to_color(al_map_rgb(0, 0, 0));
    grid.draw(display);
    combo_app_gui::draw();

    al_flip_display();
  });

  queue.register_source(al_get_mouse_event_source());
  queue.register_source(al_get_keyboard_event_source());
  queue.register_source(al_get_display_event_source(display));

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

  queue.add_reaction(al_get_mouse_event_source(), [&, last_mouse_pos = std::pair{-1, -1}](auto event) mutable {
    ImGui_ImplAllegro5_ProcessEvent(&event);
    if (ImGui::GetIO().WantCaptureMouse) {
      return;
    }
    if (config.m_mode != combo_app_gui::AppMode::Creation) {
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
        return config.m_creation_data.draw_object.value;
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
        
    apply_brush_to_grid(state, maze, grid, type_to_set);
    last_mouse_pos = std::pair{state.x, state.y};
  });

  main_visual_loop(queue, display);

  al_destroy_display(display);
}

