#pragma once

#include "imgui.h"
#include <type_traits>
#include <visual/imgui_inc.hpp>
#include <util/parameter.hpp>
#include <util/magic_enum_inc.h>
#include <boost/pfr.hpp>


namespace visual { namespace imgui {
    template<typename Enum>
    void draw_enum_radio_buttons(Enum& value, int max_in_line = -1) {
        // If enum has type with sizeof less than int as underlying
        // selector will shield us from corrupting memory
        int selector = static_cast<std::underlying_type_t<Enum>>(value);
        const auto values = magic_enum::enum_values<Enum>();
        int current_line_size = 0;
        for (auto v : values) {
            ImGui::RadioButton(magic_enum::enum_name(v).data(), &selector, int(v)); ImGui::SameLine();
            ++current_line_size;
            if (current_line_size == max_in_line) {
                ImGui::NewLine();
                current_line_size = 0;
            }
        }
        ImGui::NewLine();
        value = static_cast<Enum>(selector);
    }

    template<typename T, const char* const Name>
    void InputParameter(util::Parameter<T, Name>& parameter) {
        if constexpr (std::is_same_v<bool, T>) {
            ImGui::Checkbox(Name, &parameter.value);
        } else if constexpr (std::is_integral_v<T>) {
            if constexpr (std::is_same_v<int, T>) {
                ImGui::InputInt(Name, &parameter.value);
            } else {
                int proxy = static_cast<int>(parameter.value);
                ImGui::InputInt(Name, &proxy);
                parameter.value = static_cast<T>(proxy);
            }
        } else if constexpr (std::is_floating_point_v<T>) {
            if constexpr (std::is_same_v<float, T>) {
                ImGui::InputFloat(Name, &parameter.value);
            } else {
                float proxy = static_cast<float>(parameter.value);
                ImGui::InputFloat(Name, &proxy);
                parameter.value = static_cast<T>(proxy);
            }
        }
    }

    template<typename T, const char* const Name>
    void InputParameter(util::RestrainedParameter<T, Name>& parameter) {
        if constexpr (std::is_same_v<bool, T>) {
            ImGui::Checkbox(Name, &parameter.value);
        } else if constexpr (std::is_integral_v<T>) {
            if constexpr (std::is_same_v<int, T>) {
                ImGui::SliderInt(Name, &parameter.value, parameter.min, parameter.max);
            } else {
                int proxy = static_cast<int>(parameter.value);
                ImGui::SliderInt(Name, &parameter.value, parameter.min, parameter.max);
                parameter.value = static_cast<T>(proxy);
            }
        } else if constexpr (std::is_floating_point_v<T>) {
            if constexpr (std::is_same_v<float, T>) {
                ImGui::SliderFloat(Name, &parameter.value, parameter.min, parameter.max);
            } else {
                float proxy = static_cast<float>(parameter.value);
                ImGui::SliderFloat(Name, &parameter.value, parameter.min, parameter.max);
                parameter.value = static_cast<T>(proxy);
            }
        }
    }

    template<typename T>
    void InputParameters(T& parameters) {
        boost::pfr::for_each_field(parameters, [&]<typename U>(U& field, size_t) {
            InputParameter(field);
        });
    }

} }
