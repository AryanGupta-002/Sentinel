#include "../include/Agent.h"

Agent::Agent(int agentId, Point startPos, std::vector<int> initialResources) {
    id = agentId;
    currentPos = startPos;
    resourcePacks = initialResources;
    pathHistory.push_back(startPos);
}

void Agent::moveTo(Point newPos) {
    currentPos = newPos;
    pathHistory.push_back(newPos);
}

void Agent::stepBack() {
    if (pathHistory.size() > 1) {

        pathHistory.pop_back();
        
        currentPos = pathHistory.back();
    }
}

std::vector<int> Agent::getResourcePacks() const {
    return resourcePacks;
}

Point Agent::getPosition() const { return currentPos; }
int Agent::getId() const { return id; }
std::vector<Point> Agent::getPath() const { return pathHistory; }