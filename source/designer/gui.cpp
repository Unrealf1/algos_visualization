#include "gui.hpp"

#include <visual/imgui_inc.hpp>
#include <util/magic_enum_inc.h>


static GuiData s_data{};


GuiData& get_gui_data() {
    return s_data;
}

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

void draw_brush_window() {
    if (ImGui::Begin("Brush", nullptr, ImGuiWindowFlags_MenuBar)) {
        draw_enum_radio_buttons<MazeObject>(s_data.draw_object, 3);

        ImGui::InputInt("Maze width", &s_data.maze_width);
        ImGui::InputInt("Maze height", &s_data.maze_height);
    }
    ImGui::End();
}

void draw_generation_window() {
    if (ImGui::Begin("Generation", nullptr, ImGuiWindowFlags_MenuBar)) {
        if (ImGui::Button("Fill with chosen brush tile")) {
            s_data.fill_maze = true;
        }

        draw_enum_radio_buttons<EMazeGenerationAlgorithm>(s_data.generation_algorithm, 3);

        if (ImGui::Button("Generate")) {
            s_data.generate_maze = true;
        }
    }
}

void draw_gui() {
    ImGui_ImplAllegro5_NewFrame();
    ImGui::NewFrame();

    draw_brush_window();
    draw_generation_window();
    ImGui::End();

    ImGui::Render();
    ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
}