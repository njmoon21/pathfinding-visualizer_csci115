#include <iostream>
#include <utility>
#include <vector>

struct Pair {
    int row, col;
};


struct Grid {
    int width, height, start_row, start_col, goal_row, goal_col;

    std::vector<char> cells;

    // Expose helper functions
    bool inBounds(int row, int col) {
        return row >= 0 &&
               row < height &&
               col >= 0 &&
               col < width;
    }


    bool isBlocked(int row, int col) { return cells[toID(row, col)] == '#'; }

    int toID(int row, int col) { return row * width + col; }

    Pair fromID(int id) { return {id / width, id % width}; }


    std::vector<int> getNeighbors(int id) {
        std::vector<int> neighbors;
        Pair pair = fromID(id);

        // Up neighbor
        Pair upNeighbor = {pair.row - 1, pair.col};
        if (inBounds(upNeighbor.row, upNeighbor.col) &&
            !isBlocked(upNeighbor.row, upNeighbor.col)) {
            neighbors.push_back(toID(upNeighbor.row, upNeighbor.col));
        }

        // Down neighbor
        Pair downNeighbor = {pair.row + 1, pair.col};
        if (inBounds(downNeighbor.row, downNeighbor.col) &&
            !isBlocked(downNeighbor.row, downNeighbor.col)) {
            neighbors.push_back(toID(downNeighbor.row, downNeighbor.col));
        }

        // Left neighbor
        Pair leftNeighbor = {pair.row, pair.col - 1};
        if (inBounds(leftNeighbor.row, leftNeighbor.col) &&
            !isBlocked(leftNeighbor.row, leftNeighbor.col)) {
            neighbors.push_back(toID(leftNeighbor.row, leftNeighbor.col));
        }

        // Right neighbor
        Pair rightNeighbor = {pair.row, pair.col + 1};
        if (inBounds(rightNeighbor.row, rightNeighbor.col) &&
            !isBlocked(rightNeighbor.row, rightNeighbor.col)) {
            neighbors.push_back(toID(rightNeighbor.row, rightNeighbor.col));
        }

        return neighbors;
    }
};


int main() {


    return 0;
}