#include <app_actions.hpp>

#include <algorithm>

namespace rng = std::ranges;


void apply_brush_to_grid(int mouse_x, int mouse_y,
                         Maze& maze,
                         visual::Grid& grid,
                         MazeObject type_to_set,
                         float scale, float dx, float dy) {
  for_each_brush_affected_tile(mouse_x, mouse_y, maze, grid, scale, dx, dy, [&](int x, int y){
      const auto xsz = size_t(x);
      const auto ysz = size_t(y);
      auto& maze_cell = maze.get_cell({xsz, ysz});
      maze_cell = type_to_set;
      grid.set_cell(xsz, ysz, {.color = grid.style().color_map[maze_cell]});
      if (type_to_set == MazeObject::start) {
        maze.from = util::coords_to_idx(size_t(x), size_t(y), maze.width);
      } else if (type_to_set == MazeObject::finish) {
        maze.to = util::coords_to_idx(size_t(x), size_t(y), maze.width);
      }
  });
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
