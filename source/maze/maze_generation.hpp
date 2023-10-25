#pragma once

#include "maze.hpp"

enum class EMazeGenerationAlgorithm {
    noise, random_dfs, binary_tree
};

Maze generate_white_noise(size_t height, size_t width, double wall_prob);
Maze generate_random_dfs(size_t height, size_t width);
Maze generate_binary_tree(size_t height, size_t width, float horizontal_prob = 0.5f);

