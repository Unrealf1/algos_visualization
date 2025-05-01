#include <visual/allegro_util.hpp>
#include <visual/grid.hpp>
#include <visual/imgui_inc.hpp>
#include <util/random_utils.hpp>
#include <spdlog/spdlog.h>
#include <gui.hpp>
#include <app_actions.hpp>
#include <algorithm>

#include "algos/BFS.hpp"
#include "algos/DFS.hpp"
#include "algos/dijkstra.hpp"
#include "algos/a_star.hpp"

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

using edge_getter_t = std::vector<Maze::Node> (*) (const Maze&, const Maze::Node&);

edge_getter_t create_edge_getter(bool allow_diagonals, bool require_adjacent_for_diagonals) {
    if (!allow_diagonals) {
        return [](const Maze& maze, const Maze::Node& node) {
            return maze.get_cross_neighboors(node);
        };
    }
    if (require_adjacent_for_diagonals) {
        return [](const Maze& maze, const Maze::Node& node) {
            return maze.get_sides_and_corners(node, true);
        };
    }
    return [](const Maze& maze, const Maze::Node& node) {
        return maze.get_sides_and_corners(node, false);
    };
}

std::vector<std::pair<Maze::Node, float>> estimates_log;
std::vector<Maze::Node> search_log;
std::vector<std::pair<Maze::Node, size_t>> discover_log;
algos::NodePath<Maze::Node> path;
size_t cur_idx = 0;
size_t discover_idx = 0;


void clear_visualization() {
  estimates_log.clear();
  search_log.clear();
  discover_log.clear();
  path.clear();
  cur_idx = 0;
  discover_idx = 0;
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

  auto progress_timer = visual::Timer(1.0);
  queue.register_source(progress_timer.event_source());

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

    if (config.m_visualization_data.runPathfinding) {
      config.m_visualization_data.runPathfinding = false;
      clear_visualization();
      grid.update(maze);
      Maze::Node from {util::idx_to_coords(maze.from, maze.width)};
      Maze::Node to {util::idx_to_coords(maze.to, maze.width)};

      auto edge_getter = create_edge_getter(
          config.m_visualization_data.allow_diagonals.value,
          config.m_visualization_data.require_adjacent_for_diagonals.value
      );
      auto logging_edge_getter = [&](const Maze::Node& node) {
          auto neighboors = edge_getter(maze, node);
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
          return maze.get_cell(to) == MazeObject::slow ? config.m_creation_data.slow_tile_cost.value : 1.0;
      };

      auto logging_estimate_getter = [&](const Maze::Node& node) {
          auto dx = node.x - to.x;
          auto dy = node.y - to.y;
          auto estimate = std::sqrt(dx * dx + dy * dy);
          estimates_log.push_back({node, estimate});
          return estimate;
      };

      clock_t start = clock();
      path = [&] {
          using namespace algos;
          switch (config.m_visualization_data.algorithm.value) {
              case combo_app_gui::EAlgorithm::BFS: {
                  return BFSFindPath<Maze::Node>(from, logging_searcher, logging_edge_getter);
              }
              case combo_app_gui::EAlgorithm::DFS: {
                  return DFSFindPath<Maze::Node>(from, logging_searcher, logging_edge_getter);
              }
              case combo_app_gui::EAlgorithm::RandomDFS: {
                  return DFSFindPath<Maze::Node>(from, logging_searcher, random_logging_edge_getter);
              }
              case combo_app_gui::EAlgorithm::Dijkstra: {
                  return DijkstraFindPath(from, logging_searcher, logging_edge_getter, weight_getter);
              }
              case combo_app_gui::EAlgorithm::AStar: {
                  return AStarFindPath(from, logging_searcher, logging_edge_getter, weight_getter, logging_estimate_getter);
              }
          }
          // should not be reachable. Kept here for now because of gcc warning(end of non-void finction)
          throw std::logic_error("Unknown algorithm!");
      }();
      clock_t end = clock();
      spdlog::info("Processor time taken(ms): {}", (double(end - start)) * 1000.0 / CLOCKS_PER_SEC);

      const auto progress_step = 10.0 / double(search_log.size());
      progress_timer.change_rate(progress_step);
      progress_timer.start();
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

  queue.add_reaction(progress_timer.event_source(), [&] (const auto&) mutable {
      if (cur_idx == search_log.size()) {
          progress_timer.stop();
          if (path.empty()) {
              spdlog::info("No way!. Checked {} nodes", search_log.size());
          } else {
              spdlog::info("Path length: {}. Checked {} nodes", path.size(), search_log.size());
          }
          for (const auto& node : path) {
              grid.set_cell(node.x, node.y, {.color = grid.style().path_color});
          }
          queue.drop_all();
          return;
      }
      if (cur_idx > 0) {
          auto last = search_log[cur_idx - 1];
          grid.set_cell(last.x, last.y, {.color = grid.style().used_color});
          for (; discover_idx < discover_log.size() && discover_log[discover_idx].second <= cur_idx + 1; ++discover_idx) {
              const auto& discovered = discover_log[discover_idx].first;
              const auto& cell = grid.get_cell(discovered.x, discovered.y);
              if (cell.color != grid.style().used_color) {
                  grid.set_cell(discovered.x, discovered.y, {.color = grid.style().discovered_color});
              }
          }
      }
      auto checked_cell = search_log[cur_idx];
      grid.set_cell(checked_cell.x, checked_cell.y, {.color = grid.style().last_used_color});

      ++cur_idx;
  });

  main_visual_loop(queue, display);

  al_destroy_display(display);
}

