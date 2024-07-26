#pragma once

#include <cstdlib>
#include <ctime>
#include <random>
#include <vector>

class Game {
public:
    enum CellType {
        CELL_0,
        CELL_1,
        CELL_2,
        CELL_3,
        CELL_4,
        CELL_5,
        CELL_6,
        CELL_7,
        CELL_8,
        CELL_MINE,
        CELL_TYPES,
    };

    struct Measurements {
        int cell_size = 0;
        int cell_offset = 0;
        int grid_line_length = 0;
        int grid_line_width = 0;
        int grid_x_offset = 0;
        int grid_y_offset = 0;
        int grid_width = 0;
        int grid_height = 0;
    };

    struct GridCell {
        CellType type = CELL_0;
        bool flagged = false;
        bool revealed = false;
    };

private:
    struct GridCoords {
        int x = -1;
        int y = -1;
        bool inside = false;
    };

    typedef std::vector<std::vector<GridCell>> grid;

    const int m_rows;
    const int m_columns;
    const int m_total_mines;
    int m_unrevealed_count;
    grid m_grid{};
    int m_flagged_mines = 0;
    time_t m_start_time = 0;
    bool m_over = false;
    bool m_won = false;
    Measurements m_measurements{};
    std::mt19937_64 m_random_generator_engine = std::mt19937_64(std::random_device{}());
    std::uniform_int_distribution<char> m_random_generator;

public:
    Game(const int rows, const int columns, const int mines_count, const int window_width, const int window_height) :
        m_rows(rows),
        m_columns(columns),
        m_total_mines(mines_count),
        m_unrevealed_count(rows * columns),
        m_grid(columns, std::vector(rows, GridCell{})) {
        const float grid_ratio = static_cast<float>(m_columns) / m_rows;
        const float window_ratio = static_cast<float>(window_width) / window_height;

        const int limitant_grid_side = grid_ratio > window_ratio ? m_columns : m_rows;
        const int limitant_window_side = grid_ratio > window_ratio ? window_width : window_height;
        const int cell_size = limitant_window_side * 0.975 / limitant_grid_side;

        const int grid_line_length = cell_size * 0.65;
        const int grid_line_width = cell_size * 0.03;
        const int grid_width = cell_size * m_columns + grid_line_width;
        const int grid_x_offset = (window_width - grid_width) / 2;
        const int grid_height = cell_size * m_rows + grid_line_width;
        const int grid_y_offset = (window_height - grid_height) / 2;

        const int cell_offset = grid_line_width / 2;

        m_measurements.cell_size = cell_size;
        m_measurements.cell_offset = cell_offset;
        m_measurements.grid_line_length = grid_line_length;
        m_measurements.grid_line_width = grid_line_width;
        m_measurements.grid_x_offset = grid_x_offset;
        m_measurements.grid_y_offset = grid_y_offset;
        m_measurements.grid_width = grid_width;
        m_measurements.grid_height = grid_height;
    }

    ~Game() = default;

    [[nodiscard]] int get_rows() const {
        return m_rows;
    }

    [[nodiscard]] int get_columns() const {
        return m_columns;
    }

    [[nodiscard]] int get_total_mines() const {
        return m_total_mines;
    }

    [[nodiscard]] int get_flagged_mines() const {
        return m_flagged_mines;
    }

    [[nodiscard]] time_t get_start_time() const {
        return m_start_time;
    }

    [[nodiscard]] const Measurements &get_measurements() const {
        return m_measurements;
    }

    [[nodiscard]] const GridCell &get_grid_cell(const int x, const int y) const {
        return m_grid[x][y];
    }

    [[nodiscard]] bool is_over() const {
        return m_over;
    }

    [[nodiscard]] bool has_won() const {
        return m_won;
    }

    void place_grid_mines(const int x, const int y) {
        const time_t now = time(nullptr);
        m_start_time = now;

        srand(now);

        int placed_mines = 0;
        while (placed_mines < m_total_mines) {
            const int nx = random_between(0, m_columns - 1);
            const int ny = random_between(0, m_rows - 1);

            // Mines count at (x, y) must be 0
            if (nx >= x - 1 && nx <= x + 1 && ny >= y - 1 && ny <= y + 1)
                continue;

            if (m_grid[nx][ny].type == CELL_0) {
                m_grid[nx][ny].type = CELL_MINE;
                placed_mines++;
            }
        }

        // Count surrounding mines
        for (int i = 0; i < m_columns; i++) {
            for (int j = 0; j < m_rows; j++) {
                const CellType cell = m_grid[i][j].type;
                if (cell == CELL_MINE)
                    continue;

                const CellType surrounding = count_surrounding_mines(i, j);
                m_grid[i][j].type = surrounding;
            }
        }
    }

    [[nodiscard]] GridCoords calculate_grid_cell(const int click_x, const int click_y) const {
        const int cell_size = m_measurements.cell_size;
        const int grid_x_offset = m_measurements.grid_x_offset;
        const int grid_y_offset = m_measurements.grid_y_offset;

        bool inside = true;
        float x = static_cast<float>(click_x - grid_x_offset) / cell_size;
        float y = static_cast<float>(click_y - grid_y_offset) / cell_size;
        if (x < 0 || x > m_columns - 0.01f || y < 0 || y > m_rows - 0.01f) {
            x = -1;
            y = -1;
            inside = false;
        }

        return {static_cast<int>(x), static_cast<int>(y), inside};
    }

    void toggle_cell_flag(const int x, const int y) {
        const auto [type, flagged, revealed] = m_grid[x][y];
        if (!revealed) {
            m_grid[x][y].flagged = !flagged;

            if (!flagged)
                m_flagged_mines++;
            else
                m_flagged_mines--;

            return;
        }

        if (type < CELL_1 || type > CELL_8)
            return;

        GridCoords unrevealed[9];
        int unrevealed_count = 0;
        get_surrounding_unrevealed(x, y, unrevealed, &unrevealed_count);

        if (unrevealed_count != type - CELL_0)
            return;

        for (int i = 0; i < unrevealed_count; i++) {
            const int nx = unrevealed[i].x;
            const int ny = unrevealed[i].y;

            if (m_grid[nx][ny].flagged)
                continue;

            m_grid[nx][ny].flagged = true;
            m_flagged_mines++;
        }
    }

    void reveal_cell(const int x, const int y) {
        const auto [type, flagged, revealed] = m_grid[x][y];

        if (flagged)
            return;

        if (type == CELL_MINE) {
            m_grid[x][y].revealed = true;
            m_over = true;
            m_won = false;
            return;
        }

        GridCoords revealed_cells[9] = {{x, y}};
        int revealed_cells_count = 1;

        if (revealed) {
            if (type < CELL_1 || type > CELL_8)
                return;

            const int flagged_count = count_surrounding_flagged(x, y);

            if (flagged_count != type - CELL_0)
                return;

            if (reveal_non_flagged(x, y, revealed_cells, &revealed_cells_count)) {
                m_over = true;
                m_won = false;
                return;
            }
        } else {
            m_grid[x][y].revealed = true;
            m_unrevealed_count--;
        }

        for (int i = 0; i < revealed_cells_count; i++) {
            const int nx = revealed_cells[i].x;
            const int ny = revealed_cells[i].y;

            if (m_grid[nx][ny].type != CELL_0)
                continue;

            reveal_cell_border(nx, ny);
            reveal_cells_dfs(nx, ny);
        }

        if (m_total_mines == m_unrevealed_count) {
            m_over = true;
            m_won = true;
            flag_all_unrevealed();
        }
    }

private:
    int random_between(const int min, const int max) {
        return m_random_generator(m_random_generator_engine) % (max - min + 1) + min;
    }

    [[nodiscard]] CellType count_surrounding_mines(const int x, const int y) const {
        int surrounding = CELL_0;

        for (int i = -1; i <= 1; i++) {
            const int nx = x + i;

            if (nx < 0 || nx > m_columns - 1)
                continue;

            for (int j = -1; j <= 1; j++) {
                const int ny = y + j;

                if (ny < 0 || ny > m_rows - 1)
                    continue;

                if (m_grid[nx][ny].type == CELL_MINE)
                    surrounding++;
            }
        }

        return static_cast<CellType>(surrounding);
    }

    void get_surrounding_unrevealed(const int x, const int y, GridCoords *coords, int *counter) const {
        for (int i = -1; i <= 1; i++) {
            const int nx = x + i;

            if (nx < 0 || nx > m_columns - 1)
                continue;

            for (int j = -1; j <= 1; j++) {
                const int ny = y + j;

                if (ny < 0 || ny > m_rows - 1 || m_grid[nx][ny].revealed)
                    continue;

                coords[*counter] = {nx, ny};
                *counter = *counter + 1;
            }
        }
    }

    void flag_all_unrevealed() {
        for (int i = 0; i < m_columns; i++) {
            for (int j = 0; j < m_rows; j++) {
                if (m_grid[i][j].revealed || m_grid[i][j].flagged)
                    continue;

                m_grid[i][j].flagged = true;
                m_flagged_mines++;
            }
        }
    }

    bool reveal_non_flagged(const int x, const int y, GridCoords *coords, int *counter) {
        bool revealed_mine = false;

        for (int i = -1; i <= 1; i++) {
            const int nx = x + i;

            if (nx < 0 || nx > m_columns - 1)
                continue;

            for (int j = -1; j <= 1; j++) {
                const int ny = y + j;

                if (ny < 0 || ny > m_rows - 1)
                    continue;

                const auto [type, flagged, revealed] = m_grid[nx][ny];

                if (revealed || flagged)
                    continue;

                coords[*counter] = {nx, ny};
                *counter = *counter + 1;

                if (type != CELL_MINE)
                    m_unrevealed_count--;

                m_grid[nx][ny].revealed = true;

                if (type == CELL_MINE) {
                    revealed_mine = true;
                    break;
                }
            }

            if (revealed_mine)
                break;
        }

        return revealed_mine;
    }

    [[nodiscard]] int count_surrounding_flagged(const int x, const int y) const {
        int flagged = 0;
        for (int i = -1; i <= 1; i++) {
            const int nx = x + i;

            if (nx < 0 || nx > m_columns - 1)
                continue;

            for (int j = -1; j <= 1; j++) {
                const int ny = y + j;

                if (ny < 0 || ny > m_rows - 1 || m_grid[nx][ny].revealed || !m_grid[nx][ny].flagged)
                    continue;

                flagged++;
            }
        }

        return flagged;
    }

    [[nodiscard]] GridCoords get_surrounding_empty(const int x, const int y) const {
        GridCoords coords{};

        for (int i = -1; i <= 1; i++) {
            const int nx = x + i;

            if (nx < 0 || nx > m_columns - 1)
                continue;

            bool found = false;

            for (int j = -1; j <= 1; j++) {
                const int ny = y + j;

                if (ny < 0 || ny > m_rows - 1 || m_grid[nx][ny].type != CELL_0 || m_grid[nx][ny].revealed)
                    continue;

                coords.x = nx;
                coords.y = ny;
                found = true;
                break;
            }

            if (found)
                break;
        }

        return coords;
    }

    void reveal_cells_dfs(const int x, const int y) { // NOLINT(*-no-recursion)
        for (int i = -1; i <= 1; i++) {
            const int nx = x + i;

            if (nx < 0 || nx > m_columns - 1)
                continue;

            for (int j = -1; j <= 1; j++) {
                // Do not check corners
                if (abs(x) == 1 && abs(y) == 1)
                    continue;

                const int ny = y + j;

                if (ny < 0 || ny > m_rows - 1 || m_grid[nx][ny].type != CELL_0 || m_grid[nx][ny].revealed)
                    continue;

                m_grid[nx][ny].revealed = true;
                m_unrevealed_count--;
                reveal_cell_border(nx, ny);
                reveal_cells_dfs(nx, ny);
            }
        }
    }

    void reveal_cell_border(const int x, const int y) {
        for (int i = -1; i <= 1; i++) {
            const int bx = x + i;

            if (bx < 0 || bx > m_columns - 1)
                continue;

            for (int j = -1; j <= 1; j++) {
                const int by = y + j;
                const CellType cell_type = m_grid[bx][by].type;

                if (by < 0 || by > m_rows - 1 || m_grid[bx][by].revealed || cell_type == CELL_0 || cell_type ==
                    CELL_MINE)
                    continue;

                m_grid[bx][by].revealed = true;
                m_unrevealed_count--;
            }
        }
    }
};