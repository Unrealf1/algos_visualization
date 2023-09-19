#pragma once

#include <random>
#include <spdlog/spdlog.h>


void set_random_seed(size_t seed);
std::default_random_engine& get_rengine();

bool chance(double probability, std::default_random_engine& rengine = get_rengine());
