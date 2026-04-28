#ifndef CONFIG_H
#define CONFIG_H

#include<string>

struct Point {
    int x, y;
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

struct ConstraintProfile {
    int maxAgents;
    int proximityThreshold;
    bool requiredResourceCheck;
    std::string resourceType;
    int hazardCost;
    
    std::string failureMessage;  
    std::string traverseMessage; 
};
#endif