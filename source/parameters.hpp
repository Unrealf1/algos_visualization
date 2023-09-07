#pragma once

#include <cstddef>
#include <filesystem>
#include <spdlog/common.h>


struct ApplicationParams {
    size_t maze_width;
    size_t maze_height;

    int display_width;
    int display_height;

    enum class EAlgorithm {
        BFS, DFS, Dijkstra, AStar
    } algorithm;

    double desired_fps;
    double min_visualization_time;
    double max_visualization_time;
    double desireable_time_per_step;
    double slow_tile_chance;
    double slow_tile_cost;
    spdlog::level::level_enum debug_level;
    size_t fixed_seed;
    std::string load_file;

private:
    friend ApplicationParams load_application_params(const std::filesystem::path&);
    friend void save_application_parameters(const ApplicationParams&, const std::filesystem::path&);
    inline static const char* const s_field_names_in_order[] = {
        "maze_width",
        "maze_height",

        "display_width",
        "display_height",
        "algorithm",

        "desired_fps",
        "min_visualization_time",
        "max_visualization_time",
        "desireable_time_per_step",
        "slow_tile_chance",
        "slow_tile_cost",
        "debug_level",
        "fixed_seed",
        "load_file"
    };
};

ApplicationParams& get_cached_application_params(const std::filesystem::path&, bool force_update = false);
ApplicationParams load_application_params(const std::filesystem::path&);
void save_application_parameters(const ApplicationParams&, const std::filesystem::path&);

