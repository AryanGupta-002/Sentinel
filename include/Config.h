#ifndef CONFIG_H
#define CONFIG_H

#include<string>

struct Point {
    int x, y;
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

struct ConstraintProfile{
    int maxAgents;
    int proximityThreshold;
    bool requiredResourceCheck;
    std::string resourceType;
    int hazardCost;
};

// fancy colors to make it attractive (cause why not)
const std::string COLOR_RESET = "\033[0m";
const std::string COLOR_SAFE  = "\033[32m"; // Green
const std::string COLOR_HAZARD= "\033[31m"; // Red
const std::string COLOR_WALL  = "\033[90m"; // Gray
const std::string COLOR_ROBOT = "\033[36m"; // Cyan

#endif