#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "Agent.h"
#include "Config.h"
#include <vector>

class Visualizer {
public:
    static void renderFrame(const Agent& agent);

};

#endif