#include "../../include/PathEngine.h"
#include "../../include/Visualizer.h"
#include <vector>

bool isSafeProximity(Point p, const std::vector<Point>& threats, int threshold);
bool canCoverCost(const std::vector<int>& packs, int target);

bool runRecursiveSearch(Agent& agent, const std::vector<Point>& goals, GridMap& map, ConstraintProfile& profile, const std::vector<Point>& threats, int absoluteMaxRange, std::vector<Point>& farthestPath, int& maxDepth) {
    Point current = agent.getPosition();

    if (agent.getPath().size() > maxDepth) {
        maxDepth = agent.getPath().size();
        farthestPath = agent.getPath();
    }

    if (agent.getPath().size() > absoluteMaxRange) {
        return false; 
    }

    for (const auto& goal : goals) {
        if (current == goal) return true;
    }

    std::vector<Point> dirs = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

    for (auto d : dirs) {
        Point next = {current.x + d.x, current.y + d.y};

        if (map.isValidCoordinate(next.x, next.y) && !map.isWall(next.x, next.y)) {
            
            if (!isSafeProximity(next, threats, profile.proximityThreshold)) continue;

            int cost = map.isHazard(next.x, next.y) ? profile.hazardCost : 1;
            
            if (profile.requiredResourceCheck) {
                if (!canCoverCost(agent.getResourcePacks(), cost)) continue;
            }

            bool visited = false;
            for (Point p : agent.getPath()) { if (p == next) { visited = true; break; } }
            if (visited) continue;

            agent.moveTo(next);
            
            Visualizer::renderFrame(map, agent);
            Visualizer::sleep(500); 

            if (runRecursiveSearch(agent, goals, map, profile, threats, absoluteMaxRange, farthestPath, maxDepth)) return true;
            
            agent.stepBack();
        }
    }
    return false;
}