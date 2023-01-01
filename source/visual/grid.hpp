#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <vector>


namespace visual {

    class Grid {
    private:
        struct Cell {
            ALLEGRO_COLOR color;
        };

        const int m_width;
        const int m_height;
        std::vector<Cell> m_grid;
    public:
        Grid(int width, int height)
            : m_width(width)
            , m_height(height)
            , m_grid(size_t(width * height))
        { }

        void draw() {
            al_draw_line(0.0f, 0.0f, 10.0f, 10.0f, al_map_rgb(100, 100, 200), 2.0f);
        }
    };
}

