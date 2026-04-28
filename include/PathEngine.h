#ifndef PATHENGINE_H
#define PATHENGINE_H

#include "GridMap.h"
#include "Agent.h"
#include "Config.h"
#include <vector>
#include <stack>
#include <string>

struct DFSFrame {
    Point pos;
    int dirIndex;
};

class PathEngine {
private:
    GridMap& map;
    ConstraintProfile profile;

    std::vector<Point> farthestPath;
    int maxDepthReached = 0;

public:
    PathEngine(GridMap& m, ConstraintProfile p);

    bool solve(Agent& agent, const std::vector<Point>& goals, const std::vector<Point>& hazards, int absoluteMaxRange);

    void solveConcurrent(std::vector<Agent>& robots, const std::vector<Point>& exits, const std::vector<Point>& threats, int absoluteMaxRange);

    void calculateFinalLoadout(int totalCost, const std::vector<int>& masterEnergies, const std::vector<int>& masterWeights); 
    std::vector<Point> getFarthestPath() const;
};

#endif