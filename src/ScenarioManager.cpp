#include "../include/ScenarioManager.h"
#include <iostream>

using namespace std;

ConstraintProfile ScenarioManager::getProfile(ScenarioType type) {
    ConstraintProfile profile;

    switch (type) {
        case SCENARIO_FIRE:

            profile = {1, 1, true, "Coolant", 3};
            break;
            
        case SCENARIO_FOREST:

            profile = {1, 0, true, "Energy", 2};
            break;
            
        case SCENARIO_MINE:

            profile = {1, 2, true, "Oxygen", 5};
            break;
            
        case SCENARIO_WAREHOUSE:

            profile = {2, 3, true, "Battery", 1};
            break;
            
        case SCENARIO_CUSTOM:

            cout << "\n--- CUSTOM SCENARIO CONFIGURATION ---\n";
            cout << "Enter number of robots: ";
            cin >> profile.maxAgents;
            cout << "Enter proximity threshold (N-Queens collision distance): ";
            cin >> profile.proximityThreshold;
            cout << "Enter hazard movement penalty cost: ";
            cin >> profile.hazardCost;
            profile.requiredResourceCheck = true;
            profile.resourceType = "Custom Fuel";
            break;
            
        default:
            profile = {1, 0, false, "None", 1}; 
            break;
    }

    return profile;
}