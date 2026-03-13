#include "../include/GridMap.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

GridMap::GridMap() : rows(0), cols(0) {}

void GridMap::scanForEntrances() {
    startPoints.clear();
    exitPoints.clear();
    
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (grid[r][c] == 0) {
                if (r == 0 || c == 0) startPoints.push_back({r, c}); // Top/Left = Entry
                else if (r == rows - 1 || c == cols - 1) exitPoints.push_back({r, c}); // Bottom/Right = Exit
            }
        }
    }
}

bool GridMap::loadMap(const string& filename) {
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "[!] Error: Could not open map file: " << filename << "\n";
        return false;
    }

    grid.clear();
    string line;

    while (getline(file, line)) {
        vector<int> row;
        stringstream ss(line);
        int value;
        
        while (ss >> value) {
            row.push_back(value);
        }
        
        if (!row.empty()) {
            grid.push_back(row);
        }
    }
    
    file.close();


    if (!grid.empty()) {
        rows = grid.size();
        cols = grid[0].size();
        scanForEntrances();
        return true;
    }
    
    return false;
}

bool GridMap::isValidCoordinate(int x, int y) const {
    return (x >= 0 && x < rows && y >= 0 && y < cols);
}


bool GridMap::isWall(int x, int y) const {
    if (!isValidCoordinate(x, y)) return true;
    return grid[x][y] == 1;
}

bool GridMap::isHazard(int x, int y) const {
    if (!isValidCoordinate(x, y)) return false;
    return grid[x][y] == -1;
}

int GridMap::getCellValue(int x, int y) const {
    if (!isValidCoordinate(x, y)) return 1;
    return grid[x][y];
}

void GridMap::displayMap() const {
    cout << "\n--- Current Map Environment ---\n";
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (grid[i][j] == 1) {
                cout << COLOR_WALL << "# " << COLOR_RESET; // Wall
            } else if (grid[i][j] == -1) {
                cout << COLOR_HAZARD << "X " << COLOR_RESET; // Hazard
            } else {
                cout << COLOR_SAFE << ". " << COLOR_RESET; // Safe path
            }
        }
        cout << "\n";
    }
    cout << "-------------------------------\n";
}

int GridMap::getRows() const { return rows; }
int GridMap::getCols() const { return cols; }
std::vector<Point> GridMap::getStartPoints() const { return startPoints; }
std::vector<Point> GridMap::getExitPoints() const { return exitPoints; }