#include <iostream>
#include <vector>

struct Pair {
    int row, col;
};


struct Grid {
    int width, height, start_row, start_col, goal_row, goal_col;

    std::vector<char> cells;

    // Expose helper functions
    bool inBounds(int row, int col) {

    }


    bool isBlocked(int row, int col) {

    }


    int toID(int row, int col) { return row * width + col; }


    Pair fromID(int id) { return {id / width, id % width}; }


    std::vector<int> getNeighbors(int id) {

    }
};


int main() {


    return 0;
}