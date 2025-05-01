#include <gui.hpp>

#include <visual/imgui_inc.hpp>
#include <ImGuiFileDialog.h>
#include <visual/imgui_widgets.hpp>


namespace combo_app_gui {
  static Data s_data;

  Data& get_data() {
    return s_data;
  }

  static void draw_creation_gui() {
    visual::imgui::InputParameters(s_data.m_creation_data);
    ImGui::Text("Generation algorithm parameters");
    switch (s_data.m_creation_data.generation_algorithm) {
        case EMazeGenerationAlgorithm::noise: {
            auto& params = s_data.m_creation_data.whiteNoseGenerationParameters;
            visual::imgui::InputParameters(params);
            break;
        }
        case EMazeGenerationAlgorithm::random_dfs: {
            auto& params = s_data.m_creation_data.randomDfsGenerationParameters;
            visual::imgui::InputParameters(params);
            break;
        }
        case EMazeGenerationAlgorithm::binary_tree: {
            auto& params = s_data.m_creation_data.binaryTreeParameters;
            visual::imgui::InputParameters(params);
            break;
        }
        case EMazeGenerationAlgorithm::sidewinder: {
            auto& params = s_data.m_creation_data.binaryTreeParameters;
            visual::imgui::InputParameters(params);
            break;
        }
        default:
          throw std::logic_error("Unknown maze generation algorithm!");
    }
    if (ImGui::Button("Generate")) {
      s_data.m_creation_data.generate_maze = true;
    }
    if (ImGui::Button("Fill with chosen brush tile")) {
      s_data.m_creation_data.fill_maze = true;
    }
    // TODO: 'SAVE' button
    // TODO: 'LOAD' button
  }

  static void draw_visualization_gui() {
    visual::imgui::InputParameters(s_data.m_visualization_data);
    if (ImGui::Button("Pathfind")) {
      s_data.m_visualization_data.runPathfinding = true;
    }
  }

  void draw() {
    ImGui_ImplAllegro5_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::Begin("Menu");

    visual::imgui::draw_enum_radio_buttons(s_data.m_mode);
    if (s_data.m_mode == AppMode::Creation) {
      draw_creation_gui();
    } else {
      draw_visualization_gui();
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
    
  }
}
