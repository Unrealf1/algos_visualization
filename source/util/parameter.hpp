#pragma once

#include <functional>
#include <type_traits>
#include <util/overload_macros.h>


namespace util {
    template<typename T, const char* const Name>
    struct Parameter {
        T value{};

        using Type = T;

        operator T() const {
            return value;
        }

        Parameter& operator=(const T& t) {
            value = t;
            return *this;
        }

        Parameter& operator=(T&& t) {
            value = std::move(t);
            return *this;
        }
    };


    template<typename T, const char* const Name>
    struct RestrainedParameter : public Parameter<T, Name> {
        T min;
        T max;
    };

    template<typename>
    struct is_parameter : public std::false_type {};

    template<typename T, const char* const Name>
    struct is_parameter<Parameter<T, Name>> : public std::true_type {};

    template<typename T, const char* const Name>
    struct is_parameter<RestrainedParameter<T, Name>> : public std::true_type {};

    template<typename T>
    struct is_parameter<const T> : public is_parameter<std::decay_t<T>> {};

    template<typename T>
    struct is_parameter<T&> : public is_parameter<std::decay_t<T>> {};

    template<typename T, const char* const Name>
    constexpr const char* parameter_name(const Parameter<T, Name>&) {
        return Name;
    }
}

// For use INSIDE of structs and classes

#define PARAMETER( ... ) OVERLOAD_VA_SELECT( PARAMETER, __VA_ARGS__ )

#define PARAMETER_2(Type, Name) static inline const char util_parameter_name_##Name[] = #Name; \
util::Parameter< Type, util_parameter_name_##Name > Name

#define PARAMETER_3(Type, Name, Default) static inline const char util_parameter_name_##Name[] = #Name; \
util::Parameter< Type, util_parameter_name_##Name > Name{Default}

#define RESTRAINED_PARAMETER( ... ) OVERLOAD_VA_SELECT( RESTRAINED_PARAMETER, __VA_ARGS__ )

#define RESTRAINED_PARAMETER_4(Type, Name, Min, Max) static inline const char util_parameter_name_##Name[] = #Name; \
util::RestrainedParameter< Type, util_parameter_name_##Name > Name { {} , Min, Max}

#define RESTRAINED_PARAMETER_5(Type, Name, Default, Min, Max) static inline const char util_parameter_name_##Name[] = #Name; \
util::RestrainedParameter< Type, util_parameter_name_##Name > Name { { Default } , Min, Max}

