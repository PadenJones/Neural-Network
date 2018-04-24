#pragma once

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define TYPE_INPUT 0
#define TYPE_HIDDEN 1
#define TYPE_BIAS 2
#define TYPE_OUTPUT 3

class Neuron
{
public:
	// Variables
	int type;
	std::string id;

	double input;
	double output;

	double target; // Used for backprop
	double error;  // Used for backprop
	double delta;  // Used for backprop

	std::vector<Neuron*> parents;
	std::map<Neuron*, std::map<std::string, double>> parentData;

	std::vector<Neuron*> children;

	// Constructor
	Neuron(std::string id, int type);

	// Book Keeping Methods
	void registerChild(Neuron* child);
	void registerChild(Neuron*, double parentWeight);
	void registerParent(Neuron* parent);
	void registerParent(Neuron* parent, double weight);
	void setParentWeight(Neuron* parent, double weight);

	// Feed Forward Methods
	void setInput(double input);
	void notifyChildren(bool flag);
	void flagParentReady(Neuron* parent, bool flag);
	void calculateOutput();
	double sigmoid(double x);

	// Back Propagation Methods
	double calculateError(double target);
	void calculateNewWeights(double learningRate);
	void updateWeights();

	// Display Methods
	void printParents();
	std::string printForSave();

	// Helper Methods
	double getRandomWeight();
};