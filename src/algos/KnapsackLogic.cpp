#include "../../include/Config.h"
#include <vector>
#include <algorithm>
#include <iostream>

const int INF = 1e9; 

std::vector<int> getBestLoadout(const std::vector<int>& energies, const std::vector<int>& weights, int targetCost) {
    int totalEnergy = 0;
    for (int energy : energies) {
        totalEnergy += energy;
    }
    if (totalEnergy < targetCost) return {}; 

    int n = energies.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(totalEnergy + 1, INF));
    std::vector<std::vector<bool>> keep(n + 1, std::vector<bool>(totalEnergy + 1, false));

    dp[0][0] = 0;
    for (int i = 1; i <= n; ++i) {
        int currentEnergy = energies[i - 1];
        int currentWeight = weights[i - 1];
        
        for (int v = 0; v <= totalEnergy; ++v) {
            dp[i][v] = dp[i - 1][v];
            keep[i][v] = false;
            if (v >= currentEnergy && dp[i - 1][v - currentEnergy] != INF) {
                int weightIfWeTakeIt = dp[i - 1][v - currentEnergy] + currentWeight;
                if (weightIfWeTakeIt < dp[i][v]) {
                    dp[i][v] = weightIfWeTakeIt;
                    keep[i][v] = true; 
                }
            }
        }
    }
    int minWeight = INF;
    int bestEnergy = -1;
    
    for (int v = targetCost; v <= totalEnergy; ++v) {
        if (dp[n][v] < minWeight) {
            minWeight = dp[n][v];
            bestEnergy = v;
        }
    }

    if (bestEnergy == -1) return {};
    std::vector<int> chosenIndices;
    int currentV = bestEnergy;
    
    for (int i = n; i > 0; --i) {
        if (keep[i][currentV]) {
            chosenIndices.push_back(i - 1);
            currentV -= energies[i - 1];
        }
    }

    return chosenIndices;
}