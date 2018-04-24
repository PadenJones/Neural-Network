#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>

#include "Network.h"
#include "Trainer.h"

Network* network = new Network();
Trainer trainer;

#define SEVENTHS "sevenths.csv"
#define XOR "xor.csv"

int main()
{
	network->create({ 2, 2, 1 });
	trainer.readFile(XOR);
	trainer.testInputs(network);
	trainer.train(network, 100, 5, .3, 0, false, "");
	trainer.testInputs(network);
	
	//network->load("xor.net");           // Net Save
	//trainer.readFile(std::string(XOR)); // Training Data
	//trainer.testInputs(network);
	//trainer.train(network, 5000, 5, .5, 0, true, "xor.net");

	// network (needs to have net saved loaded)

	// 1. network->load("netsave.net");
	// 2. trainer.readFile("trainingdata.td");
	// 3. trainer.train(network,...);

	delete network;
	return 0;
}