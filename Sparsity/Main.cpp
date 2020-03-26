#include <fstream>
#include <iostream>
#include "Graph.h"

//gets file name from user and tries to open file, returns false if file can't be opened and true otherwise
bool openFile(std::ifstream& inputFile)
{
	//declaration
	std::string inputName;

	//get file name
	printf("Input file name: ");
	std::getline(std::cin, inputName);

	//try to open file
	inputFile.open(inputName);

	//end if file could not open
	if (!inputFile.is_open())
	{
		printf("Can't open file.\n");
		return false;
	}

	//file opened
	return true;
}

//gets k from user, throws exception if k < 1 or non-int
int getK()
{
	//declarations
	int k;
	std::string line;

	//get k
	printf("Input k: ");
	std::getline(std::cin, line);
	k = std::stoi(line);

	//k must be positive
	if (k < 1)
	{
		throw std::out_of_range("Parameter k must be positive.");
	}

	//return k
	return k;
}

//gets l from user, throws exception if non-int
int getL()
{
	//declarations
	int l;
	std::string line;

	//get l
	printf("Input l: ");
	std::getline(std::cin, line);
	l = std::stoi(line);

	//return l
	return l;
}

//checks sparsity and tightness using Kiraly's algorithm (https://web.cs.elte.hu/egres/qp/egresqp-19-04.pdf)
std::string checkSparsityWithNegativeL(std::ifstream& inputFile, int k, int l)
{
	//TODO
	return "WIP";
}

//checks sparsity and tightness using Lee and Streinu's component pebble game (https://www.sciencedirect.com/science/article/pii/S0012365X07005602)
std::string checkSparsityWithNonNegativeL(std::ifstream& inputFile, int k, int l)
{
	//TODO
	return "WIP";
}

//shows result
void checkSparsity(std::ifstream& inputFile, int k, int l)
{
	//declaration
	std::string line;

	//print result
	printf("Result: %s\nPress Enter to finish.", (l < 0) ? (checkSparsityWithNegativeL(inputFile, k, l).c_str()) : (checkSparsityWithNonNegativeL(inputFile, k, l).c_str()));

	//wait until user presses enter
	std::getline(std::cin, line);
}

//handles errors, closes file
int main(int argc, char** argv)
{
	//declarations
	int k;
	int l;
	std::ifstream inputFile;

	//try to open file, end if file could not open
	if (!openFile(inputFile))
	{
		return -1;
	}

	//get k and l, end if invalid
	try
	{
		//get k and l
		k = getK();
		l = getL();
	}
	catch (std::invalid_argument exception)
	{
		//not int
		printf("Invalid argument.\n");
		return -2;
	}
	catch (std::out_of_range exception)
	{
		//out of range
		printf("Out of range.\n");
		return -3;
	}

	//run algorithm, close file, end
	checkSparsity(inputFile, k, l);
	inputFile.close();
	return 0;
}