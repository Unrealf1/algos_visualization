#include "gui.hpp"

#include <visual/imgui_inc.hpp>
#include <util/magic_enum_inc.h>


static GuiData s_data{};


GuiData& get_gui_data() {
    return s_data;
}

void draw_gui() {
    ImGui_ImplAllegro5_NewFrame();
    ImGui::NewFrame();

    if (ImGui::Begin("Brush", nullptr, ImGuiWindowFlags_MenuBar)) {
        int draw_object_selector = static_cast<std::underlying_type_t<MazeObject>>(s_data.draw_object);
        for (auto obj : magic_enum::enum_values<MazeObject>()) {
            ImGui::RadioButton(magic_enum::enum_name(obj).data(), &draw_object_selector, int(obj));
        }
        s_data.draw_object = static_cast<MazeObject>(draw_object_selector);
        ImGui::InputInt("Maze width", &s_data.maze_width);
        ImGui::InputInt("Maze height", &s_data.maze_height);
    }
    ImGui::End();

    if (ImGui::Begin("Generation", nullptr, ImGuiWindowFlags_MenuBar)) {
        if (ImGui::Button("Fill with chosen tile")) {
            s_data.fill_maze = true;
        }
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
}