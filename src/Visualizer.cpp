#include "../include/Visualizer.h"
#include <iostream>

using namespace std;

void Visualizer::renderFrame(const Agent& agent) {
    Point pos = agent.getPosition();
    cout << "[STEP] " << pos.x << " " << pos.y << "\n";
}