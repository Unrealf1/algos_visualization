#pragma once

#include <vector>
#include <map>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <util.hpp>
#include <maze.hpp>


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
            ALLEGRO_COLOR last_used_color;
        };

        const static ColorMap s_default_color_map;
        const static Style s_default_style;

        struct Cell {
            ALLEGRO_COLOR color;
        };

    private:
        size_t m_width;
        size_t m_height;

        float m_visual_width;
        float m_visual_height;
        std::vector<Cell> m_grid;
        Style m_style;

    public:

        Grid(const Maze& maze, float vis_width, float vis_height, Style style = s_default_style);

        const Style& style();

        void set_dimentions(float width, float height);
        Cell& get_cell(size_t w, size_t h);
        void draw();
    };
}

