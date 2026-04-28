#ifndef AGENT_H
#define AGENT_H

#include "Config.h"
#include <vector>
#include <string>

class Agent{
    private:
    std::string id;
    Point currentPos;
    std::vector<Point> pathHistory;
    std::vector<int> resourcePacks;

    public:
    Agent(std::string agentId, Point startPos, std::vector<int> initialResources);

    void moveTo(Point newPos);
    void stepBack();
    std::vector<int> getResourcePacks() const;
    Point getPosition() const;
    std::string getId() const;
    std::vector<Point> getPath() const;

};

#endif