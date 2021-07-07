#include "../include/ProductionTime.h"

#include <cmath>

namespace {
    const double approachingSpeed = 0.1;
    const double formingSpeed = 0.01;
    const double returningSpeed = 0.08;
    const double strokeLength = 0.2;

    const double toolRemovalTime = 3;
    const double toolSetUpTime = 4;
    const double configurationTime = 20;
    const double strokeAdjustmentTime = 20;

    const double flipingTime = 25;
    const double rotatingTime = 20;
    const double partRemovalTime = 10;
    const double sheetPositioningTime = 10;
} // namespace

double computeTotalProductionTime(unsigned nParts, unsigned nTools, unsigned nBends, unsigned nFlips, unsigned nRotations)
{
    double setUpTime = machineSetUpTime(nTools);
    double totalBendingTime = computeTotalBendingTime(nBends, nFlips, nRotations);

    return setUpTime + (nParts * totalBendingTime);
}

double machineSetUpTime(unsigned nTools)
{
    return ((nTools * toolRemovalTime) + (nTools * toolSetUpTime) + configurationTime + strokeAdjustmentTime);
}

double computeTotalBendingTime(unsigned nBends, unsigned nFlips, unsigned nRotations)
{
    double timeToPerformBend = nBends * computeTimeToFormBend();
    double linearTime = (nFlips * flipingTime) + (nRotations * rotatingTime) + partRemovalTime + sheetPositioningTime;

    return (timeToPerformBend + linearTime);
}

double computeTimeToFormBend()
{
    double formingTime = strokeLength / formingSpeed;
    double approachingTime = strokeLength / approachingSpeed;
    double returningTime = strokeLength / returningSpeed;

    return (approachingTime + formingTime + returningTime);
}