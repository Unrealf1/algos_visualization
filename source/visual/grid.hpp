#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <vector>
#include <glm_include.hpp>
#include <util.hpp>


namespace visual {

    class Grid {
    public:
        struct Cell {
            ALLEGRO_COLOR color;
        };

    private:
        size_t m_width;
        size_t m_height;

        float m_visual_width;
        float m_visual_height;
        std::vector<Cell> m_grid;

    public:
        Grid(size_t width, size_t height, float vis_width, float vis_height)
            : m_width(width)
            , m_height(height)
            , m_visual_width(vis_width)
            , m_visual_height(vis_height)
            , m_grid(width * height)
        { }

        void set_dimentions(float width, float height) {
            m_visual_width = width;
            m_visual_height = height;
        }

        void set_cell(size_t w, size_t h, const Cell& cell) {
            const auto idx = util::coords_to_idx(w, h, m_width);
            m_grid[idx] = cell;
        }
            
        void draw() {
            const float cell_width = m_visual_width / float(m_width);
            const float cell_height = m_visual_height / float(m_height);
            
            for (size_t x = 0; x < m_width; ++x) {
                const float cell_x = float(x) * cell_width;
                for (size_t y = 0; y < m_height; ++y) {
                    const float cell_y = float(y) * cell_height;
                    const auto idx = util::coords_to_idx(x, y, m_width);
                    al_draw_filled_rectangle(cell_x, cell_y, cell_x + cell_width, cell_y + cell_height, m_grid[idx].color);
                }
            }
            for (size_t x = 0; x < m_width + 1; ++x) {
                const float line_x = float(x) * cell_width;
                //al_draw_line(line_x, 0.0, line_x, m_visual_height, al_map_rgb(100, 100, 200), 2.0f);

                for (size_t y = 0; y < m_height + 1; ++y) {
                    const float line_y = float(y) * cell_height;
                    //al_draw_line(0.0, line_y, m_visual_width, line_y, al_map_rgb(100, 100, 200), 2.0f);
                    
                }
            }
        }
    };
}

