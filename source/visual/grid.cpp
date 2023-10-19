#include "grid.hpp"
#include <util/util.hpp>
#include <algorithm>

namespace visual {

const Grid::ColorMap Grid::s_default_color_map = {
    { MazeObject::wall, al_map_rgb(0, 0, 0) },
    { MazeObject::space, al_map_rgb(255, 255, 255) },
    { MazeObject::start, al_map_rgb(190, 160, 80) },
    { MazeObject::finish, al_map_rgb(20, 200, 60) },
    { MazeObject::slow, al_map_rgb(155, 100, 0) }
};

const Grid::Style Grid::s_default_style = {
    .color_map = s_default_color_map,
    .draw_lattice = false,
    .lattice_color = al_map_rgb(100, 100, 200),
    .path_color = al_map_rgb(182, 102, 142),
    .used_color = al_map_rgb(0, 200, 200),
    .discovered_color = al_map_rgb(0, 100, 100),
    .last_used_color = al_map_rgb(200, 0, 0)
};

Grid::Grid(const Maze& maze, float vis_width, float vis_height, Style style)
    : m_grid(maze.items.size())
    , m_width(maze.width)
    , m_height(maze.height)
    , m_bitmap(int(vis_height), int(vis_width))
    , m_visual_screen_width(vis_width)
    , m_visual_screen_height(vis_height)
    , m_style(std::move(style))
{ 
    for (size_t i = 0; i < m_grid.size(); ++i) {
        m_grid[i].color = m_style.color_map[maze.items[i]];
    }
    recalculate_visual_parameters();
}

void Grid::recalculate_visual_parameters() {
    const float cell_width = m_visual_screen_width / float(m_width);
    const float cell_height = m_visual_screen_height / float(m_height);
    m_visual_cell_dimention = std::min(cell_width, cell_height);
    m_visual_grid_width = m_visual_cell_dimention * float(m_width);
    m_visual_grid_height = m_visual_cell_dimention * float(m_height);
    m_visual_offset_x = (m_visual_screen_width - m_visual_grid_width) / 2;
    m_visual_offset_y = (m_visual_screen_height - m_visual_grid_height) / 2;
    m_need_full_redraw = true;

    const auto w = int(m_visual_screen_width);
    const auto h = int(m_visual_screen_height);
    if (m_bitmap.height() != h || m_bitmap.width() != w) {
        m_bitmap = visual::Bitmap(w, h);
    }
}

void Grid::update(const Maze& maze) {
    auto temp = Grid(maze, m_visual_screen_width, m_visual_screen_height);
    m_grid = std::move(temp.m_grid);
    m_width = temp.m_width;
    m_height = temp.m_height;
    recalculate_visual_parameters();
}

const Grid::Style& Grid::style() const {
    return m_style;
}

Grid::Style& Grid::style() {
    return m_style;
}

void Grid::set_dimentions(float width, float height) {
    m_visual_screen_width = width;
    m_visual_screen_height = height;
    recalculate_visual_parameters();
}

std::pair<float, float> Grid::get_dimentions() const {
    return {m_visual_screen_width, m_visual_screen_height};
}

const Grid::Cell& Grid::get_cell(size_t w, size_t h) const {
    const auto idx = util::coords_to_idx(w, h, m_width);
    return m_grid[idx];
}

void Grid::set_cell(size_t w, size_t h, Cell new_value) {
    const auto idx = util::coords_to_idx(w, h, m_width);
    m_grid[idx] = new_value;
    m_dirty_cells.insert(idx);
}

void Grid::draw(ALLEGRO_DISPLAY* display) {
    al_set_target_bitmap(m_bitmap.get_raw());
    if (m_need_full_redraw) {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        for (size_t x = 0; x < m_width; ++x) {
            const float cell_x = m_visual_offset_x + float(x) * m_visual_cell_dimention;
            for (size_t y = 0; y < m_height; ++y) {
                const float cell_y = m_visual_offset_y + float(y) * m_visual_cell_dimention;
                const auto idx = util::coords_to_idx(x, y, m_width);
                al_draw_filled_rectangle(cell_x, cell_y, cell_x + m_visual_cell_dimention, cell_y + m_visual_cell_dimention, m_grid[idx].color);
            }
        }
    } else {
        for (auto idx : m_dirty_cells) {
            const auto [x, y] = util::idx_to_coords(idx, m_width);
            const float cell_x = m_visual_offset_x + float(x) * m_visual_cell_dimention;
            const float cell_y = m_visual_offset_y + float(y) * m_visual_cell_dimention;
            al_draw_filled_rectangle(cell_x, cell_y, cell_x + m_visual_cell_dimention, cell_y + m_visual_cell_dimention, m_grid[idx].color);
        }
    }

    if (m_style.draw_lattice && (m_need_full_redraw || !m_dirty_cells.empty())) {
        for (size_t x = 0; x < m_width + 1; ++x) {
            const float line_x = m_visual_offset_x + float(x) * m_visual_cell_dimention;
            al_draw_line(line_x, m_visual_offset_y, line_x, m_visual_screen_height - m_visual_offset_y, m_style.lattice_color, 2);
            for (size_t y = 0; y < m_height + 1; ++y) {
                const float line_y = m_visual_offset_y + float(y) * m_visual_cell_dimention;
                al_draw_line(m_visual_offset_x, line_y, m_visual_screen_width - m_visual_offset_x, line_y, m_style.lattice_color, 2);
            }
        }
    }
    m_dirty_cells.clear();
    m_need_full_redraw = false;
    al_set_target_bitmap(al_get_backbuffer(display));
    al_draw_bitmap(m_bitmap.get_raw(), 0, 0, 0);
}

std::pair<size_t, size_t> Grid::get_cell_under_cursor_coords(int mouse_x, int mouse_y) const {
    const auto x = std::min(size_t((float(mouse_x) - m_visual_offset_x) * float(m_width) / m_visual_grid_width), m_width - 1);
    const auto y = std::min(size_t((float(mouse_y) - m_visual_offset_y) * float(m_height) / m_visual_grid_height), m_height - 1);
    return { x, y };
}

}
