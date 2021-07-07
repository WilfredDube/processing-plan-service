#ifndef PRODUCTION_TIME_H_
#define PRODUCTION_TIME_H_

/**
 *  Compute the total production time i.e. the total bending time
 *  Total production time = time to setup + (num of parts * bending time)
 */
double computeTotalProductionTime(unsigned nParts, unsigned nTools, unsigned nBends, unsigned nFlips, unsigned nRotations);

/**
 *  Computes the time to setup the machine
 *  Total bending machine setup time = (the number of tools * time to remove tools for the previous part) + 
 *  (the number of tools * time to setup the tools for the current part) + time to configure the machine +
 *   stock adjustment time
 */
double machineSetUpTime(unsigned nTools);

/**
 *  Computes the time to bend the whole part
 *  Time to bend the part = (number of bends * time to perform a bend) + 
 *  (number of rotations * time to rotate) + (number of flips * time to flip) +
 *  time to remove part + time to position sheet
 */
double computeTotalBendingTime(unsigned nBends, unsigned nFlips, unsigned nRotations);

/**
 *  Compute the time to perfom a bend
 *  Time to perform a bend = Approaching time + forming time + returning time
 */
double computeTimeToFormBend();

#endif