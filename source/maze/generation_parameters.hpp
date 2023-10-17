#pragma once

#include <util/parameter.hpp>


namespace maze_generation {
    using util::Parameter;
    using util::RestrainedParameter;

    struct RandomDfsParameters {
        RESTRAINED_PARAMETER(float, slow_prob, 0.0f, 1.0f);
    };

    struct WhiteNoiseParameters {
        RESTRAINED_PARAMETER(float, wall_prob, 0.0f, 1.0f);
        RESTRAINED_PARAMETER(float, slow_prob, 0.0f, 1.0f);
    };
}

