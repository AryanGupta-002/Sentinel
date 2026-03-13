#include "../include/Visualizer.h"
#include <iostream>
#include <cstdlib>

#ifdef _WIN32
    #include <windows.h> // Windows specific
#else
    #include <unistd.h>  // Mac/Linux specific
#endif

using namespace std;

void Visualizer::sleep(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);         // Windows 'Sleep' takes milliseconds
#else
    usleep(milliseconds * 1000); // Unix 'usleep' takes microseconds
#endif
}

void Visualizer::renderFrame(const GridMap& map, const Agent& agent) {

    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    cout << "\n=========================================\n";
    cout << "  SENTINEL LIVE TRACKING FEED \n";
    cout << "=========================================\n";

    Point agentPos = agent.getPosition();
    vector<Point> path = agent.getPath();

    for (int i = 0; i < map.getRows(); ++i) {
        for (int j = 0; j < map.getCols(); ++j) {
            Point currentPt = {i, j};

            // 1. Is the system currently standing here?
            if (currentPt == agentPos) {
                cout << COLOR_ROBOT << "@ " << COLOR_RESET;
            }
            else {
                // 2. Did the system visit this spot? (Draw Breadcrumbs)
                bool inPath = false;
                for (const auto& p : path) {
                    if (currentPt == p) { inPath = true; break; }
                }
                
                if (inPath) {
                    cout << COLOR_ROBOT << ". " << COLOR_RESET; // Visited trail
                }
                // 3. Is it a Hazard?
                else if (map.isHazard(i, j)) {
                    cout << COLOR_HAZARD << "X " << COLOR_RESET;
                }
                // 4. Is it a Wall?
                else if (map.isWall(i, j)) {
                    cout << COLOR_WALL << "# " << COLOR_RESET;
                }
                // 5. Normal unvisited safe path
                else {
                    cout << COLOR_SAFE << ". " << COLOR_RESET;
                }
            }
        }
        cout << "\n";
    }

    cout << "=========================================\n";
    cout << "Current Position: [" << agentPos.x << ", " << agentPos.y << "]\n";
    cout << "Path Length (Memory Stack): " << path.size() << " steps\n";
    cout << "-----------------------------------------\n";
}