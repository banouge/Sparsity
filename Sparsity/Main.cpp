#include <fstream>
#include <iostream>
#include <unordered_map>
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

//gets next edge from file, adds edge to set, creates and adds vertices to map if necessary
Edge* getNextEdge(std::ifstream& inputFile, std::unordered_map<std::string, Vertex*>& vertices, std::unordered_set<Edge*>& edges, int k)
{
	//declarations
	Vertex* origin;
	Vertex* destination;
	std::string line = "";

	//get line
	std::getline(inputFile, line);

	//return null if at end
	if (line.size() == 0)
	{
		return nullptr;
	}

	//get vertex names
	std::size_t posComma = line.find(',');
	std::string originName = line.substr(0, posComma);
	std::string destinationName = line.substr(posComma + 1);

	//get or create origin and destination
	origin = (vertices.count(originName)) ? (vertices.at(originName)) : (vertices.emplace(originName, new Vertex(originName, k)).first->second);
	destination = (vertices.count(destinationName)) ? (vertices.at(destinationName)) : (vertices.emplace(destinationName, new Vertex(destinationName, k)).first->second);

	//add edge to set, return edge
	return *edges.emplace(new Edge(origin, destination)).first;
}

//deletes vertices and edges
void deleteVerticesAndEdges(std::unordered_map<std::string, Vertex*>& vertices, std::unordered_set<Edge*>& edges)
{
	//delete vertices
	for (std::pair<std::string, Vertex*> vertex : vertices)
	{
		delete vertex.second;
	}

	//delete edges
	for (Edge* edge : edges)
	{
		delete edge;
	}
}

//attempts to remove edge in Kiraly's algorithm, returns true if successful and false if too many have been removed
bool removeEdge(int& numRemovableEdges, bool& isEdgeHandled)
{
	//check how many edges can be removed
	if (numRemovableEdges-- > 0)
	{
		//remove edge from consideration
		isEdgeHandled = true;
		return true;
	}
	else
	{
		//had to remove too many edges
		return false;
	}
}

//attempt to add edge to digraph in Kiraly's algorithm, returns true if edge is accepted and false otherwise
bool addEdge()
{
	//TODO, SquareWithSlash.csv is (1, -1)-tight and (2, -1)-sparse
	return true;
}

//checks sparsity and tightness using Kiraly's algorithm (https://web.cs.elte.hu/egres/qp/egresqp-19-04.pdf)
std::string checkSparsityWithNegativeL(std::ifstream& inputFile, int k, int l)
{
	//declarations
	std::unordered_map<std::string, Vertex*> vertices;
	std::unordered_set<Vertex*> labeledVertices;
	std::unordered_set<Edge*> edges;
	std::unordered_set<Edge*> acceptedEdges;
	int numRemovableEdges = -l;

	//iterate through edges
	while (Edge* edge = getNextEdge(inputFile, vertices, edges, k))
	{
		//declaration
		bool isEdgeHandled = false;

		//repeat until edge is handled completely
		while (!isEdgeHandled)
		{
			//check if both vertices are labeled
			if (labeledVertices.count(edge->ORIGIN) && labeledVertices.count(edge->DESTINATION))
			{
				//remove edge if possible and return "not sparse" otherwise
				if (!removeEdge(numRemovableEdges, isEdgeHandled))
				{
					return "NOT SPARSE";
				}
			}
			else
			{
				//attempt to add edge to digraph
				isEdgeHandled = addEdge();
			}
		}
	}

	//clean up and return "tight" if |E| = k|V| - l and "sparse" otherwise
	bool isTight = edges.size() == k * vertices.size() - l;
	deleteVerticesAndEdges(vertices, edges);
	return (isTight) ? ("TIGHT") : ("SPARSE");
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