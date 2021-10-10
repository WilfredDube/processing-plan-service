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

std::string RedisCache = "tcp://redis:6379";

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

    auto bendSequenceGenerator = std::make_shared<BendSequenceGenerator>(redis, vec, unStringifiedSheetMetalObj);

    int startTime = clock();
    auto begin = std::chrono::high_resolution_clock::now();

    bendSequenceGenerator->generateBendingSequence();

    auto end = std::chrono::high_resolution_clock::now();
    int stopTime = clock();

    auto total_time = (stopTime - startTime) / double(CLOCKS_PER_SEC);

    bendSequenceGenerator->print();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() << "[ns]" << std::endl;
    std::cout << "Total difference = " << total_time << "[s]" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;

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
