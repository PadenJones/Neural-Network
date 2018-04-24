#include "Network.h"

/******************************************************************************
* Constructor - doesn't do much...
******************************************************************************/
Network::Network()
{
	srand(time(NULL));
	this->totalError = 0;
}


/******************************************************************************
* Init - creates the network based on given dimensions.
******************************************************************************/
void Network::init(std::vector<int> dimensions, bool createBias)
{
	this->clear();

	// Increment dimensions to make room for bias neurons
	this->dimensions = dimensions;

	if (createBias)
	{
		for (int i = 0; i < dimensions.size(); i++)
		{
			dimensions[i] += !(i == dimensions.size() - 1);
			this->dimensions[i] = dimensions[i];
		}
	}

	// Create Neurons
	for (int i = 0; i < dimensions.size(); i++)
	{
		std::vector<Neuron*> tempLayer;

		int type = 0;
		for (int j = 0; j < dimensions[i]; j++)
		{
			if (i == 0)
			{
				type = TYPE_INPUT;
			}
			else if (i < dimensions.size() - 1)
			{
				type = TYPE_HIDDEN;
			}
			else if (i == dimensions.size() - 1)
			{
				type = TYPE_OUTPUT;
			}
			std::string name = "[" + std::to_string(i) + ", " + std::to_string(j) + "]";

			if (j == dimensions[i] - 1 && type != TYPE_OUTPUT)
			{
				type = TYPE_BIAS;
				name = "[" + std::to_string(i) + ", b]";
			}

			tempLayer.push_back(new Neuron(name, type));
		}

		network.push_back(tempLayer);
	}

	// Register Parents
	for (int i = 0; i < dimensions.size(); i++)
	{
		for (int j = 0; j < dimensions[i]; j++)
		{
			if (network[i][j]->type != TYPE_INPUT && network[i][j]->type != TYPE_BIAS)
			{
				for (int k = 0; k < dimensions[i - 1]; k++)
				{
					network[i][j]->registerParent(network[i - 1][k]);
				}
			}
		}
	}
}


/******************************************************************************
* Set Weights - accepts a vector<vector<double>> of weights and assigns
*	the associated weights to a given neuron's parents.
******************************************************************************/
void Network::setWeights(std::vector<std::vector<std::vector<double>>> weights)
{
	for (int i = 0; i < dimensions.size(); i++)
	{
		for (int j = 0; j < dimensions[i]; j++)
		{
			if (network[i][j]->type != TYPE_INPUT && network[i][j]->type != TYPE_BIAS)
			{
				for (int k = 0; k < network[i][j]->parents.size(); k++)
				{
					network[i][j]->setParentWeight(network[i - 1][k], weights[i][j][k]);
				}
			}
		}
	}
}


/******************************************************************************
* Input - takes a vector of inputs and sends the values to the input neurons.
******************************************************************************/
void Network::input(std::vector<double> inputs)
{
	for (int i = 0; i < dimensions[0]; i++)
	{
		if (network[0][i]->type == TYPE_INPUT)
		{
			network[0][i]->setInput(inputs[i]);
		}
	}
}


/******************************************************************************
* Output - returns a vector of the network's output values.
******************************************************************************/
std::vector<double> Network::output()
{
	std::vector<double> outputs;

	int outputLayer = dimensions.size() - 1;
	for (int i = 0; i < dimensions[outputLayer]; i++)
	{
		outputs.push_back(network[outputLayer][i]->output);
	}

	return outputs;
}

/******************************************************************************
* Bool Output - converts output values to 1's or 0's
******************************************************************************/
std::vector<bool> Network::boolOutput()
{
	std::vector<bool> outputs;

	int outputLayer = dimensions.size() - 1;
	for (int i = 0; i < dimensions[outputLayer]; i++)
	{
		outputs.push_back(round(network[outputLayer][i]->output));
	}

	return outputs;
}


/******************************************************************************
* Training Round - multiple training rounds, uses training algorithm.
******************************************************************************/
void Network::trainingRound(
	std::vector<std::vector<double>> inputs,
	std::vector<std::vector<double>> outputs,
	int iterations,
	int reinforcement,
	double learningRate,
	int debugLevel,
	bool saveWeights,
	std::string filename
)
{
	int roll = 0;
	double subsetError = 0;
	double averageError = 1;
	std::vector<int> stats;
	for (int i = 0; i < inputs.size(); i++)
	{
		stats.push_back(0);
	}

	for (int i = 0; i < iterations; i++)
	{
		subsetError = 0;
		roll = rand() % inputs.size();

		stats[roll]++;

		for (int j = 0; j < reinforcement; j++)
		{
			input(inputs[roll]);
			train(outputs[roll], learningRate);
			subsetError += totalError;
		}

		subsetError /= reinforcement;

		averageError -= averageError / (i + 1.0);
		averageError += subsetError / (i + 1.0);

		if (debugLevel > 0)
		{
			std::cout << "Roll: " << std::setw(3) << roll <<
				std::setprecision(4) << std::fixed <<
				" | Set Error: " << subsetError <<
				" | Average Error: " << averageError <<
				std::string((subsetError > averageError) ? " - Higher" : " - Lower") <<
				std::endl;

			std::cout << "Frequencies - [";
			for (int j = 0; j < inputs.size(); j++)
			{
				std::cout << stats[j];

				if (j != inputs.size() - 1)
				{
					std::cout << ", ";
				}
				else
				{
					std::cout << "]\n\n";
				}
			}
		}
	}

	if (saveWeights)
	{
		save(filename);
	}
}


/******************************************************************************
* Train - implementation of backpropagation training
******************************************************************************/
void Network::train(std::vector<double> targets, double learningRate)
{
	calculateTotalError(targets);
	calculateNewWeights(learningRate);
	updateWeights();
}


/******************************************************************************
* Calculate Total Error - sums the errors for each output neuron.
******************************************************************************/
double Network::calculateTotalError(std::vector<double> targets)
{
	double error = 0;

	int outputLayer = dimensions.size() - 1;
	for (int i = 0; i < dimensions[outputLayer]; i++)
	{
		error += network[outputLayer][i]->calculateError(targets[i]);
	}

	this->totalError = error;

	return error;
}


/******************************************************************************
* Calculate New Weights - reverse iterates through the network calculating
*	the new weights for each relationship.
******************************************************************************/
void Network::calculateNewWeights(double learningRate)
{
	int outputLayer = dimensions.size() - 1;
	for (int i = outputLayer; i > -1; i--)
	{
		for (int j = 0; j < dimensions[i]; j++)
		{
			if (network[i][j]->type != TYPE_INPUT || network[i][j]->type != TYPE_BIAS)
			{
				network[i][j]->calculateNewWeights(learningRate);
			}
		}
	}
}


/******************************************************************************
* Update Weights - sets a neuron's parent weights to the future
*	weight values.
******************************************************************************/
void Network::updateWeights()
{
	int outputLayer = dimensions.size() - 1;
	for (int i = outputLayer; i > -1; i--)
	{
		for (int j = 0; j < dimensions[i]; j++)
		{
			if (network[i][j]->type != TYPE_INPUT || network[i][j]->type != TYPE_BIAS)
			{
				network[i][j]->updateWeights();
			}
		}
	}
}


/******************************************************************************
* Create - creates a network of given dimensions with random values.
******************************************************************************/
void Network::create(std::vector<int> dimensions)
{
	init(dimensions, true);
}


/******************************************************************************
* Load - reads in network values from a file and creates a new network.
******************************************************************************/
bool Network::load(std::string filename)
{
	clear();

	std::vector<std::vector<double>> savedNet = NetworkSaver::loadNetwork(filename);
	if (savedNet.size() > 0)
	{
		std::vector<int> dimensions(savedNet[0].begin(), savedNet[0].end());

		// Create Network (creates bias neurons as well)
		init(dimensions, false);

		// Restore Neuron Values (restores neuron types)
		int savedIndex = 1;
		for (int i = 0; i < savedNet[0].size(); i++)
		{
			for (int j = 0; j < savedNet[0][i]; j++)
			{
				network[i][j]->type = savedNet[savedIndex][TYPE];
				network[i][j]->input = savedNet[savedIndex][INPUT];
				network[i][j]->output = savedNet[savedIndex][OUTPUT];
				network[i][j]->target = savedNet[savedIndex][TARGET];
				network[i][j]->error = savedNet[savedIndex][ERROR];
				network[i][j]->delta = savedNet[savedIndex][DELTA];
				savedIndex++;
			}
		}

		// Restore Weights
		// TODO: fix real hacky way of implmenting dimensions (only supports 3 level networks..)
		std::vector<std::vector<std::vector<double>>> weights = buildWeightTable(dimensions[0], dimensions[1], dimensions[2]);
		savedIndex = 1;
		int parentIndex = 0;
		Neuron* target;
		Neuron* parent;
		for (int i = 0; i < savedNet[0].size(); i++)
		{
			for (int j = 0; j < savedNet[0][i]; j++)
			{
				if (savedNet[savedIndex].size() > 6)
				{
					for (int k = 6; k < savedNet[savedIndex].size(); k += 3)
					{
						parentIndex = (k - 6) / 3;
						target = network[i][j];
						parent = target->parents[parentIndex];

						target->parentData[parent]["weight"] = savedNet[savedIndex][k];
						target->parentData[parent]["futureWeight"] = savedNet[savedIndex][k + 1];
						target->parentData[parent]["flag"] = savedNet[savedIndex][k + 2];
					}
				}
				savedIndex++;
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}


/******************************************************************************
* Save - writes the current network to a file.
******************************************************************************/
bool Network::save(std::string filename)
{
	return NetworkSaver::saveNetwork(printToString(), filename);
}


/******************************************************************************
* Clear - destroys the network
******************************************************************************/
void Network::clear()
{
	for (int i = 0; i < dimensions.size(); i++)
	{
		for (int j = 0; j < dimensions[i]; j++)
		{
			delete network[i][j];
		}
	}

	network.clear();
	dimensions.clear();
}

/******************************************************************************
* Print to String - compacts the network to a list of values which
*	can be used to save/restore network state.
******************************************************************************/
std::string Network::printToString()
{
	std::string str = "";

	int size = dimensions.size();
	for (int i = 0; i < size; i++)
	{
		str += std::to_string((double)dimensions[i]) + ((i != size - 1) ? std::string(" ") : std::string("\n"));
	}

	size = dimensions.size();
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < dimensions[i]; j++)
		{
			str += network[i][j]->printForSave() + "\n";
		}
	}

	return str;
}


/******************************************************************************
* Print Parents - prints the parents of any neuron.
******************************************************************************/
void Network::printParents()
{
	for (int i = 0; i < dimensions.size(); i++)
	{
		for (int j = 0; j < dimensions[i]; j++)
		{
			network[i][j]->printParents();
		}
	}
}


/******************************************************************************
* Stat - prints the network stats to console
******************************************************************************/
void Network::stat()
{
	std::cout << "Dimensions: " << dimensions.size() << " - ";

	if (dimensions.size() >= 3)
	{
		std::cout << "Input: " << dimensions[0]
			<< " | Hidden: " << dimensions[1]
			<< " | Output: " << dimensions[2]
			<< std::endl;
	}
	else if (dimensions.size() >= 2)
	{
		std::cout << "Input: " << dimensions[0]
			<< " | Output: " << dimensions[1]
			<< std::endl;
	}
	else if (dimensions.size() >= 1)
	{
		std::cout << "Input: " << dimensions[0] << std::endl;
	}
	else if (dimensions.size() >= 0)
	{
		std::cout << "Empty Network..." << std::endl;
	}
}


/******************************************************************************
* Build Weight Table - returns an empty vector which is to be used to
*	set neuron weights.
******************************************************************************/
std::vector<std::vector<std::vector<double>>>
Network::buildWeightTable(int inputCount, int hiddenCount, int outputCount)
{
	std::vector<std::vector<std::vector<double>>> weights;
	for (int i = 0; i < inputCount + 1; i++)
	{
		std::vector<std::vector<double>> layer;
		for (int j = 0; j < hiddenCount + 1; j++)
		{
			std::vector<double> neuron;
			for (int k = 0; k < outputCount; k++)
			{
				neuron.push_back(0);
			}
			layer.push_back(neuron);
		}
		weights.push_back(layer);
	}

	return weights;
}


/******************************************************************************
* Destructor - deallocates neurons created with the new command.
******************************************************************************/
Network::~Network()
{
	for (int i = 0; i < dimensions.size(); i++)
	{
		for (int j = 0; j < dimensions[i]; j++)
		{
			delete network[i][j];
		}
	}
}