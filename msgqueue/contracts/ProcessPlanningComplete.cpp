#include "ProcessPlanningComplete.h"

std::string ProcessPlanningComplete::EventName()
{
    return "processPlanningComplete";
};

json ProcessPlanningComplete::toJson()
{
    json jsonObject;

    std::vector<json> bendSequence;

    for (auto &bendID : processingPlan.bendSequence)
    {
        json jsonBendID = {
            {"bend_id", bendID},
        };
        bendSequence.push_back(jsonBendID);
    }

    json processingplan = {
        {"cadfile_id", cadFileID},
        {"rotations", processingPlan.rotations},
        {"flips", processingPlan.flips},
        {"tools", processingPlan.tools},
        {"modules", processingPlan.modules},
        {"processing_time", processingPlan.processingTime},
        {"quantity", 1},
        {"total_tool_distance", processingPlan.totalToolDistance},
        {"bend_sequences", bendSequence}};

    json resultJson;
    resultJson["pp_plan"] = processingplan;
    resultJson["process_level"] = processLevel;
    resultJson["user_id"] = userID;
    resultJson["cadfile_id"] = cadFileID;
    resultJson["task_id"] = taskID;

    return resultJson;
}
