#pragma once

#include <vector>
#include <set>
#include <map>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <util/util.hpp>
#include <maze/maze.hpp>
#include <visual/allegro_util.hpp>


namespace visual {
    class Grid {
    public:
        using ColorMap = std::map<MazeObject, ALLEGRO_COLOR>;

        struct Style {
            ColorMap color_map;
            bool draw_lattice;
            ALLEGRO_COLOR lattice_color;
            ALLEGRO_COLOR path_color;
            ALLEGRO_COLOR used_color;
            ALLEGRO_COLOR discovered_color;
            ALLEGRO_COLOR last_used_color;
        };

        const static ColorMap s_default_color_map;
        const static Style s_default_style;

        struct Cell {
            ALLEGRO_COLOR color;
        };

    private:
        std::vector<Cell> m_grid;
        size_t m_width;
        size_t m_height;

        Bitmap m_bitmap;

        std::set<size_t> m_dirty_cells;
        bool m_need_full_redraw;

        float m_visual_screen_width;
        float m_visual_screen_height;
        float m_visual_offset_x;
        float m_visual_offset_y;
        float m_visual_grid_width;
        float m_visual_grid_height;
        float m_visual_cell_dimention;

        Style m_style;

    public:

        Grid(const Maze& maze, float vis_width, float vis_height, Style style = s_default_style);

        void update(const Maze& maze);

        const Style& style() const;
        Style& style();

        void set_dimentions(float width, float height);
        std::pair<float, float> get_dimentions() const;
        const Cell& get_cell(size_t w, size_t h) const;
        void set_cell(size_t w, size_t h, Cell new_value);
        Cell* cell_under_cursor(int mouse_x, int mouse_y);
        const std::vector<Cell>& get_cells() const;
        void draw(ALLEGRO_DISPLAY* display);
        void request_full_redraw();
        std::pair<size_t, size_t> get_cell_under_cursor_coords(int mouse_x, int mouse_y) const;
    private:
        void recalculate_visual_parameters();
    };
}

