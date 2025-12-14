#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>


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


enum class Algorithm {
    BFS, DIJKSTRA, A_STAR, ALL
};


void mapLoading(std::string& path, Grid& grid) {
    std::ifstream file(path);

    std::string line;
    std::vector<std::string> gridLines;

    // Reading map format configuration
    while (std::getline(file, line)) {
        if (line.empty() == true)
            continue;

        std::stringstream ss(line);
        std::string config;
        ss >> config;

        if (config == "WIDTH")
            ss >> grid.width;
        else if (config == "HEIGHT")
            ss >> grid.height;
        else if (config == "START")
            ss >> grid.start_row >> grid.start_col;
        else if (config == "GOAL")
            ss >> grid.goal_row >> grid.goal_col;
        else
            gridLines.push_back(line);
    }

    // Loading the grid from the file
    grid.cells.resize(grid.width * grid.height);

    for (int row = 0; row < grid.height; row++) {
        for (int col = 0; col < grid.width; col++) {
            char ch = gridLines[row][col];
            grid.cells[grid.toID(row, col)] = ch;
        }
    }
}


struct argumentConfigs {
    bool map_mode_ = false, generation_mode_ = false;
    std::string map_path_;
    int width_ = 0, height_ = 0;
    float density_ = 0;
    Algorithm algorithm_;
};


bool argumentParser(int argc, char* argv[], argumentConfigs& configuration) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        // If argv[i] == "--map"
        if (arg == "--map") {
            if (configuration.generation_mode_ == true) {
                std::cout << "Error: Generation mode is already on.\n";
                return false;
            }
            if (i + 1 >= argc) {
                std::cout << "Error: Requires a path containing desired file\n";
                return false;
            }

            configuration.map_mode_ = true;
            configuration.map_path_ = argv[i + 1];
        }


        /*

    // If argv[i] == "--generate"
        else if (arg == "--generate") {
            if (configuration.map_mode_ == true) {
                std::cout << "Error: Map mode is already on.\n";
                return false;
            }
            configuration.generation_mode_ = true;
        }

         */



        // If argv[i] == "--algo"
        else if (arg == "--algo") {
            if (i + 1 >= argc) {
                std::cout << "--algo requires a value <std::string>\n";
                return false;
            }

            std::string algoValue = argv[i + 1];
            // {bfs|dijkstra|astar|all}
            if (algoValue == "bfs")
                configuration.algorithm_ = Algorithm::BFS;
            else if (algoValue == "dijkstra")
                configuration.algorithm_ = Algorithm::DIJKSTRA;
            else if (algoValue == "astar")
                configuration.algorithm_ = Algorithm::A_STAR;
            else if (algoValue == "all")
                configuration.algorithm_ = Algorithm::ALL;
            else {
                std::cout << "Invalid input, must explicitly choose the following: {bfs|dijkstra|astar|all}\n";
                return false;
            }
        }


        // If argv[i] == "width"
        else if (arg == "--width") {
            if (i + 1 >= argc) {
                std::cout << "--width requires a value <int>\n";
                return false;
            }
            configuration.width_ = std::stoi(argv[i + 1]);
        }


        // If argv[i] == "height"
        else if (arg == "--height") {
            if (i + 1 >= argc) {
                std::cout << "--height requires a value <int>\n";
                return false;
            }
            configuration.height_ = std::stoi(argv[i + 1]);
        }


        // If argv[i] == "density"
        else if (arg == "--density") {
            if (i + 1 >= argc) {
                std::cout << "--density requires a value <float>\n";
                return false;
            }
            configuration.density_ = std::stof(argv[i + 1]);
        }

        // Invalid argument
        else {
            std::cout << "Unknown argument: " << arg << std::endl;
            return false;
        }
    }

    // Identifying specific invalid cases
    // Can't have both map and generation mode off
    if (configuration.map_mode_ == false && configuration.generation_mode_ == false) {
        std::cout << "Must have --map or --generation (NOT BOTH)\n";
        return false;
    }

    // If generation mode is on, all width/height/density values must be present/valid
    if (configuration.generation_mode_ == true) {
        if (configuration.width_ <= 0) {
            std::cout << "Invalid width value (must be greater than 0)\n";
            return false;
        }
        if (configuration.height_ <= 0) {
            std::cout << "Invalid height value (must be greater than 0)\n";
            return false;
        }
        if (configuration.density_ < 0 || configuration.density_ >= 1) {
            std::cout << "Invalid density value (valid range: 0 < value <= 1)\n";
            return false;
        }
    }

    // Everything went through, therefore the parsing has completed with no issues
    return true;
}

// Algorithm implementations
struct algorithmSearch {
    bool found = false;
    std::vector<bool> visited;
    std::vector<int> path; // Each int is an ID that a cell represents

    // Step 7 is now optional
    // int visited_amount_, path_len_, cost_, time;
};


algorithmSearch BFS(Grid& grid) {
    algorithmSearch result;

    int n = grid.width * grid.height; // represents the amount of cells in the grid
    result.visited.assign(n, false); // fill every cell with false, since at the start we haven't visited anything
    std::vector<int> parents(n, -1); // Each cell containing a parentID, -1 is a sentinel value

    int startId = grid.toID(grid.start_row, grid.start_col);
    int goalId = grid.toID(grid.goal_row, grid.goal_col);


    // "BFS uses a queue and guarantees shortest paths on unit grids"
    // Start
    std::queue<int> queue;
    queue.push(startId);
    result.visited[startId] = true;


    while (!queue.empty()) {
        int curr = queue.front();
        queue.pop();

        if (curr == goalId) {
            result.found = true;
            break;
        }

        for (int neighbor : grid.getNeighbors(curr)) {
            if (result.visited[neighbor] == false) {
                result.visited[neighbor] = true;
                parents[neighbor] = curr;
                queue.push(neighbor);
            }
        }
    }


    if (result.found == true) {
        int curr = goalId;
        while (curr != -1) {
            result.path.push_back(curr);
            curr = parents[curr];
        }
        std::reverse(result.path.begin(), result.path.end());
    }


    return result;
}


algorithmSearch Dijkstras(Grid& grid) {

}

algorithmSearch Astar(Grid& grid) {

}


void print(Grid& grid, algorithmSearch& result, std::string& algorithm) {
    std::vector<char> visualGrid = grid.cells;


    for (int i = 0; i < result.visited.size(); i++) {
        if (result.visited[i] && visualGrid[i] == '.')
            visualGrid[i] = '+';
    }

    if (result.found == true) {
        for (int cellId : result.path) {
            if (visualGrid[cellId] != '#')
                visualGrid[cellId] = '*';
        }
    }


    int startId = grid.toID(grid.start_row, grid.start_col);
    int goalId = grid.toID(grid.goal_row, grid.goal_col);

    visualGrid[startId] = 'S';
    visualGrid[goalId] = 'G';


    // Final print
    std::cout << "=== " << algorithm << " ===\n";
    for (int row = 0; row < grid.height; row++) {
        for (int col = 0; col < grid.width; col++) {
            std::cout << visualGrid[grid.toID(row, col)];
        }
        std::cout << "\n";
    }
    bool pathFound = result.found;
    std::cout << "Found: " << pathFound << "\n";
}


int main(int argc, char* argv[]) {
    if (argc != 5 && argc != 10) {
        std::cout << "Invalid argument count. Please try again\n";
        return 1;
    }

    argumentConfigs configuration;
    bool parsingSuccessful = argumentParser(argc, argv, configuration);
    if (parsingSuccessful == false)
        return 1;

    // TODO

    return 0;
}