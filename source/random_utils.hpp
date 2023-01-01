#pragma once

#include <random>
#include <spdlog/spdlog.h>


inline auto& get_rengine() {
    static std::random_device rd{};
    static size_t seed = 0;
    if (seed == 0) {
        seed = rd();
        spdlog::info("seed: {}", seed);
    }
    static std::default_random_engine rengine(seed);
    return rengine;
}
