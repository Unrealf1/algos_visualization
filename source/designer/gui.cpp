#include "gui.hpp"

#include <visual/imgui_inc.hpp>
#include <util/magic_enum_inc.h>
#include <ImGuiFileDialog.h>


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
    if (ImGui::CollapsingHeader("Brush")) {
        draw_enum_radio_buttons<MazeObject>(s_data.draw_object, 3);
        ImGui::InputInt("Brush size", &s_data.brush_size);
        s_data.brush_size = std::clamp(s_data.brush_size, 1, 20);
    }
}

void draw_maze_parameters() {
    if (ImGui::CollapsingHeader("Maze parameters")) {
        ImGui::InputInt("Maze width", &s_data.maze_width);
        ImGui::InputInt("Maze height", &s_data.maze_height);
    }
}

void draw_save_dialog() {
    const char* const key = "designer_draw_save_dialog";
    if (ImGui::Button("Save maze")) {
        ImGuiFileDialog::Instance()->OpenDialog(key, "Choose File", ".maze", ".", 1, nullptr, ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ConfirmOverwrite);
    }

    // display
    if (ImGuiFileDialog::Instance()->Display(key)) 
    {
        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            s_data.save_data.file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();
            s_data.save_data.do_save = true;
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }
}

void draw_load_dialog() {
    const char* const key = "designer_draw_load_dialog";
    if (ImGui::Button("Load maze")) {
        ImGuiFileDialog::Instance()->OpenDialog(key, "Choose File", ".maze", ".", 1, nullptr, ImGuiFileDialogFlags_Modal);
    }

    if (ImGuiFileDialog::Instance()->Display(key))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            s_data.load_data.file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();
            s_data.load_data.do_load = true;
        }

        ImGuiFileDialog::Instance()->Close();
    }
}

void draw_generation_window() {
    if (ImGui::CollapsingHeader("Generation")) {
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

    ImGui::Begin("Menu");

    draw_brush_window();
    draw_generation_window();
    draw_maze_parameters();
    draw_load_dialog();
    ImGui::SameLine();
    draw_save_dialog();

    ImGui::End();

    ImGui::Render();
    ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
}
