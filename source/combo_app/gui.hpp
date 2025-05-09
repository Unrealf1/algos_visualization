#pragma once

#include <maze/maze_generation.hpp>
#include <maze/generation_parameters.hpp>
#include <util/parameter.hpp>


namespace combo_app_gui{
  struct CreationData {
    PARAMETER(size_t, fixed_seed, 0);
    PARAMETER(bool, draw_grid, false);
    PARAMETER(int, maze_width, 50);
    PARAMETER(int, maze_height, 50);
    PARAMETER(MazeObject, draw_object, MazeObject::wall);
    
    RESTRAINED_PARAMETER(int, brush_size, 1, 1, 100);

    PARAMETER(EMazeGenerationAlgorithm, generation_algorithm, EMazeGenerationAlgorithm::random_dfs);
    bool fill_maze = false;
    bool generate_maze = true;
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
    maze_generation::SidewinderParameters sidewinderParameters;
    RESTRAINED_PARAMETER(float, slow_tile_cost, 2.0f, 0.0f, 10.0f);

    bool update_visuals = false;
  };

  enum class EAlgorithm {
      BFS, DFS, RandomDFS, Dijkstra, AStar
  };

  struct VisualizationData {
    PARAMETER(EAlgorithm, algorithm);

    PARAMETER(bool, allow_diagonals);
    PARAMETER(bool, require_adjacent_for_diagonals);

    RESTRAINED_PARAMETER(double, desireable_time_per_step, 0.005, 0.0001, 1.0);

    bool runPathfinding = false;
  };

  struct VisualizationProgress {
    bool path_found = false;
    bool finished = false;
    bool display = false;
    uint64_t nodes_checked;
    uint64_t path_length;
    double processor_time_ms;
    double path_cost;
  };

  enum class AppMode{
    Creation, PathFinding
  };

  struct Data {
    AppMode m_mode = AppMode::Creation;
    CreationData creation_data{}; 
    VisualizationData visualization_data{};
    VisualizationProgress visualization_progress{};
  };

  Data& get_data();
  void draw();
}

