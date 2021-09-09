#include "../include/Processor.h"

#include "../include/sheet-metal-component/BoostSerializer.h"
#include "../include/cad-file-reader/CadFileReader.h"
#include "../include/cad-file-reader/CadFileReaderFactory.h"
#include "../../logging/include/LoggingFacility.h"
#include "../../logging/include/StandardOutputLogger.h"

#include "../../libseqgen/include/BendSequenceGenerator.h"

#include "../../msgqueue/contracts/ProcessPlanningStarted.h"
#include "../../msgqueue/contracts/ProcessPlanningComplete.h"

std::shared_ptr<Event> ProcessCadFile(EventPtr event)
{
    auto cadFile = dynamic_cast<ProcessPlanningStarted *>(event.get());

    auto unStringifiedSheetMetalObj = restore(cadFile->serializedData);
    auto loggingService = std::make_shared<Fxt::Logging::StandardOutputLogger>(cadFile->userID);

    int value = 0;
    std::vector<int> vec(cadFile->bendCount);
    std::generate(begin(vec), end(vec), [&value]
                  {
                      value += 1;
                      return value;
                  });

    int startTime = clock();

    auto bendSequenceGenerator = std::make_shared<BendSequenceGenerator>(vec, unStringifiedSheetMetalObj);
    bendSequenceGenerator->generateBendingSequence();
    bendSequenceGenerator->print();

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
    result->processingPlan.processingTime = total_time;
    result->processingPlan.quantity = 1;
    result->processingPlan.modules = bendSequenceGenerator->getNumberOfModules();
    result->processingPlan.rotations = bendSequenceGenerator->getNumberOfRotations();
    result->processingPlan.tools = bendSequenceGenerator->getNumberOfTools();
    result->processingPlan.totalToolDistance = bendSequenceGenerator->getNumberOfDistance();

    loggingService->writeInfoEntry("{" + cadFile->userID + "}: " + cadFile->cadFileID + ": Process planning on the extracted features....");

    return result;
}
