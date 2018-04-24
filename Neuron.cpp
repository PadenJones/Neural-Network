#include "Neuron.h"

/******************************************************************************
* Constructor
******************************************************************************/
Neuron::Neuron(std::string id, int type)
{
	srand(time(0));
	this->id = id;
	this->type = type;
	this->input = 0;
	this->output = type == TYPE_BIAS;

	this->target = 0;
	this->error = 0;
	this->delta = 0;
}


/******************************************************************************
* Book Keeping Methods
******************************************************************************/
void Neuron::registerChild(Neuron* child)
{
	registerChild(child, getRandomWeight());
}

void Neuron::registerChild(Neuron* child, double parentWeight)
{
	child->registerParent(this, parentWeight);
}

void Neuron::registerParent(Neuron* parent)
{
	registerParent(parent, getRandomWeight());
}

void Neuron::registerParent(Neuron* parent, double parentWeight)
{
	parents.push_back(parent);
	setParentWeight(parent, parentWeight);
	parentData[parent]["flag"] = parent->type == TYPE_BIAS;

	parent->children.push_back(this);
}

void Neuron::setParentWeight(Neuron* parent, double parentWeight)
{
	parentData[parent]["weight"] = parentWeight;
}



/******************************************************************************
* Feed Forward Methods
******************************************************************************/
void Neuron::setInput(double input)
{
	if (type == TYPE_INPUT)
	{
		this->input = input;
		this->output = input;
		notifyChildren(true);
	}
}

void Neuron::notifyChildren(bool flag)
{
	for (int i = 0; i < children.size(); i++)
	{
		children[i]->flagParentReady(this, flag);
	}
}

void Neuron::flagParentReady(Neuron* parent, bool flag)
{
	parentData[parent]["flag"] = (flag) ? 1 : 0;

	bool ready = true;
	for (int i = 0; i < parents.size(); i++)
	{
		ready *= parentData[parents[i]]["flag"];
	}

	if (ready)
	{
		calculateOutput();

		for (int i = 0; i < parents.size(); i++)
		{
			if (parents[i]->type != TYPE_BIAS)
			{
				parentData[parents[i]]["flag"] = 0;
			}
		}
	}
}

void Neuron::calculateOutput()
{
	double total = 0;

	for (int i = 0; i < parents.size(); i++)
	{
		total += parents[i]->output * parentData[parents[i]]["weight"];
	}

	this->output = sigmoid(total);

	notifyChildren(true);
}

double Neuron::sigmoid(double x)
{
	double e = 2.71828182845904523536028747135266249;
	return 1 / (1 + pow(e, -x));
}



/******************************************************************************
* Back Propagation Methods
******************************************************************************/
double Neuron::calculateError(double target)
{
	this->target = target;
	
	error = (1.0 / 2.0) * pow(target - output, 2);

	if (type == TYPE_OUTPUT)
	{
		double dTotaldError = output - target;
		double dOutdNet = output * (1 - output);

		delta = dTotaldError * dOutdNet;
		
		return error;
	}

	if (type == TYPE_HIDDEN) {
		return 0;
	}
}

void Neuron::calculateNewWeights(double learningRate)
{
	if (type == TYPE_OUTPUT)
	{
		double dTotaldWeight = 0;
		double oldWeight = 0;

		for (int i = 0; i < parents.size(); i++)
		{
			dTotaldWeight = parents[i]->output * delta;
			oldWeight = parentData[parents[i]]["weight"];
			parentData[parents[i]]["futureWeight"] = oldWeight - (learningRate * dTotaldWeight);
		}

		return;
	}

	// Todo: this will not work for hidden neurons with hidden neuron children,
	// their deltas are never set...
	if (type == TYPE_HIDDEN)
	{
		double dEdOut = 0;
		for (int i = 0; i < children.size(); i++)
		{
			// Sum of children's (delta * weight)
			dEdOut += children[i]->parentData[this]["weight"] * children[i]->delta;
		}
		
		double dOutdNet = output * (1 - output);

		double dEdW = 0;
		for (int i = 0; i < parents.size(); i++)
		{
			dEdW = dEdOut * dOutdNet * parents[i]->output;
			parentData[parents[i]]["futureWeight"] = parentData[parents[i]]["weight"] - (learningRate * dEdW);
		}

		return;
	}
}

void Neuron::updateWeights()
{
	if (type == TYPE_OUTPUT || type == TYPE_HIDDEN)
	{
		for (int i = 0; i < parents.size(); i++)
		{
			parentData[parents[i]]["weight"] = parentData[parents[i]]["futureWeight"];
			parentData[parents[i]]["futureWeight"] = 0;
		}
	}
}



/******************************************************************************
* Display Methods
******************************************************************************/
void Neuron::printParents()
{
	std::cout << "Neuron " << id << " Parents:" << std::endl;
	for (int i = 0; i < parents.size(); i++)
	{
		std::cout << "\tNeuron " << parents[i]->id << " - val: " << parents[i]->output
			<< " | weight: " << parentData[parents[i]]["weight"]
			<< std::endl;
	}
}

std::string Neuron::printForSave()
{
	std::string str = "";

	str += std::to_string((double)type) + " ";
	str += std::to_string(input) + " ";
	str += std::to_string(output) + " ";
	str += std::to_string(target) + " ";
	str += std::to_string(error) + " ";
	str += std::to_string(delta) + std::string((parents.size() > 0)? " " : "");

	int size = parents.size();
	for (int i = 0; i < size; i++)
	{
		str += std::to_string(parentData[parents[i]]["weight"]) + " ";
		str += std::to_string(parentData[parents[i]]["futureWeight"]) + " ";
		str += std::to_string(parentData[parents[i]]["flag"]) + std::string((i != size - 1) ? " " : "");
	}

	return str;
}



/******************************************************************************
* Helper Functions
******************************************************************************/
double Neuron::getRandomWeight()
{
	return ((double)(rand() % 200) / (100.0) - 1);
}