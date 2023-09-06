#include "grid.hpp"

namespace visual {

const Grid::ColorMap Grid::s_default_color_map = {
    { MazeObject::wall, al_map_rgb(0, 0, 0) },
    { MazeObject::space, al_map_rgb(255, 255, 255) },
    { MazeObject::start, al_map_rgb(190, 160, 80) },
    { MazeObject::finish, al_map_rgb(20, 200, 60) }
};

const Grid::Style Grid::s_default_style = {
    .color_map = s_default_color_map,
    .draw_lattice = false,
    .lattice_color = al_map_rgb(100, 100, 200),
    .path_color = al_map_rgb(182, 102, 142),
    .used_color = al_map_rgb(0, 200, 200),
    .last_used_color = al_map_rgb(200, 0, 0)
};

Grid::Grid(const Maze& maze, float vis_width, float vis_height, Style style)
    : m_width(maze.width)
    , m_height(maze.height)
    , m_visual_width(vis_width)
    , m_visual_height(vis_height)
    , m_grid(maze.items.size())
    , m_style(std::move(style))
{ 
    for (size_t i = 0; i < m_grid.size(); ++i) {
        m_grid[i].color = m_style.color_map[maze.items[i]];
    }
}

const Grid::Style& Grid::style() {
    return m_style;
}

void Grid::set_dimentions(float width, float height) {
    m_visual_width = width;
    m_visual_height = height;
}

Grid::Cell& Grid::get_cell(size_t w, size_t h) {
    const auto idx = util::coords_to_idx(w, h, m_width);
    return m_grid[idx];
}

void Grid::draw() {
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

    if (m_style.draw_lattice) {
        for (size_t x = 0; x < m_width + 1; ++x) {
            const float line_x = float(x) * cell_width;
            al_draw_line(line_x, 0.0, line_x, m_visual_height, m_style.lattice_color, 2);
            for (size_t y = 0; y < m_height + 1; ++y) {
                const float line_y = float(y) * cell_height;
                al_draw_line(0.0, line_y, m_visual_width, line_y, m_style.lattice_color, 2);
            }
        }
    }
}

}
