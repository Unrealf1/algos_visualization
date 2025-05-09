#include <gui.hpp>

#include <visual/imgui_inc.hpp>
#include <ImGuiFileDialog.h>
#include <visual/imgui_widgets.hpp>
#include <util/random_utils.hpp>


namespace combo_app_gui {
  static Data s_data;

  Data& get_data() {
    return s_data;
  }

  static void draw_creation_gui() {
    auto& data = s_data.creation_data;

    int dims[] = {data.maze_width, data.maze_height};
    ImGui::InputInt2("Dimentions", dims);
    data.maze_width = std::max(dims[0], 1);
    data.maze_height = std::max(dims[1], 1);

    ImGui::Separator();

    ImGui::Text("Brush");
    visual::imgui::draw_enum_radio_buttons(data.draw_object.value, 2);
    ImGui::InputInt("Brush size", &data.brush_size.value);
    if (ImGui::Button("Fill with chosen tile")) {
      s_data.creation_data.fill_maze = true;
    }

    ImGui::Separator();

    ImGui::Text("Generation algorithm");

    int seedCopy = static_cast<int>(data.fixed_seed);
    ImGui::PushItemWidth(90);
    ImGui::InputInt("Seed", &seedCopy, 0);
    data.fixed_seed = static_cast<size_t>(seedCopy);
    ImGui::SameLine();
    if (ImGui::Button("Randomize")) {
      auto& rengine = get_rengine();
      s_data.creation_data.fixed_seed.value = std::uniform_int_distribution<size_t>(1)(rengine);
    }

    visual::imgui::draw_enum_radio_buttons(data.generation_algorithm.value, 2);

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
    auto& slowCostParam = s_data.creation_data.slow_tile_cost;
    ImGui::PushItemWidth(100);
    ImGui::SliderFloat("Slow tile cost", &slowCostParam.value, slowCostParam.min, slowCostParam.max);
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
    ImGui::Checkbox("Show grid", &s_data.creation_data.draw_grid.value);

    ImGui::Separator();

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
