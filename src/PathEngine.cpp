#include "../include/PathEngine.h"
#include <iostream>

bool runRecursiveSearch(Agent& agent, const std::vector<Point>& goals, GridMap& map, ConstraintProfile& profile, const std::vector<Point>& threats, int absoluteMaxRange, std::vector<Point>& farthestPath, int& maxDepth);

std::vector<int> getBestLoadout(const std::vector<int>& availablePacks, int totalCost);

PathEngine::PathEngine(GridMap& m, ConstraintProfile p) : map(m), profile(p), maxDepthReached(0) {}

bool PathEngine::solve(Agent& agent, const std::vector<Point>& goals, const std::vector<Point>& hazards, int absoluteMaxRange) {
    maxDepthReached = 0;
    farthestPath.clear();
    return runRecursiveSearch(agent, goals, map, profile, hazards, absoluteMaxRange, farthestPath, maxDepthReached);
}

std::vector<Point> PathEngine::getFarthestPath() const {
    return farthestPath;
}

void PathEngine::calculateFinalLoadout(int totalCost, const std::vector<int>& masterInventory) {
    std::cout << "\n--- MISSION BRIEFING ---\n";
    std::cout << "Total Mission Cost: " << totalCost << " units.\n";

    std::vector<int> finalLoadout = getBestLoadout(masterInventory, totalCost);

    if (finalLoadout.empty()) {
        std::cout << "[!] MISSION ABORT: Insufficient resources to complete the 21-unit trip.\n";
    } else {
        std::cout << ">> REQUIRED LOADOUT: Pack exactly these units: ";
        for (int pack : finalLoadout) {
            std::cout << "[" << pack << "] ";
        }
        std::cout << "\n";
    }
}