#include "../include/BendSequenceGenerator.h"
#include "../include/GA.h"

#include "../../libfxtract/include/Computation.h"

#include <algorithm>
#include <iostream>
#include <chrono>
#include <random>
#include <map>
#include <utility>
#include <boost/serialization/vector.hpp>

std::vector<std::vector<int>> genomes;
std::map<std::pair<Fxt::SheetMetalComponent::FaceID, Fxt::SheetMetalComponent::FaceID>, std::pair<int, double>> cache;

std::string
save(const std::vector<std::vector<int>> genomes)
{
    std::stringstream ss;
    boost::archive::text_oarchive oa(ss);
    oa << genomes;

    return ss.str();
}

std::vector<std::vector<int>> restoreGenome(std::string restoreStr)
{
    std::vector<std::vector<int>> restored;

    std::stringstream iss(restoreStr);
    boost::archive::text_iarchive oa(iss);
    oa >> restored;

    return restored;
}

BendSequenceGenerator::BendSequenceGenerator(sw::redis::Redis &redis, std::vector<int> chromosome, SheetMetalPtr &model)
    : sheetMetalFeature{model}, redis{redis}
{
    sequenceImpl_ = std::make_shared<BendSequenceGenerator::Sequence>(chromosome);
};

double BendSequenceGenerator::Sequence::cal_fitness(SheetMetalPtr& sheetMetalFeature)
{
    int len = targetSize; 
    double parallel = 1, equality = 1, direction = 1, fitness = 0, seqDistance = 0.0, pairwiseFitness;

    for (int i = 0; i < len; i++)
    { 
        if ((i + 1) < len)
        {
            if (cache.find({chromosome[i], chromosome[i + 1]}) != cache.end())
            {
                fitness += cache[{chromosome[i], chromosome[i + 1]}].first;
                distance += cache[{chromosome[i], chromosome[i + 1]}].second;
                continue;
            }
            else
            {
            // Add 15 if the current and next bend have the same bend angle
                if (sheetMetalFeature->isSameAngle(chromosome[i], chromosome[i + 1]))
                {
                equality = 10;
                }
                else
                {
                ++nTools;
            }
        
            // Compute the distance between the current and next bend
            double dist = sheetMetalFeature->distance(chromosome[i], chromosome[i + 1]);

                if (dist != 0)
                {
                seqDistance = sheetMetalFeature->distance(chromosome[i], chromosome[i + 1]);
            }             
        
                if (sheetMetalFeature->isSameDirection(chromosome[i], chromosome[i + 1]))
                {
                direction = 30;
                }
                else
                {
                ++nFlips;
            }            
       
            // Add 20 to the fitness if the current and next bend are parallel
                if (sheetMetalFeature->isParallel(chromosome[i], chromosome[i + 1]))
                {
                parallel = 20;
                }
                else
                {
                ++nRotations;
            }
        } 

            pairwiseFitness = parallel * equality * direction / (seqDistance * nFlips * nRotations + 1);
            fitness += pairwiseFitness;
  
        distance += seqDistance;

            cache.insert({{chromosome[i], chromosome[i + 1]}, {pairwiseFitness, seqDistance}});

            parallel = 1;
            equality = 1;
            direction = 1;
            seqDistance = 0.0;
        }
    }

    return fitness;
}

void BendSequenceGenerator::generateBendingSequence()
{   
    std::vector<int> initialSequence = sequenceImpl_->chromosome;

    if (initialSequence.size() == 1 || initialSequence.size() == 2)
    {
        return ;
    }

    // current generation 
    int generation = 0;   
    std::vector<Sequence> population; 

    size_t POPULATION_SIZE = Fxt::Computation::computeFactorial(initialSequence.size());

    // create initial population 
    int arr[initialSequence.size()];

    for (size_t i = 0; i < initialSequence.size(); i++)
    {
        arr[i] = initialSequence[i];
    }
    
            genomes = restoreGenome(*val);
    findPermutations(arr, initialSequence.size(), genomes);

            auto value = save(genomes);
    { 
        population.push_back({ Sequence(genomes[i]) });
        population[i].fitness = population[i].cal_fitness(sheetMetalFeature);
    }    

    // // sort the population in increasing order of fitness score 
    std::sort(population.begin(), population.end());

    int n = 0;   
    while (n < 10)
    {   
        std::cout<< "Generation: " << n << "\t"; 
        std::cout<< "String: " ;
        for(auto& c : population[n].chromosome)
        	std::cout<< c << " ";
        std::cout <<"\t"; 
        std::cout << "Fitness: "<< population[n].fitness << "  "<< "Flips: "<< population[n].nFlips;
        std::cout << " Tools: " << population[n].nTools;
        std::cout << " Rotations: " << population[n].nRotations;
        std::cout << " Distance: " << population[n].distance << "\n"; 
  
        ++n; 
     }


    int bestSeq = 0;
    sequenceImpl_->chromosome = population[bestSeq].chromosome;
    sequenceImpl_->fitness = population[bestSeq].fitness;
    sequenceImpl_->nTools = population[bestSeq].nTools;
    sequenceImpl_->nFlips = population[bestSeq].nFlips;
    sequenceImpl_->nRotations = population[bestSeq].nRotations;
    sequenceImpl_->distance = population[bestSeq].distance;
    sequenceImpl_->targetSize = population[bestSeq].targetSize;
    sequenceImpl_->nModules = sheetMetalFeature->getNumberOfModules();
}

std::vector<int> BendSequenceGenerator::getSequence() { return sequenceImpl_->chromosome; }

size_t BendSequenceGenerator::getSequenceSize() { return sequenceImpl_->chromosome.size(); }
    
void BendSequenceGenerator::print()
{
    std::cout << "======================================================================";
    std::cout << "==============================" << std::endl;
    std::cout << "======================================================================";
    std::cout << "==============================" << std::endl;
    std::cout<< "Best Sequence: " << "\t"; 
    for(auto& c : sequenceImpl_->chromosome) {
        std::cout<< c << " ";
        std::cout <<"\t"; 
    }
    std::cout<< "Fitness : "<< sequenceImpl_->fitness << "\n";
    std::cout<< "No of tools : "<< sequenceImpl_->nTools << "\n";
    std::cout<< "No of rotations : "<< sequenceImpl_->nRotations << "\n";
    std::cout<< "No of flips : "<< sequenceImpl_->nFlips << "\n";
    std::cout<< "Distance : "<< sequenceImpl_->distance << "\n";
    std::cout<< "Target size : "<< sequenceImpl_->targetSize << "\n";
    std::cout << "======================================================================";
    std::cout << "==============================" << std::endl;
    std::cout << "======================================================================";
    std::cout << "==============================" << std::endl;
}

bool BendSequenceGenerator::Sequence::operator<(const BendSequenceGenerator::Sequence& individualSequence)
{
    return (this->fitness > individualSequence.fitness);
}

bool BendSequenceGenerator::Sequence::operator==(const BendSequenceGenerator::Sequence& individualSequence)
{
    return std::equal(cbegin(this->chromosome), cend(this->chromosome), cbegin(individualSequence.chromosome));
}

size_t BendSequenceGenerator::getNumberOfTools() { return sequenceImpl_->nTools; }

size_t BendSequenceGenerator::getNumberOfRotations() { return sequenceImpl_->nRotations; }

size_t BendSequenceGenerator::getNumberOfFlips() { return sequenceImpl_->nFlips; }

size_t BendSequenceGenerator::getNumberOfModules() { return sequenceImpl_->nModules; }

double BendSequenceGenerator::getNumberOfDistance() { return sequenceImpl_->distance; }