#pragma once

#include "maze.hpp"

enum class EMazeGenerationAlgorithm {
    noise, random_dfs, binary_tree, sidewinder
};

Maze generate_white_noise(size_t width, size_t height, double wall_prob);
Maze generate_random_dfs(size_t width, size_t height);
Maze generate_binary_tree(size_t width, size_t height, float horizontal_prob = 0.5f);
Maze generate_sidewinder(size_t width, size_t height, float group_prob = 0.5f);

