
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include "../include/Config.h"
#include "../include/GridMap.h"
#include "../include/Agent.h"
#include "../include/PathEngine.h"
#include "../include/ScenarioManager.h"
#include "../include/Visualizer.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "[ERROR] Insufficient arguments. \n";
        cout << "Usage: ./sentinel <scenario> <map_file> <battery1> <battery2> ...\n";
        return 1;
    }

    int scenarioChoice = stoi(argv[1]);
    string mapFile = argv[2];

    ScenarioType type = static_cast<ScenarioType>(scenarioChoice);
    ConstraintProfile missionRules = ScenarioManager::getProfile(type);
    
    GridMap environment;
    if (!environment.loadMap(mapFile)) {
        cout << "[ERROR] Map file missing or failed to load.\n";
        return 1;
    }

    vector<Point> entryPoints = environment.getStartPoints();
    vector<Point> exitPoints = environment.getExitPoints();

    if (entryPoints.empty()) {
        cout << "[FAILED] SYSTEM HALT: Map contains ZERO valid Entry points.\n";
        return 0;
    }

    if (exitPoints.empty()) {
        cout << "[FAILED] SYSTEM HALT: Map contains ZERO valid Exit points.\n";
        return 0; 
    }

    vector<int> warehouseEnergies;
    vector<int> warehouseWeights;
    int absoluteMaxRange = 0;
    for (int i = 3; i < argc; ++i) {
        int energy = 0, weight = 0;
        if (sscanf(argv[i], "%d %d", &energy, &weight) == 2) {
            warehouseEnergies.push_back(energy);
            warehouseWeights.push_back(weight);
            absoluteMaxRange += energy;
        } else {
            energy = stoi(argv[i]);
            warehouseEnergies.push_back(energy);
            warehouseWeights.push_back(0);
            absoluteMaxRange += energy;
        }
    }

    cout << "[GRID] " << environment.getRows() << " " << environment.getCols() << "\n";
    
    vector<Point> threats;
    for (int r = 0; r < environment.getRows(); ++r) {
        for (int c = 0; c < environment.getCols(); ++c) {
            if (environment.isHazard(r, c)) threats.push_back({r, c});
        }
    }

    PathEngine engine(environment, missionRules);
    
    vector<Point> absoluteBestPath;
    int bestCost = 999999; 
    int bestEntryIndex = -1;

    if (type == SCENARIO_WAREHOUSE || type == SCENARIO_CUSTOM) {
        vector<Agent> deployedRobots;

        int numAgents = (type == SCENARIO_CUSTOM) ? missionRules.maxAgents : entryPoints.size();

        for (size_t i = 0; i < numAgents; ++i) {
            size_t entryIndex = i % entryPoints.size();
            cout << "[LOG] Deploying simulation at Entry " << entryIndex + 1 << "...\n";
            string botName = "R" + to_string(i + 1);
            deployedRobots.push_back(Agent(botName, entryPoints[entryIndex], warehouseEnergies));
        }

        engine.solveConcurrent(deployedRobots, exitPoints, threats, absoluteMaxRange);

        for (size_t i = 0; i < deployedRobots.size(); ++i) {
            Agent& robot = deployedRobots[i];
            
            bool missionSuccess = false;
            for (const auto& exitNode : exitPoints) {
                if (robot.getPosition() == exitNode) {
                    missionSuccess = true;
                    break;
                }
            }

            if (missionSuccess) {
                int currentCost = robot.getPath().size();
                cout << "[SUCCESS] Route found via Entry " << i + 1 << " (Cost: " << currentCost << " steps).\n";
                
                if (currentCost < bestCost) {
                    bestCost = currentCost;
                    absoluteBestPath = robot.getPath();
                    bestEntryIndex = i + 1; 
                }
            } else {
                if (robot.getPath().size() - 1 >= absoluteMaxRange) {
                    cout << "[FAILED] Mission aborted due to insufficient resources.\n";
                } else {
                    cout << "[HAZARD] " << missionRules.failureMessage << "\n";
                }
            }
        }
    } else {
        for (size_t i = 0; i < entryPoints.size(); ++i) {
            cout << "[LOG] Deploying simulation at Entry " << i + 1 << "...\n";
            
            Agent robot(to_string(1), entryPoints[i], warehouseEnergies);
            bool missionSuccess = engine.solve(robot, exitPoints, threats, absoluteMaxRange);
            
            if (missionSuccess) {
                int currentCost = robot.getPath().size();
                cout << "[SUCCESS] Route found via Entry " << i + 1 << " (Cost: " << currentCost << " steps).\n";
                
                if (currentCost < bestCost) {
                    bestCost = currentCost;
                    absoluteBestPath = robot.getPath();
                    bestEntryIndex = i + 1;
                }
            } else {
                if (engine.getFarthestPath().size() - 1 >= absoluteMaxRange) {
                    cout << "[FAILED] Mission aborted due to insufficient resources.\n";
                } else {
                    cout << "[HAZARD] " << missionRules.failureMessage << "\n";
                }
            }
        }
    }

    if (bestEntryIndex != -1) {
        cout << "[LOG] =======================================\n";
        cout << "[LOG] ALL SIMULATIONS COMPLETE.\n";
        cout << "[LOG] Optimal Route: Entry " << bestEntryIndex << ".\n";
        cout << "[LOG] Transmitting final green path coordinates...\n";
        
        for (const Point& p : absoluteBestPath) {
            cout << "[BEST_PATH] " << p.x << " " << p.y << "\n";
        }

        engine.calculateFinalLoadout(bestCost, warehouseEnergies, warehouseWeights);
    } else {
        cout << "[FAILED] All entry points exhausted. Goal is unreachable due to lack of resources.\n";
    }
    
    return 0;
}