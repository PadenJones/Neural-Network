#include "Trainer.h"

/******************************************************************************
* Train (overloaded) [below] - trains a specified network
******************************************************************************/
void Trainer::train(
   Network* network,
   int iterations,
   int reinforcement,
   double learningRate,
   int debugLevel,
   bool save,
   std::string filename)
{
   validateDimensions(network, true);
   network->trainingRound(inputs, outputs, iterations, reinforcement, learningRate, debugLevel, save, filename);
}

// (overloaded) [base]
bool Trainer::train(Network* network, TrainingData td)
{
   readFile(td.trainingSet);

   if (!validateDimensions(network, false))
   {
      return false;
   }

   train(network, td.iterations, td.reinforcement, td.learningRate, td.debugLevel, td.save, td.saveFile);

   return true;
}


/******************************************************************************
* Test Inputs - tests a networks IO values based on a set of training
*  data
******************************************************************************/
void Trainer::testInputs(Network* network)
{
   if (!validateDimensions(network, false))
   {
      std::cout << "   Could not test inputs, the network dimensions invalid." << std::endl;
      return;
   }

   bool flag;
   for (int i = 0; i < inputs.size(); i++)
   {
      network->input(inputs[i]);
      std::cout << "\n\n";
      std::cout << "   Input: " << printVecDouble(inputs[i], true) << std::endl;
      std::cout << "  Output: " << printVecDouble(network->output(), false) << std::endl;
      std::cout << "Expected: " << printVecDouble(outputs[i], false) << std::endl;
      std::cout << "  Status: ";

      flag = true;
      std::vector<double> netOuts = network->output();
      for (int j = 0; j < outputs[i].size(); j++)
      {
         double threshold = .2;
         flag *= (outputs[i][j] > netOuts[j] - threshold && outputs[i][j] < netOuts[j] + threshold);
      }
      std::string status = (flag) ? "Good" : "Bad";
      std::cout << status << std::endl;
   }
}


/******************************************************************************
* Print Vec Doube - helper function - nicely writes a vector of doubles to
*  a string
******************************************************************************/
std::string Trainer::printVecDouble(std::vector<double> data, bool round)
{
   std::string str = "[";

   for (int i = 0; i < data.size(); i++)
   {
      str += round ? std::to_string((int)std::round(data[i])) : std::to_string(data[i]).substr(0, 4);
      str += i != data.size() - 1 ? ", " : "]";
   }

   return str;
}


/******************************************************************************
* Validate Dimensions - verifies that a networks dimensions math the
*  of the training data
******************************************************************************/
bool Trainer::validateDimensions(Network * network, bool overwrite)
{
   if (dimensions.size() == 0 || network->dimensions.size() == 0)
   {
      return false;
   }
   else if (dimensions.size() != network->dimensions.size())
   {
      if (overwrite)
      {
         network->clear();
         network->create(dimensions);
         return true;
      }
      else
      {
         return false;
      }
   }
   else
   {
      int offset = 0;
      for (int i = 0; i < dimensions.size(); i++)
      {
         offset = (i != dimensions.size() - 1) ? 1 : 0;

         if (network->dimensions[i] - offset != dimensions[i])
         {
            if (overwrite)
            {
               network->clear();
               network->create(dimensions);
               i = dimensions.size();
               return true;
            }

            return false;
         }
      }
      return true;
   }
}


/******************************************************************************
* Split - splits CSV line into a vector of strings
******************************************************************************/
std::vector<std::string> Trainer::split(const std::string &text, char sep)
{
   std::vector<std::string> tokens;
   std::size_t start = 0, end = 0;
   while ((end = text.find(sep, start)) != std::string::npos)
   {
      if (end != start)
      {
         tokens.push_back(text.substr(start, end - start));
      }
      start = end + 1;
   }
   if (end != start)
   {
      tokens.push_back(text.substr(start));
   }
   return tokens;
}


/******************************************************************************
* Read File - reads a file into a 2d vector of doubles
******************************************************************************/
void Trainer::readFile(std::string filename)
{
   inputs.clear();
   outputs.clear();
   std::vector<std::vector<double>> data;

   std::string line = "";
   std::ifstream inputFile(filename);
   if (inputFile.is_open())
   {
      while (getline(inputFile, line))
      {
         std::vector<double> doubles;
         std::vector<std::string> entries = split(line, ',');
         for (int i = 0; i < entries.size(); i++)
         {
            if (entries[i][0] != NULL)
            {
               doubles.push_back(std::stof(entries[i]));
            }
         }

         data.push_back(doubles);
      }
   }

   dimensions = std::vector<int>(data[0].begin(), data[0].end());

   data.erase(data.begin());

   for (int i = 0; i < data.size(); i++)
   {
      std::vector<double> tempInput(data[i].begin(), data[i].begin() + dimensions[0]);
      std::vector<double> tempOutput(data[i].begin() + dimensions[0], data[i].end());
      inputs.push_back(tempInput);
      outputs.push_back(tempOutput);
   }
}