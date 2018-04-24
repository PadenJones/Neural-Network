#include "NetworkSaver.h"

/******************************************************************************
* Save Network - writes a network to a file
******************************************************************************/
bool NetworkSaver::saveNetwork(std::string data, std::string filename)
{
	NetworkSaver ns;
	return ns.saveString(data, filename);
}


/******************************************************************************
* Load Network - reads a network into a 2d vector of doubles
******************************************************************************/
std::vector<std::vector<double>> NetworkSaver::loadNetwork(std::string filename)
{
	NetworkSaver ns;
	return ns.readVecVecDouble(filename);
}


/******************************************************************************
* Get Number of Digits in Max Double - returns the number of digits in the
*	max double when printed using printf
******************************************************************************/
int  NetworkSaver::getNumDigitsInMaxDouble()
{
	double max = std::numeric_limits<double>::max();
	char buffer[1024];
	sprintf_s(buffer, "%f", max);
	std::string str = buffer;
	return str.length();
}


/******************************************************************************
* Double Array to String - converts an array of double to a single
*	line string
******************************************************************************/
std::string NetworkSaver::doubleArrToString(double data[], int width)
{
	std::string str = "";
	int bufferWidth = (getNumDigitsInMaxDouble() + 1) * width;
	char* buffer = new char[bufferWidth];

	int length = 0;
	for (int i = 0; i < width; i++)
	{
		std::string eol = (i != width - 1) ? " " : "\n";
		std::string format = "%.10f" + eol;
		length += snprintf(buffer + length, bufferWidth - length, format.c_str(), data[i]);
	}

	str = buffer;

	delete buffer;
	return str;
}


/******************************************************************************
* Double Vector to String - converts a vector of doubles into a signle
*	line string
******************************************************************************/
std::string  NetworkSaver::doubleVecToString(std::vector<double> data)
{
	std::string str = "";
	int width = data.size();

	double* dataArr = new double[width];

	for (int i = 0; i < width; i++)
	{
		dataArr[i] = data[i];
	}

	str = doubleArrToString(dataArr, width);

	delete dataArr;
	return str;
}


/******************************************************************************
* Split - splits a string into a vector of components based on
*	a deliminator (I do not think this is mine, but I can't remember
*   where it came from...)
******************************************************************************/
std::vector<std::string>  NetworkSaver::split(const std::string &text, char sep) {
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
* Save Vector of Vector<double> - writes a 2d vector of doubles to file
******************************************************************************/
bool  NetworkSaver::saveVecVecDouble(std::vector<std::vector<double>> data, std::string filename)
{
	std::string str = "";

	for (int i = 0; i < data.size(); i++)
	{
		str += doubleVecToString(data[i]) + "\n";
	}

	return saveString(str, filename);
}


/******************************************************************************
* Save String - writes a string to a file
******************************************************************************/
bool NetworkSaver::saveString(std::string data, std::string filename)
{
	std::ofstream outputFile(filename);

	if (outputFile.is_open())
	{
		outputFile << data;
		outputFile.close();
	}
	else
	{
		return false;
	}

	return true;
}


/******************************************************************************
* Read Vector of Vector<double> - reads a 2d vector of doubles from file
******************************************************************************/
std::vector<std::vector<double>>  NetworkSaver::readVecVecDouble(std::string filename)
{
	std::vector<std::vector<double>> data;

	std::string line = "";
	std::ifstream inputFile(filename);
	if (inputFile.is_open())
	{
		int lineCount = 0;

		while (getline(inputFile, line))
		{
			std::vector<double> doubles;

			std::vector<std::string> entries = split(line, ' ');
			for (int i = 0; i < entries.size(); i++)
			{
				doubles.push_back(std::stof(entries[i]));
			}

			data.push_back(doubles);
		}
	}

	return data;
}


/******************************************************************************
* Print Vector of Vector<double> - prints a 2d vector of doubles cleanly
******************************************************************************/
void NetworkSaver::printVecVecDouble(std::vector<std::vector<double>> data)
{
	for (int i = 0; i < data.size(); i++)
	{
		std::cout << "Row: " << i << " - ";

		for (int j = 0; j < data[i].size(); j++)
		{
			std::cout << data[i][j];

			if (j != data[i].size() - 1)
			{
				std::cout << ", ";
			}
			else
			{
				std::cout << std::endl;
			}
		}
	}
}