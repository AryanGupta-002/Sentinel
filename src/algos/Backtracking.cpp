#include "../../include/PathEngine.h"
#include "../../include/Visualizer.h"
#include <vector>
#include <iostream>
#include <map>
#include <stack>

bool isSafeProximity(Point p, const std::vector<Point>& threats, int threshold);
bool canCoverCost(const std::vector<int>& packs, int target);

bool runRecursiveSearch(Agent& agent, const std::vector<Point>& goals, GridMap& map, ConstraintProfile& profile, 
            const std::vector<Point>& threats, int absoluteMaxRange, std::vector<Point>& farthestPath, int& maxDepth) {

    Point current = agent.getPosition();

    if (agent.getPath().size() > maxDepth) {
        maxDepth = agent.getPath().size();
        farthestPath = agent.getPath();
    }

    if (agent.getPath().size() - 1 > absoluteMaxRange) {
        return false; 
    }

    for (const auto& goal : goals) {
        if (current == goal) return true;
    }

    std::vector<Point> dirs = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

    for (auto d : dirs) {
        Point next = {current.x + d.x, current.y + d.y};

        if (map.isValidCoordinate(next.x, next.y) && !map.isWall(next.x, next.y)) {
            if (!isSafeProximity(next, threats, profile.proximityThreshold)) {
                std::cout << "[HAZARD] " << profile.failureMessage << "\n";
                continue; 
            }

            int cost = map.isHazard(next.x, next.y) ? profile.hazardCost : 1;
            if (profile.requiredResourceCheck) {
                if (!canCoverCost(agent.getResourcePacks(), cost)) {
                    if (map.isHazard(next.x, next.y)) {
                        std::cout << "[HAZARD] No combination of packs could help the user cover the hazard cost.\n";
                    } else {
                        std::cout << "[LOG] Insufficient battery for standard movement. Backtracking...\n";
                    }
                    continue; 
                }
            }

            bool visited = false;
            for (Point p : agent.getPath()) { if (p == next) { visited = true; break; } }
            if (visited) continue;
            if (map.isHazard(next.x, next.y)) {
                std::cout << "[ACTION] " << profile.traverseMessage << "\n";
            }

            agent.moveTo(next);

            Visualizer::renderFrame(agent);

            if (runRecursiveSearch(agent, goals, map, profile, threats, absoluteMaxRange, farthestPath, maxDepth)) return true;
            
            agent.stepBack();
            std::cout << "[LOG] Dead end reached. Backtracking...\n";
        }
    }
    return false;
}
void PathEngine::solveConcurrent(std::vector<Agent>& robots, const std::vector<Point>& exits, const std::vector<Point>& threats, int absoluteMaxRange) {
    
    std::map<std::string, std::stack<DFSFrame>> robotMemory;
    std::map<std::string, bool> robotFinished;
    for (Agent& r : robots) {
        std::stack<DFSFrame> initialStack;
        initialStack.push({r.getPosition(), 0});
        robotMemory[r.getId()] = initialStack;
        robotFinished[r.getId()] = false;
    }

    bool simulationRunning = true;
    while (simulationRunning) {
        simulationRunning = false;
        for (Agent& agent : robots) {
            std::string id = agent.getId();

            if (robotFinished[id]) continue;
            if (robotMemory[id].empty()) continue;

            simulationRunning = true;

            std::stack<DFSFrame>& memStack = robotMemory[id];
            DFSFrame currentFrame = memStack.top();
            memStack.pop(); 

            Point current = agent.getPosition();
            if (agent.getPath().size() > maxDepthReached) {
                maxDepthReached = agent.getPath().size();
                farthestPath = agent.getPath();
            }
            bool reachedExit = false;
            for (const auto& exitNode : exits) {
                if (current == exitNode) {
                    reachedExit = true;
                    break;
                }
            }
            if (reachedExit) {
                robotFinished[id] = true;
                continue;
            }
            if (agent.getPath().size() - 1 > absoluteMaxRange) {
                agent.stepBack();
                continue;
            }

            bool movedThisTick = false;
            while (currentFrame.dirIndex < 4 && !movedThisTick) {
                int agentNum = 1;
                if (id.length() > 1 && id[0] == 'R') {
                    agentNum = std::stoi(id.substr(1));
                }

                std::vector<Point> agentDirs;
                if (agentNum % 2 != 0) {
                    agentDirs = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}}; 
                } else {
                    agentDirs = {{0, 1}, {-1, 0}, {1, 0}, {0, -1}}; 
                }

                Point d = agentDirs[currentFrame.dirIndex];
                Point next = {current.x + d.x, current.y + d.y};
                
                currentFrame.dirIndex++;

                if (map.isValidCoordinate(next.x, next.y) && !map.isWall(next.x, next.y)) {
                    bool robotCollision = false;
                    for (const Agent& otherBot : robots) {
                        if (otherBot.getId() != id && otherBot.getPosition() == next) {
                            robotCollision = true;
                            std::cout << "[HAZARD] " << id << " " << profile.failureMessage << "\n";
                            break; 
                        }
                    }
                    if (robotCollision) continue;
                    if (!isSafeProximity(next, threats, profile.proximityThreshold)) {
                        std::cout << "[HAZARD] " << id << " " << profile.failureMessage << "\n";
                        continue;
                    }
                    int cost = map.isHazard(next.x, next.y) ? profile.hazardCost : 1;
                    if (profile.requiredResourceCheck) {
                        if (!canCoverCost(agent.getResourcePacks(), cost)) {
                            if (map.isHazard(next.x, next.y)) {
                                std::cout << "[HAZARD] " << id << " No combination of packs could help the robot cover the hazard cost.\n";
                            } else {
                                std::cout << "[LOG] " << id << " Insufficient battery. Backtracking...\n";
                            }
                            continue;
                        }
                    }
                    bool visited = false;
                    for (Point p : agent.getPath()) { if (p == next) { visited = true; break; } }
                    if (visited) continue;
                    if (map.isHazard(next.x, next.y)) {
                        std::cout << "[ACTION] " << id << " " << profile.traverseMessage << "\n";
                    }
                    memStack.push(currentFrame);
                    memStack.push({next, 0});
                    
                    agent.moveTo(next);
                    std::cout << "[STEP] " << id << " " << next.x << " " << next.y << "\n";
                    
                    movedThisTick = true;
                }
            }

            if (!movedThisTick) {
                agent.stepBack();
                if (!memStack.empty()) {
                    std::cout << "[LOG] " << id << " Dead end reached. Backtracking...\n";
                }
            }
        }
    }
}