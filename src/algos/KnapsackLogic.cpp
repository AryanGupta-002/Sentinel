#include <vector>
#include <numeric>
#include <algorithm>

std::vector<int> getBestLoadout(const std::vector<int>& availablePacks, int totalCost) {
    int totalAvailable = 0;
    for (int pack : availablePacks) totalAvailable += pack;

    if (totalAvailable < totalCost) return {}; 

    std::vector<int> dp(totalAvailable + 1, -1);
    dp[0] = 0;

    for (int pack : availablePacks) {
        for (int currentSum = totalAvailable; currentSum >= pack; currentSum--) {
            if (dp[currentSum - pack] != -1 && dp[currentSum] == -1) {
                dp[currentSum] = pack;
            }
        }
    }

    int bestSum = totalCost;
    while (bestSum <= totalAvailable && dp[bestSum] == -1) {
        bestSum++;
    }

    std::vector<int> loadout;
    int current = bestSum;
    while (current > 0) {
        int packUsed = dp[current];
        loadout.push_back(packUsed);
        current -= packUsed;
    }

    return loadout;
}