#pragma once

#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#include "Network.h"

struct TrainingData {
   std::string trainingSet;
   int iterations;
   int reinforcement;
   double learningRate;
   int debugLevel;
   bool save;
   std::string saveFile;
};

class Trainer
{
public:
   std::vector<int> dimensions;
   std::vector<std::vector<double>> inputs;
   std::vector<std::vector<double>> outputs;

   void train(Network* network, int iterations, int reinforcement, double learningRate, int debugLevel, bool save, std::string filename);
   bool train(Network* network, TrainingData td);
   void testInputs(Network* network);
   std::string printVecDouble(std::vector<double> data, bool round);

   bool validateDimensions(Network* network, bool overwrite);

   std::vector<std::string> split(const std::string & text, char sep);
   void readFile(std::string filename);
};