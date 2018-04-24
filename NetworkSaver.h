#pragma once

#include <fstream>
#include <limits>
#include <iostream>
#include <string>
#include <vector>

class NetworkSaver
{
public:
	static bool saveNetwork(std::string data, std::string filename);
	static std::vector<std::vector<double>> loadNetwork(std::string filename);

private:
	// Loading
	std::vector<std::string> split(const std::string & text, char sep);
	std::vector<std::vector<double>> readVecVecDouble(std::string filename);

	// Saving
	int getNumDigitsInMaxDouble();
	std::string doubleArrToString(double data[], int width);
	std::string doubleVecToString(std::vector<double> data);
	bool saveVecVecDouble(std::vector<std::vector<double>> data, std::string filename);
	bool saveString(std::string data, std::string filename);

	// Display Functions
	void printVecVecDouble(std::vector<std::vector<double>> data);
};