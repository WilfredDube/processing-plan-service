#include "../include/Processor.h"

#include "../include/sheet-metal-component/BoostSerializer.h"
#include "../include/cad-file-reader/CadFileReader.h"
#include "../include/cad-file-reader/CadFileReaderFactory.h"
#include "../../logging/include/LoggingFacility.h"
#include "../../logging/include/StandardOutputLogger.h"

#include "../../libseqgen/include/BendSequenceGenerator.h"
#include "../../libseqgen/include/ProductionTime.h"

#include "../../msgqueue/contracts/ProcessPlanningStarted.h"
#include "../../msgqueue/contracts/ProcessPlanningComplete.h"

#include <chrono>
#include <sw/redis++/redis++.h>
#include <stdlib.h>

std::string RedisCache; // "tcp://localhost:6379";

std::shared_ptr<Event> ProcessCadFile(EventPtr event, Logger loggingService)
{
    using namespace sw::redis;

    auto cadFile = dynamic_cast<ProcessPlanningStarted *>(event.get());
    loggingService->setLoggingID(cadFile->userID, cadFile->cadFileID);

    loggingService->writeInfoEntry(__FILE__, __LINE__, "Unserializing data....");
    auto unStringifiedSheetMetalObj = restore(cadFile->serializedData);

    int value = 0;
    std::vector<int> vec(cadFile->bendCount);
    std::generate(begin(vec), end(vec), [&value]
                  {
                      value += 1;
                      return value;
                  });
    try
    {
        RedisCache = std::getenv("REDIS");
    }
    catch (const std::exception &e)
    {
        loggingService->writeErrorEntry(__FILE__, __LINE__, e.what());
        exit(1);
    }

    auto redis = Redis(RedisCache);
    auto bendSequenceGenerator = std::make_shared<BendSequenceGenerator>(redis, vec, unStringifiedSheetMetalObj, loggingService);

    int startTime = clock();

    try
    {
        bendSequenceGenerator->generateBendingSequence();
    }
    catch (const std::exception &e)
    {
        loggingService->writeErrorEntry(__FILE__, __LINE__, e.what());
    }

    int stopTime = clock();

    auto total_time = (stopTime - startTime) / double(CLOCKS_PER_SEC);

    auto result = std::make_shared<ProcessPlanningComplete>();

    result->cadFileID = cadFile->cadFileID;
    result->userID = cadFile->userID;
    result->taskID = cadFile->taskID;

    std::vector<FaceID> bendSequence;

    for (FaceID bendID : bendSequenceGenerator->getSequence())
    {
        bendSequence.push_back(bendID);
    }

    result->processLevel = 2;
    result->processingPlan.bendSequence = bendSequence;
    result->processingPlan.cadFileID = cadFile->cadFileID;
    result->processingPlan.flips = bendSequenceGenerator->getNumberOfFlips();
    result->processingPlan.processingTime = total_time + cadFile->freTime;
    result->processingPlan.quantity = 1;
    result->processingPlan.modules = bendSequenceGenerator->getNumberOfModules();
    result->processingPlan.rotations = bendSequenceGenerator->getNumberOfRotations();
    result->processingPlan.tools = bendSequenceGenerator->getNumberOfTools();
    result->processingPlan.totalToolDistance = bendSequenceGenerator->getNumberOfDistance();
    result->processingPlan.estimatedProductionTime = computeTotalProductionTime(1, result->processingPlan.tools, cadFile->bendCount,
                                                                                result->processingPlan.flips, result->processingPlan.rotations);

    loggingService->writeInfoEntry(__FILE__, __LINE__, "Process planning complete....");

    return result;
}
