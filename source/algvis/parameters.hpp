#pragma once

#include <cstddef>
#include <filesystem>
#include <spdlog/common.h>
#include <maze/maze.hpp>
#include <util/parameter.hpp>


struct ApplicationParams {
    PARAMETER(size_t, maze_width);
    PARAMETER(size_t, maze_height);

    PARAMETER(int, display_width);
    PARAMETER(int, display_height);

    enum class EAlgorithm {
        BFS, DFS, RandomDFS, Dijkstra, AStar
    };
    PARAMETER(EAlgorithm, algorithm);

    PARAMETER(EMazeGenerationAlgorithm, generation_algorithm);

    PARAMETER(double, wait_seconds);
    PARAMETER(double, desired_fps);
    PARAMETER(double, min_visualization_time);
    PARAMETER(double, max_visualization_time);
    PARAMETER(double, desireable_time_per_step);
    RESTRAINED_PARAMETER(double, slow_tile_chance, 0.0, 1.0);
    RESTRAINED_PARAMETER(double, slow_tile_cost, 0.0, 1.0);
    PARAMETER(spdlog::level::level_enum, debug_level);
    PARAMETER(size_t, fixed_seed);
    PARAMETER(std::string, load_file);
    PARAMETER(std::string, save_file);
};

ApplicationParams& get_cached_application_params(const std::filesystem::path&, bool force_update = false);
ApplicationParams load_application_params(const std::filesystem::path&);
void save_application_parameters(const ApplicationParams&, const std::filesystem::path&);

