#pragma once

#include <gui.hpp>
#include <visual/grid.hpp>


void for_each_brush_affected_tile(int mouse_x, int mouse_y,
                                  const Maze& maze,
                                  const visual::Grid& grid,
                                  float scale, float dx, float dy,
                                  auto callback){
  auto& config = combo_app_gui::get_data();

  auto modifiedX = (mouse_x - dx) / scale;
  auto modifiedY = (mouse_y - dy) / scale;
  auto [coords_x, coords_y] = grid.get_cell_under_cursor_coords(modifiedX, modifiedY);
  const auto brush_size = config.creation_data.brush_size;
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
      callback(x, y);
    }
  }
}


void apply_brush_to_grid(int mouse_x, int mouse_y,
                         Maze& maze,
                         visual::Grid& grid,
                         MazeObject type_to_set,
                         float scale, float dx, float dy);

Maze create_maze(const combo_app_gui::CreationData& gui_data);

