#ifndef GRIDMAP_H
#define GRIDMAP_H

#include <vector>
#include <string>
#include "Config.h"

class GridMap {
    private:
        std::vector<std::vector<int>> grid;
        int rows, cols;
        std::vector<Point> startPoints;
        std::vector<Point> exitPoints;
        void scanForEntrances();

    public:
        GridMap();

        std::vector<Point> getStartPoints() const;
        std::vector<Point> getExitPoints() const;

        bool loadMap(const std::string& filename);
        void displayMap() const;

        bool isValidCoordinate(int x, int y) const;
        bool isWall(int x, int y) const;
        bool isHazard(int x, int y) const;

        int getRows() const;
        int getCols() const;
        int getCellValue(int x, int y) const;    
};

#endif