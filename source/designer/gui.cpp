#include "gui.hpp"

#include <visual/imgui_inc.hpp>
#include <ImGuiFileDialog.h>
#include <visual/imgui_widgets.hpp>


static GuiData s_data{};


GuiData& get_gui_data() {
    return s_data;
}

void draw_brush_window() {
    if (ImGui::CollapsingHeader("Brush")) {
        visual::imgui::draw_enum_radio_buttons<MazeObject>(s_data.draw_object, 3);
        ImGui::InputInt("Brush size", &s_data.brush_size);
        s_data.brush_size = std::clamp(s_data.brush_size, 1, 20);
    }
}

void draw_maze_parameters() {
    if (ImGui::CollapsingHeader("Maze parameters")) {
        ImGui::InputInt("Maze width", &s_data.maze_width);
        ImGui::InputInt("Maze height", &s_data.maze_height);

        s_data.maze_width = std::max(s_data.maze_width, 1);
        s_data.maze_height = std::max(s_data.maze_height, 1);
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

        visual::imgui::draw_enum_radio_buttons<EMazeGenerationAlgorithm>(s_data.generation_algorithm, 3);

        if (s_data.generation_algorithm == EMazeGenerationAlgorithm::noise) {
            visual::imgui::InputParameters(s_data.whiteNoseGenerationParameters);
        } else if (s_data.generation_algorithm == EMazeGenerationAlgorithm::random_dfs) {
            visual::imgui::InputParameters(s_data.randomDfsGenerationParameters);
        } else if (s_data.generation_algorithm == EMazeGenerationAlgorithm::binary_tree) {
            visual::imgui::InputParameters(s_data.binaryTreeParameters);
        }

        if (ImGui::Button("Generate")) {
            s_data.generate_maze = true;
        }
    }
}

void draw_visual_params() {
    if (ImGui::CollapsingHeader("visual")) {
        const auto prev = s_data.visual_parameters;
        visual::imgui::InputParameters(s_data.visual_parameters);
        s_data.update_visuals = prev != s_data.visual_parameters;
    }
}

void draw_gui() {
    ImGui_ImplAllegro5_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Menu");

    draw_brush_window();
    draw_generation_window();
    draw_maze_parameters();
    draw_visual_params();
    draw_load_dialog();
    ImGui::SameLine();
    draw_save_dialog();

    ImGui::End();

    ImGui::Render();
    ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
}
