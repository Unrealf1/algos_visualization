#pragma once

#include <maze/maze.hpp>


struct GuiData{
    MazeObject draw_object = MazeObject::wall; 
    int maze_width = 20;
    int maze_height = 20;

    bool fill_maze = false;
};

GuiData& get_gui_data();

void draw_gui();