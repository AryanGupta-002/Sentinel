#ifndef PATHENGINE_H
#define PATHENGINE_H

#include "GridMap.h"
#include "Agent.h"
#include "Config.h"
#include <vector>

class PathEngine {
private:
    GridMap& map;
    ConstraintProfile profile;

    std::vector<Point> farthestPath;
    int maxDepthReached = 0;

public:
    PathEngine(GridMap& m, ConstraintProfile p);

    bool solve(Agent& agent, const std::vector<Point>& goals, const std::vector<Point>& hazards, int absoluteMaxRange);

    void calculateFinalLoadout(int totalCost, const std::vector<int>& masterInventory); 
    std::vector<Point> getFarthestPath() const;
};

#endif