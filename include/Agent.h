#ifndef AGENT_H
#define AGENT_H

#include "Config.h"
#include<vector>

class Agent{
    private:
    int id;
    Point currentPos;
    std::vector<Point> pathHistory;
    std::vector<int> resourcePacks;

    public:
    Agent(int agentId, Point startPos, std::vector<int> initialResources);

    void moveTo(Point newPos);
    void stepBack();
    std::vector<int> getResourcePacks() const;
    Point getPosition() const;
    int getId() const;
    std::vector<Point> getPath() const;
};

#endif