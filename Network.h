#pragma once

#include <iomanip>
#include <iostream>
#include <string>
#include <string>
#include <vector>

#include <math.h>

#include "NetworkSaver.h"
#include "Neuron.h"

#define TYPE 0
#define INPUT 1
#define OUTPUT 2
#define TARGET 3
#define ERROR 4
#define DELTA 5

#ifndef TYPE_INPUT
#define TYPE_INPUT 0
#define TYPE_HIDDEN 1
#define TYPE_BIAS 2
#define TYPE_OUTPUT 3
#endif

class Network
{
public:
	// Member Variables
	std::vector<int> dimensions;
	std::vector<std::vector<Neuron*>> network;
	double totalError;

	// Constructor
	Network();

	// Build
	void init(std::vector<int> dimensions, bool createBias);
	void setWeights(std::vector<std::vector<std::vector<double>>> weights);

	// IO
	void input(std::vector<double> inputs);
	std::vector<double> output();
	std::vector<bool> boolOutput();
	void trainingRound(std::vector<std::vector<double>> inputs, std::vector<std::vector<double>> outputs, int iterations, int reinforcement, double learningRate, int debugLevel, bool save, std::string filename);

	// Training
	void train(std::vector<double>targets, double learningRate);
	double calculateTotalError(std::vector<double> targets);
	void calculateNewWeights(double learningRate);
	void updateWeights();

	// Create/Load/Save
	void create(std::vector<int> dimensions);
	bool load(std::string filename);
	bool save(std::string filename);

	void clear();

	// Display Functions
	std::string Network::printToString();
	void printParents();
	void stat();

	// Helper Functions
	static std::vector<std::vector<std::vector<double>>> buildWeightTable(int inputCount, int hiddenCount, int outputCount);

	// Destructor
	~Network();
};