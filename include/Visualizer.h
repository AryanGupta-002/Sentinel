#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "GridMap.h"
#include "Agent.h"
#include "Config.h"
#include <vector>

class Visualizer {
public:
    static void renderFrame(const GridMap& map, const Agent& agent);
    static void sleep(int milliseconds);
};

#endif