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
    spdlog::level::level_enum debug_level;
    size_t fixed_seed;

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
        "debug_level",
        "fixed_seed"
    };
};

ApplicationParams load_application_params(const std::filesystem::path&);
void save_application_parameters(const ApplicationParams&, const std::filesystem::path&);

