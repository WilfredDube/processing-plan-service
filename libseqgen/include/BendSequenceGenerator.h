#pragma once

#include "../../libfxtract/include/sheet-metal-component/SheetMetal.h"
#include "../../logging/include/LoggingFacility.h"

#include <memory>
#include <vector>
#include <sw/redis++/redis++.h>

class BendSequenceGenerator
{
    struct Sequence
    {
        std::vector<int> chromosome;
        size_t nTools{1}, nRotations{0}, nFlips{0}, targetSize;
        double distance{0}, fitness{0.0}, nModules{0};

        Sequence(std::vector<int> chromosome) : chromosome{chromosome}, targetSize{chromosome.size()} {}

        double cal_fitness(SheetMetalPtr &sheetMetalFeature);

        Sequence mate(Sequence parentSequence);
        bool operator<(const Sequence &individualSequence);
        bool operator==(const Sequence &individualSequence);
    };

    std::shared_ptr<Sequence> sequenceImpl_;

    SheetMetalPtr &sheetMetalFeature;
    sw::redis::Redis &redis;
    Logger &logger;

public:
    BendSequenceGenerator() = delete;
    explicit BendSequenceGenerator(sw::redis::Redis &redis, std::vector<int> chromosome, SheetMetalPtr &model, Logger &logger);

    void generateBendingSequence();

    size_t getNumberOfTools();
    size_t getNumberOfRotations();
    size_t getNumberOfFlips();
    size_t getNumberOfModules();
    double getNumberOfDistance();

    std::vector<int> getSequence();
    size_t getSequenceSize();
    void print();
};