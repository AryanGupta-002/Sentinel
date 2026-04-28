#include "../include/PathEngine.h"
#include <iostream>

bool runRecursiveSearch(Agent& agent, const std::vector<Point>& goals, GridMap& map, ConstraintProfile& profile, const std::vector<Point>& threats, int absoluteMaxRange, std::vector<Point>& farthestPath, int& maxDepth);

std::vector<int> getBestLoadout(const std::vector<int>& energies, const std::vector<int>& weights, int totalCost);

PathEngine::PathEngine(GridMap& m, ConstraintProfile p) : map(m), profile(p), maxDepthReached(0) {}

bool PathEngine::solve(Agent& agent, const std::vector<Point>& goals, const std::vector<Point>& hazards, int absoluteMaxRange) {
    maxDepthReached = 0;
    farthestPath.clear();
    return runRecursiveSearch(agent, goals, map, profile, hazards, absoluteMaxRange, farthestPath, maxDepthReached);
}

std::vector<Point> PathEngine::getFarthestPath() const {
    return farthestPath;
}

void PathEngine::calculateFinalLoadout(int totalCost, const std::vector<int>& finalEnergies, const std::vector<int>& finalWeights) {
    std::cout << "\n--- MISSION BRIEFING ---\n";

    std::vector<int> finalLoadoutIndices = getBestLoadout(finalEnergies, finalWeights, totalCost);

    if (finalLoadoutIndices.empty()) {
        std::cout << "Total Mission Cost: " << totalCost << " units.\n";
        std::cout << "MISSION ABORT: Insufficient resources to complete the trip.\n";
    } else {
        int totalWeight = 0;
        for (int idx : finalLoadoutIndices) {
            totalWeight += finalWeights[idx];
        }
        std::cout << "Total Mission Cost: " << totalCost << " units.\n";
        std::cout << "Total Minimum Weight: " << totalWeight << " units.\n";
        std::cout << ">> REQUIRED LOADOUT: Pack exactly these units:" << "\n";
        for (int idx : finalLoadoutIndices) {
            std::cout << "[Pack " << idx + 1 << ": Energy: " << finalEnergies[idx] << ", Weight: " << finalWeights[idx] << "]\n";
        }
        std::cout << "\n";
    }
}