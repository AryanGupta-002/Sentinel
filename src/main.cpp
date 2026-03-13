#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include "../include/Config.h"
#include "../include/GridMap.h"
#include "../include/Agent.h"
#include "../include/PathEngine.h"
#include "../include/ScenarioManager.h"
#include "../include/Visualizer.h"

using namespace std;

void printMenu() {
    cout << "\n=========================================\n";
    cout << "  PROJECT SENTINEL: PATH RECOVERY SYSTEM \n";
    cout << "=========================================\n";
    cout << "1. Fire Hazard (Strict Proximity: 2 Blocks)\n";
    cout << "2. Forest Path (Terrain Penalty)\n";
    cout << "3. Mine / Tunnel (Oxygen Constraint)\n";
    cout << "4. Warehouse (Multi-Robot Collision)\n";
    cout << "5. Custom Case\n";
    cout << "0. Exit\n";
    cout << "Choice: ";
}

int main() {
    bool running = true;
    
    while (running) {
        printMenu();
        int choice;
        cin >> choice;
        
        if (choice == 0) {
            cout << "Shutting down Sentinel System...\n";
            break;
        }
        
        if (choice < 1 || choice > 5) {
            cout << "Invalid choice. Please try again.\n";
            continue;
        }

        ScenarioType type = static_cast<ScenarioType>(choice);
        ConstraintProfile missionRules = ScenarioManager::getProfile(type);
        
        GridMap environment;
        string mapFile;
        cout << "\nEnter map filename (e.g., maps/test_10x10.txt): ";
        cin >> mapFile;
        
        cout << "[>] Loading Environment Data...\n";
        if (!environment.loadMap(mapFile)) {
            cout << "[!] Error: Map file missing or failed to load. Please check the path.\n";
            continue;
        }

        vector<Point> entryPoints = environment.getStartPoints();
        vector<Point> exitPoints = environment.getExitPoints();

        if (entryPoints.empty() || exitPoints.empty()) {
            cout << "[!] Error: Map must have at least one Entry (Top/Left) and one Exit (Bottom/Right).\n";
            continue;
        }
        
        cout << "[+] Map loaded! Detected " << entryPoints.size() << " Entries and " << exitPoints.size() << " Exits.\n";

        cout << "\n--- MISSION RESOURCES ---\n";
        cout << "Enter number of available battery/oxygen packs in the warehouse: ";
        int numPacks;
        cin >> numPacks;

        vector<int> warehouseBatteries(numPacks);
        int absoluteMaxRange = 0;
        
        cout << "Enter the values of the packs (separated by space): ";
        for (int i = 0; i < numPacks; i++) {
            cin >> warehouseBatteries[i];
            absoluteMaxRange += warehouseBatteries[i]; 
        }

        vector<Point> threats;
        for (int r = 0; r < environment.getRows(); ++r) {
            for (int c = 0; c < environment.getCols(); ++c) {
                if (environment.isHazard(r, c)) threats.push_back({r, c});
            }
        }

        PathEngine engine(environment, missionRules);
        bool missionSuccess = false;

        cout << "\n[>] Deploying Agent and executing Pathfinding Algorithms...\n";

        for (size_t i = 0; i < entryPoints.size(); ++i) {
            cout << "\n[>] Deploying Agent at Entry " << i + 1 << " [" << entryPoints[i].x << "," << entryPoints[i].y << "]...\n";
            
            Agent robot(1, entryPoints[i], warehouseBatteries);

            missionSuccess = engine.solve(robot, exitPoints, threats, absoluteMaxRange);
            
            Visualizer::renderFrame(environment, robot);

            if (missionSuccess) {
                cout << "\n[+] GOAL REACHED SAFELY via Entry " << i + 1 << ".\n";
                int estimatedCost = robot.getPath().size(); 
                engine.calculateFinalLoadout(estimatedCost, warehouseBatteries);
                break;
            } else {
                cout << "[-] Path blocked or resources insufficient from Entry " << i + 1 << ". Recalculating...\n";
            }
        }

        if (!missionSuccess) {
            cout << "\n[-] MISSION ABORT: All entry points exhausted. Goal is unreachable.\n";
            vector<Point> safeStop = engine.getFarthestPath();
            if (!safeStop.empty()) {
                cout << "[!] Suggesting best safe extraction point at: [" 
                     << safeStop.back().x << ", " << safeStop.back().y << "]\n";
            }
        }
    }
    
    return 0;
}