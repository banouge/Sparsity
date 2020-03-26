#include <fstream>
#include <iostream>
#include <string>

//gets file name from user
std::string getFileName()
{
	//declaration
	std::string line;

	//get file name
	printf("Input file name: ");
	std::getline(std::cin, line);

	//return file name
	return line;
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

//opens/closes input file
int main(int argc, char** argv)
{
	//declarations
	int k;
	int l;
	std::ifstream inputFile;

	//try to open file
	inputFile.open(getFileName());

	//end if file could not open
	if (!inputFile.is_open())
	{
		printf("Can't open file.\n");
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

	//close file and end
	inputFile.close();
	return 0;
}