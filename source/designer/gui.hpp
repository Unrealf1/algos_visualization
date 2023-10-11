#pragma once

#include <maze/maze.hpp>


struct GuiData{
    MazeObject draw_object = MazeObject::wall; 
    int brush_size = 1;
    int maze_width = 20;
    int maze_height = 20;

    EMazeGenerationAlgorithm generation_algorithm = EMazeGenerationAlgorithm::noise;
    bool fill_maze = false;
    bool generate_maze = false;
    struct SaveData {
        bool do_save = false;
        std::string file_path_name;
    } save_data;
};

GuiData& get_gui_data();

void draw_gui();