#include "../../include/Config.h"
#include <vector>
#include <cmath>

bool isSafeProximity(Point p, const std::vector<Point>& threats, int threshold){
    if (threshold == 0) return true;
    
    for(const auto& threat : threats){
        if (p.x == threat.x || p.y == threat.y){
            int dist = std::abs(p.x - threat.x) + std::abs(p.y - threat.y);

            if(dist<= threshold) return false;
        }
    }
    return true;
}