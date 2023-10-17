#pragma once

#include <functional>


namespace util {

    template<typename T, const char* const Name>
    struct Parameter {
        T value{};
    };


    template<typename T, const char* const Name>
    struct RestrainedParameter : public Parameter<T, Name> {
        T min;
        T max;
    };

    #define PARAMETER(Type, Name) static inline const char util_parameter_name_##Name[] = #Name; \
    Parameter< Type, util_parameter_name_##Name > Name

    #define RESTRAINED_PARAMETER(Type, Name, Min, Max) static inline const char util_parameter_name_##Name[] = #Name; \
    RestrainedParameter< Type, util_parameter_name_##Name > Name { {} , Min, Max}
}

