#pragma once

#include <gui.hpp>
#include <visual/allegro_util.hpp>
#include <visual/grid.hpp>


void apply_brush_to_grid(const ALLEGRO_MOUSE_STATE& state,
                         Maze& maze,
                         visual::Grid& grid,
                         MazeObject type_to_set,
                         float scale, float dx, float dy);

Maze create_maze(const combo_app_gui::CreationData& gui_data);

