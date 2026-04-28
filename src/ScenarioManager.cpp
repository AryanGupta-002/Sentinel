#include "../include/ScenarioManager.h"
#include <iostream>
#include <cstdlib>
#include <string>
using namespace std;

ConstraintProfile ScenarioManager::getProfile(ScenarioType type) {
    ConstraintProfile profile;

    switch (type) {
        case SCENARIO_FIRE:
            profile = {1, 2, true, "Coolant", 3, "Fire detected 2 blocks ahead. Rerouting..."};
            break;
            
        case SCENARIO_FOREST:
            profile = {1, 0, true, "Energy", 2, 
                       "Dense mud puddle drained mobility reserves. Rerouting...", 
                       "Trying to walk over the mud as enough resource is available."};
            break;
            
        case SCENARIO_MINE:
            profile = {1, 1, true, "Oxygen", 5, "Toxic gas leak detected. Oxygen compromised. Rerouting..."};
            break;
            
        case SCENARIO_WAREHOUSE:
            profile = {2, 3, true, "Battery", 1, "Foreign robot collision imminent! Evasive maneuvers engaged..."};
            break;
            
        case SCENARIO_CUSTOM: { 
            const char* envAgents = std::getenv("CUSTOM_AGENTS");
            const char* envProx = std::getenv("CUSTOM_PROX");
            const char* envCost = std::getenv("CUSTOM_COST");

            profile.maxAgents = envAgents ? std::stoi(envAgents) : 2;
            profile.proximityThreshold = envProx ? std::stoi(envProx) : 1;
            profile.hazardCost = envCost ? std::stoi(envCost) : 2;
            
            profile.requiredResourceCheck = true;
            profile.resourceType = "Custom Fuel";
            
            profile.failureMessage = "Unidentified environmental anomaly/Insufficient fuel. Rerouting...";
            profile.traverseMessage = "Traversing custom hazard zone...";
            break;
        }
            
        default:
            profile = {1, 0, false, "None", 1, "Path blocked. Rerouting..."}; 
            break;
    }

    return profile;
}