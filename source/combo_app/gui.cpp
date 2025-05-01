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
    visual::imgui::InputParameters(s_data.creation_data);
    ImGui::Text("Generation algorithm parameters");
    switch (s_data.creation_data.generation_algorithm) {
        case EMazeGenerationAlgorithm::noise: {
            auto& params = s_data.creation_data.whiteNoseGenerationParameters;
            visual::imgui::InputParameters(params);
            break;
        }
        case EMazeGenerationAlgorithm::random_dfs: {
            auto& params = s_data.creation_data.randomDfsGenerationParameters;
            visual::imgui::InputParameters(params);
            break;
        }
        case EMazeGenerationAlgorithm::binary_tree: {
            auto& params = s_data.creation_data.binaryTreeParameters;
            visual::imgui::InputParameters(params);
            break;
        }
        case EMazeGenerationAlgorithm::sidewinder: {
            auto& params = s_data.creation_data.binaryTreeParameters;
            visual::imgui::InputParameters(params);
            break;
        }
        default:
          throw std::logic_error("Unknown maze generation algorithm!");
    }
    if (ImGui::Button("Generate")) {
      s_data.creation_data.generate_maze = true;
    }
    if (ImGui::Button("Fill with chosen brush tile")) {
      s_data.creation_data.fill_maze = true;
    }
    // TODO: 'SAVE' button
    // TODO: 'LOAD' button
  }

  static void draw_visualization_progress() {
    ImGui::Text("Nodes checked: %lu", s_data.visualization_progress.nodes_checked);

    if (s_data.visualization_progress.finished) {
      if (s_data.visualization_progress.path_found) {
        ImGui::Text("Path found! Length = %lu. Cost = %.2f",
                    s_data.visualization_progress.path_length,
                    s_data.visualization_progress.path_cost);
      } else {
        ImGui::Text("Path not found.");
      }
    } else {
      ImGui::Text("Searching...");
    }

    ImGui::Text("Algorithm took %.1fms to run.", s_data.visualization_progress.processor_time_ms);
  }

  static void draw_visualization_gui() {
    visual::imgui::InputParameters(s_data.visualization_data);
    if (ImGui::Button("Pathfind")) {
      s_data.visualization_data.runPathfinding = true;
    }

    if (s_data.visualization_progress.display) {
      draw_visualization_progress();
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
