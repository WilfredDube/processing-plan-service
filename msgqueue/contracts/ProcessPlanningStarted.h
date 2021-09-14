#pragma once

#include "../Event.h"

struct ProcessPlanningStarted : public Event
{
    std::string userID, cadFileID, taskID, serializedData;
    int bendCount;
    double freTime;

    std::string EventName();
    void createEvent(std::string data);
    json toJson();
};