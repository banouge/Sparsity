#include <fstream>
#include <iostream>
#include <queue>
#include <stack>
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

//gets next edge from file, adds edge to set (but not vertices because edge may be reversed or rejected), creates and adds vertices to map if necessary
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

	//add edge to set and return edge
	return *edges.emplace(new Edge(origin, destination)).first;
}

//adds edge to vertices' edges
void addEdgeToVertices(Edge* edge)
{
	//add edge to vertices
	edge->ORIGIN->addOutEdge(edge);
	edge->DESTINATION->addInEdge(edge);
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
	if (numRemovableEdges--)
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

//initalizes queue and set and map
void initializeBreadthFirstSearch(Edge* edge, std::queue<Vertex*>& sourceQueue, std::unordered_set<Vertex*>& sourceVertices, std::unordered_map<Vertex*, Edge*>& sourceEdges)
{
	//add edge and edge vertices to sources
	sourceQueue.push(edge->DESTINATION);
	sourceQueue.push(edge->ORIGIN);
	sourceVertices.emplace(edge->DESTINATION);
	sourceVertices.emplace(edge->ORIGIN);
	sourceEdges.emplace(edge->DESTINATION, edge);
	sourceEdges.emplace(edge->ORIGIN, edge);
}

//reverses path from source to vertex of edge
void reversePath(Vertex* source, Edge* edge, std::unordered_map<Vertex*, Edge*>& sourceEdges, std::unordered_set<Edge*>& edges)
{
	//declaration
	Edge* reversibleEdge = sourceEdges.at(source);

	//reverse each edge on the path
	while (reversibleEdge != edge)
	{
		//get and add reverse
		Edge* reversedEdge = reversibleEdge->getReverse();
		addEdgeToVertices(reversedEdge);
		edges.emplace(reversedEdge);

		//remove old
		reversibleEdge->ORIGIN->removeOutEdge(reversibleEdge);
		reversibleEdge->DESTINATION->removeInEdge(reversibleEdge);
		edges.erase(reversibleEdge);
		delete reversibleEdge;

		//move on
		reversibleEdge = sourceEdges.at(reversedEdge->ORIGIN);
	}
}

//adds in neighbors of a vertex to breadth-first search
void addInNeighbors(std::queue<Vertex*>& sourceQueue, std::unordered_set<Vertex*>& sourceVertices, std::unordered_map<Vertex*, Edge*>& sourceEdges)
{
	//iterate through in edges
	for (Edge* inEdge : sourceQueue.front()->getInEdges())
	{
		//only add vertices that haven't already been added
		if (!sourceVertices.count(inEdge->ORIGIN))
		{
			sourceQueue.push(inEdge->ORIGIN);
			sourceVertices.emplace(inEdge->ORIGIN);
			sourceEdges.emplace(inEdge->ORIGIN, inEdge);
		}
	}
}

//analyzes paths to vertices of edge, returns true if edge can be added and false otherwise
bool analyzePaths(Edge* edge, std::unordered_set<Edge*>& edges, std::unordered_set<Vertex*>& labeledVertices, int k)
{
	//declarations
	std::queue<Vertex*> sourceQueue;
	std::unordered_set<Vertex*> sourceVertices;
	std::unordered_map<Vertex*, Edge*> sourceEdges;

	//initialize
	initializeBreadthFirstSearch(edge, sourceQueue, sourceVertices, sourceEdges);

	//breadth-first search
	while (!sourceQueue.empty())
	{
		if (sourceQueue.front()->getInDegree() < k) //in degree is below max
		{
			//reverse path to edge vertex, edge can now be added because in degree of one of the edge vertices decreased
			reversePath(sourceQueue.front(), edge, sourceEdges, edges);
			return true;
		}
		else //in degree is at max
		{
			//add in neighbors and move on
			addInNeighbors(sourceQueue, sourceVertices, sourceEdges);
			sourceQueue.pop();
		}
	}

	//label vertices that can reach edge vertices
	for (Vertex* source : sourceVertices)
	{
		labeledVertices.emplace(source);
	}

	//edge can't be added
	return false;
}

//attempt to add edge to digraph in Kiraly's algorithm, returns true if edge is accepted and false otherwise
bool addEdge(Edge* edge, std::unordered_set<Edge*>& edges, std::unordered_set<Vertex*>& labeledVertices, int k)
{
	//declaration
	bool isEdgeHandled = false;

	//repeat until edge is handled
	while (!isEdgeHandled)
	{
		if (edge->DESTINATION->getInDegree() < k) //edge is oriented properly
		{
			//add edge
			addEdgeToVertices(edge);
			return true;
		}
		else if (edge->ORIGIN->getInDegree() < k) //edge is reversed
		{
			//get reverse
			Edge* reversedEdge = edge->getReverse();

			//remove edge
			edges.erase(edge);
			delete edge;

			//add reverse
			edges.emplace(reversedEdge);
			addEdgeToVertices(reversedEdge);
			return true;
		}
		else //more work must be done
		{
			//edge is not handled if it can be added because it has yet to be added, end otherwise
			isEdgeHandled = !analyzePaths(edge, edges, labeledVertices, k);
		}
	}

	//edge could not be added
	return false;
}

//checks sparsity and tightness using Kiraly's algorithm (https://web.cs.elte.hu/egres/qp/egresqp-19-04.pdf)
std::string checkSparsityWithNegativeL(std::ifstream& inputFile, int k, int l)
{
	//declarations
	std::unordered_map<std::string, Vertex*> vertices;
	std::unordered_set<Vertex*> labeledVertices;
	std::unordered_set<Edge*> edges;
	int numRemovableEdges = -l;

	//iterate through edges
	while (Edge* edge = getNextEdge(inputFile, vertices, edges, k))
	{
		//declaration
		bool isEdgeHandled = false;

		//repeat until edge is handled completely
		while (!isEdgeHandled)
		{
			if (labeledVertices.count(edge->ORIGIN) && labeledVertices.count(edge->DESTINATION)) //both vertices are labeled
			{
				//remove edge if possible and clean up and return "not sparse" otherwise
				if (!removeEdge(numRemovableEdges, isEdgeHandled))
				{
					deleteVerticesAndEdges(vertices, edges);
					return "NOT SPARSE";
				}
			}
			else //a vertex is unlabeled
			{
				//attempt to add edge to digraph
				isEdgeHandled = addEdge(edge, edges, labeledVertices, k);
			}
		}
	}

	//clean up and return "tight" if |E| = k|V| - l and "sparse" otherwise
	bool isTight = edges.size() == k * vertices.size() - l;
	deleteVerticesAndEdges(vertices, edges);
	return (isTight) ? ("TIGHT") : ("SPARSE");
}

//accepts edge in Lee and Streinu's component pebble game
void addEdgeToDigraph(Edge* edge, std::unordered_set<Edge*>& edges)
{
	if (edge->ORIGIN->getNumPebbles()) //edge is oriented properly
	{
		//add edge
		edge->ORIGIN->removePebble();
		addEdgeToVertices(edge);
	}
	else //edge is reversed
	{
		//get reverse
		Edge* reversedEdge = edge->getReverse();

		//remove edge
		edges.erase(edge);
		delete edge;

		//add reverse
		edges.emplace(reversedEdge);
		reversedEdge->ORIGIN->removePebble();
		addEdgeToVertices(reversedEdge);
	}
}

//adds out neighbors of vertex to depth-first search
void addOutNeighbors(Vertex* vertex, std::stack<Vertex*>& sourceStack, std::unordered_set<Vertex*>& sourceVertices, std::unordered_map<Vertex*, Edge*>& sourceEdges)
{
	//iterate through in edges
	for (Edge* outEdge : vertex->getOutEdges())
	{
		//only add vertices that haven't already been added
		if (!sourceVertices.count(outEdge->DESTINATION))
		{
			sourceStack.push(outEdge->DESTINATION);
			sourceVertices.emplace(outEdge->DESTINATION);
			sourceEdges.emplace(outEdge->DESTINATION, outEdge);
		}
	}
}

//moves pebble from source to vertex of edge, reverses path from edge to vertex
void movePebble(Vertex* source, Edge* edge, std::unordered_map<Vertex*, Edge*>& sourceEdges, std::unordered_set<Edge*>& edges)
{
	//declarations
	Vertex* destination = nullptr;
	Edge* reversibleEdge = sourceEdges.at(source);

	//reverse each edge on the path
	while (reversibleEdge != edge)
	{
		//get and add reverse
		Edge* reversedEdge = reversibleEdge->getReverse();
		addEdgeToVertices(reversedEdge);
		edges.emplace(reversedEdge);

		//remove old
		reversibleEdge->ORIGIN->removeOutEdge(reversibleEdge);
		reversibleEdge->DESTINATION->removeInEdge(reversibleEdge);
		edges.erase(reversibleEdge);
		delete reversibleEdge;

		//move on
		destination = reversedEdge->DESTINATION;
		reversibleEdge = sourceEdges.at(destination);
	}

	//move pebble
	source->removePebble();
	destination->addPebble();
}

//gets pebble onto edge's vertices using depth-first search
void getPebble(Edge* edge, std::unordered_set<Edge*>& edges)
{
	//declarations
	std::stack<Vertex*> sourceStack;
	std::unordered_set<Vertex*> sourceVertices;
	std::unordered_map<Vertex*, Edge*> sourceEdges;

	//initialize
	sourceVertices.emplace(edge->DESTINATION);
	sourceVertices.emplace(edge->ORIGIN);
	sourceEdges.emplace(edge->DESTINATION, edge);
	sourceEdges.emplace(edge->ORIGIN, edge);
	addOutNeighbors(edge->DESTINATION, sourceStack, sourceVertices, sourceEdges);
	addOutNeighbors(edge->ORIGIN, sourceStack, sourceVertices, sourceEdges);

	//depth-first search
	while (!sourceStack.empty())
	{
		//declaration
		Vertex* source = sourceStack.top();

		if (source->getNumPebbles()) //pebble found
		{
			//move pebble and end
			movePebble(source, edge, sourceEdges, edges);
			return;
		}
		else //no pebble found
		{
			//add out neighbors and move on
			sourceStack.pop();
			addOutNeighbors(source, sourceStack, sourceVertices, sourceEdges);
		}
	}
}

//checks sparsity and tightness using Lee and Streinu's component pebble game (https://www.sciencedirect.com/science/article/pii/S0012365X07005602)
std::string checkSparsityWithNonNegativeL(std::ifstream& inputFile, int k, int l)
{
	//declarations
	std::unordered_map<std::string, Vertex*> vertices;
	std::unordered_set<Edge*> edges;

	//iterate through edges
	while (Edge* edge = getNextEdge(inputFile, vertices, edges, k))
	{
		if ((edge->ORIGIN == edge->DESTINATION && l >= k) || edge->ORIGIN->doesVertexShareComponent(edge->DESTINATION)) //edge is illegal loop or violates component
		{
			//clean up and return not sparse, sparse graphs with l >= k can't have loops, component violation means subgraph has too many edges
			deleteVerticesAndEdges(vertices, edges);
			return "NOT SPARSE";
		}
		else //edge is acceptable
		{
			//if edge doesn't have enough pebbles then get enough pebbles
			while (edge->ORIGIN->getNumPebbles() + edge->DESTINATION->getNumPebbles() <= l)
			{
				getPebble(edge, edges);
			}

			//add edge
			addEdgeToDigraph(edge, edges);

			//TODO: maintain components
		}
	}

	//clean up and return "tight" if |E| = k|V| - l and "sparse" otherwise
	bool isTight = edges.size() == k * vertices.size() - l;
	deleteVerticesAndEdges(vertices, edges);
	return (isTight) ? ("TIGHT") : ("SPARSE");
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