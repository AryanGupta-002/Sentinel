#include <vector>

bool canCoverCost(const std::vector<int>& packs, int target) {
    if (target <= 0) return true;
    
    for (int pack : packs) {
        if (pack >= target) return true;
    }

    std::vector<bool> dp(target + 1, false);
    dp[0] = true;

    for (int pack : packs) {
        for (int i = target; i >= pack; i--) {
            if (dp[i - pack]) {
                dp[i] = true;
            }
        }
    }
    return dp[target];
}