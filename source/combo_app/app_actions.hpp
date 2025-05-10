#pragma once

#include <gui.hpp>
#include <visual/grid.hpp>


void apply_brush_to_grid(int mouse_x, int mouse_y,
                         Maze& maze,
                         visual::Grid& grid,
                         MazeObject type_to_set,
                         float scale, float dx, float dy);

Maze create_maze(const combo_app_gui::CreationData& gui_data);

