#include <app_actions.hpp>

#include <algorithm>

namespace rng = std::ranges;


void apply_brush_to_grid(const ALLEGRO_MOUSE_STATE& state,
                         Maze& maze,
                         visual::Grid& grid,
                         MazeObject type_to_set) {
  auto& config = combo_app_gui::get_data();

  auto [coords_x, coords_y] = grid.get_cell_under_cursor_coords(state.x, state.y);
  const auto brush_size = config.m_creation_data.brush_size;
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
}

Maze create_maze(const combo_app_gui::CreationData& gui_data) {
    const size_t width = size_t(gui_data.maze_width);
    const size_t height = size_t(gui_data.maze_height);
    switch (gui_data.generation_algorithm) {
        case EMazeGenerationAlgorithm::noise: {
            const auto& params = gui_data.whiteNoseGenerationParameters;
            Maze maze = generate_white_noise(width, height, double(params.wall_prob.value));
            Maze::add_random_start_finish(maze);
            maze.add_slow_tiles(double(params.slow_prob));
            return maze;
        }
        case EMazeGenerationAlgorithm::random_dfs: {
            const auto& params = gui_data.randomDfsGenerationParameters;
            Maze maze = generate_random_dfs(width, height);
            maze.add_slow_tiles(double(params.slow_prob));
            return maze;
        }
        case EMazeGenerationAlgorithm::binary_tree: {
            const auto& params = gui_data.binaryTreeParameters;
            Maze maze = generate_binary_tree(width, height, params.horizontal_prob);
            maze.add_slow_tiles(double(params.slow_prob));
            return maze;
        }
        case EMazeGenerationAlgorithm::sidewinder: {
            const auto& params = gui_data.sidewinderParameters;
            Maze maze = generate_sidewinder(width, height, params.group_prob);
            maze.add_slow_tiles(double(params.slow_prob));
            return maze;
        }
    }
    throw std::logic_error("Unknown maze generation algorithm!");
}
