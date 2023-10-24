#pragma once

#include <maze/maze.hpp>
#include <maze/generation_parameters.hpp>


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
    struct LoadData {
        bool do_load = false;
        std::string file_path_name;
    } load_data;

    maze_generation::RandomDfsParameters randomDfsGenerationParameters;
    maze_generation::WhiteNoiseParameters whiteNoseGenerationParameters;
    maze_generation::BinaryTreeParameters binaryTreeParameters;

    struct VisualParameters {
        bool operator==(const VisualParameters&) const = default;
        PARAMETER(bool, draw_grid);
    } visual_parameters;
    bool update_visuals = false;
};

GuiData& get_gui_data();

void draw_gui();

