#ifndef SCENARIOMANAGER_H
#define SCENARIOMANAGER_H

#include "Config.h"
// enum so scenerio selection becomes easypeasyyy 
enum ScenarioType {
    SCENARIO_FIRE = 1,
    SCENARIO_FOREST = 2,
    SCENARIO_MINE = 3,
    SCENARIO_WAREHOUSE = 4,
    SCENARIO_CUSTOM = 5
};

class ScenarioManager {
public:
    static ConstraintProfile getProfile(ScenarioType type);
};

#endif