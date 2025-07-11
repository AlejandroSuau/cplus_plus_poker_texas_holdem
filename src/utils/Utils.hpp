#include <utility>

// Helper to convert from 1D Array to 2D array.

using ColRow = std::pair<std::size_t, std::size_t>;

ColRow FromIndexToColRow(std::size_t index, std::size_t cols_count) {
    return std::make_pair(index / cols_count, index % cols_count);
}

std::size_t FromColRowToIndex(ColRow colrow, std::size_t cols_count) {
    return (colrow.second * cols_count + colrow.first);
}

