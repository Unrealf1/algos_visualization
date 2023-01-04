#include <random_utils.hpp>


static size_t s_seed = 0;

void set_random_seed(size_t seed) {
    if (s_seed != 0) {
        throw std::logic_error("reinitialization of random seed is not supported");
    }
    s_seed = seed;
}

std::default_random_engine& get_rengine() {
    if (s_seed == 0) {
        std::random_device rd{};
        set_random_seed(rd());
        spdlog::info("random seed: {}", s_seed);
    }
    static std::default_random_engine rengine(s_seed);
    return rengine;
}

