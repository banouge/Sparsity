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

//adds vetices to map
void addVertices(std::ifstream& inputFile, std::unordered_map<std::string, Vertex*>& vertices, int k)
{
	//declaration
	std::string line = "";

	//get line
	std::getline(inputFile, line);

	//add first vertices
	while (line.find(',') < line.size())
	{
		std::size_t posComma = line.find(',');
		std::string vertexName = line.substr(0, posComma);
		vertices.emplace(vertexName, new Vertex(vertexName, k));
		line = line.substr(posComma + 1);
	}

	//add last vertex if there are multiple vertices
	if (line.size())
	{
		vertices.emplace(line, new Vertex(line, k));
	}
}

//gets next edge from file, adds edge to set (but not vertices because edge may be reversed or rejected)
Edge* getNextEdge(std::ifstream& inputFile, std::unordered_map<std::string, Vertex*>& vertices, std::unordered_set<Edge*>& edges)
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
	line = line.substr(posComma + 1);
	posComma = line.find(',');
	std::string destinationName = (posComma < line.size()) ? (line.substr(0, posComma)) : (line);

	//get origin and destination
	origin = vertices.at(originName);
	destination = vertices.at(destinationName);

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
	//initialization
	std::unordered_map<std::string, Vertex*> vertices;
	std::unordered_set<Vertex*> labeledVertices;
	std::unordered_set<Edge*> edges;
	int numRemovableEdges = -l;
	addVertices(inputFile, vertices, k);

	//iterate through edges
	while (Edge* edge = getNextEdge(inputFile, vertices, edges))
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
void addEdgeToDigraph(Edge*& edge, std::unordered_set<Edge*>& edges)
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
		edge = reversedEdge;
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

//gets vertices reachable from edge vertices, returns true if a reached vertex that isn't in the edge has a pebble and false otherwise
bool doVerticesReachableFromEdgeHavePebbles(Edge* edge, std::unordered_set<Vertex*>& reachableVertices)
{
	//declaration
	std::stack<Vertex*> vertexStack;

	//initialize
	vertexStack.push(edge->ORIGIN);
	vertexStack.push(edge->DESTINATION);
	reachableVertices.emplace(edge->ORIGIN);
	reachableVertices.emplace(edge->DESTINATION);

	//get vertices reachable from edge vertices
	while (!vertexStack.empty())
	{
		//get top
		Vertex* vertex = vertexStack.top();
		vertexStack.pop();

		//add out neighbors
		for (Edge* outEdge : vertex->getOutEdges())
		{
			//only address vertices that haven't been addressed yet
			if (!reachableVertices.count(outEdge->DESTINATION))
			{
				//end because edge could have had more pebbles by taking a pebble from this vertex, components do not change
				if (outEdge->DESTINATION->getNumPebbles())
				{
					return true;
				}

				//add neighbor
				vertexStack.push(outEdge->DESTINATION);
				reachableVertices.emplace(outEdge->DESTINATION);
			}
		}
	}

	//no reached vertices had pebbles
	return false;
}

//gets component vertices
void getComponentVertices(std::unordered_set<Vertex*>& componentVertices, std::unordered_set<Vertex*>& reachableVertices, std::unordered_map<std::string, Vertex*>& vertices)
{
	//declaration
	std::stack<Vertex*> vertexStack;

	//initialize
	for (std::pair<std::string, Vertex*> vertex : vertices)
	{
		if (vertex.second->getNumPebbles() && !reachableVertices.count(vertex.second)) //vertex is unreachable from edge vertices and has pebbles
		{
			//add to stack
			vertexStack.push(vertex.second);
		}
		else //vertex is reachable from edge vertices or has no pebbles
		{
			//add to component
			componentVertices.emplace(vertex.second);
		}
	}

	//remove from component all vertices that reach a vertex with pebbles that is unreachable from edge vertices
	while (!vertexStack.empty())
	{
		//get top
		Vertex* vertex = vertexStack.top();
		vertexStack.pop();

		//add in neighbor to stack, remove in neighbor from component (in neighbor has access to more pebbles)
		for (Edge* inEdge : vertex->getInEdges())
		{
			//only add/remove if neighbor hasn't been added/removed already
			if (componentVertices.count(inEdge->ORIGIN))
			{
				vertexStack.push(inEdge->ORIGIN);
				componentVertices.erase(inEdge->ORIGIN);
			}
		}
	}
}

//updates components
void updateComponents(Edge* edge, std::unordered_map<std::string, Vertex*>& vertices, int l)
{
	//components only change if the edge had the minimum possible pebbles
	if (edge->getNumPebbles() == l)
	{
		//declarations
		std::unordered_set<Vertex*> reachableVertices;
		std::unordered_set<Vertex*> componentVertices;

		//edge couldn't have had more pebbles by taking a pebble from a reachable vertex, components change
		if (!doVerticesReachableFromEdgeHavePebbles(edge, reachableVertices))
		{
			//get component vertices
			getComponentVertices(componentVertices, reachableVertices, vertices);

			//update components
			for (Vertex* vertex : componentVertices)
			{
				vertex->addVerticesThatShareComponent(componentVertices);
			}
		}
	}
}

//checks sparsity and tightness using Lee and Streinu's component pebble game (https://www.sciencedirect.com/science/article/pii/S0012365X07005602)
std::string checkSparsityWithNonNegativeL(std::ifstream& inputFile, int k, int l)
{
	//initialization
	std::unordered_map<std::string, Vertex*> vertices;
	std::unordered_set<Edge*> edges;
	addVertices(inputFile, vertices, k);

	//iterate through edges
	while (Edge* edge = getNextEdge(inputFile, vertices, edges))
	{
		if ((edge->IS_LOOP && l >= k) || edge->ORIGIN->doesVertexShareComponent(edge->DESTINATION) || l >= 2 * k) //edge is illegal
		{
			//clean up and return not sparse, sparse graphs with l >= k can't have loops and l >= 2k can't have edges, component violation means subgraph has too many edges
			deleteVerticesAndEdges(vertices, edges);
			return "NOT SPARSE";
		}
		else //edge is acceptable
		{
			//if edge doesn't have enough pebbles then get enough pebbles
			while (edge->getNumPebbles() <= l)
			{
				getPebble(edge, edges);
			}

			//add edge and update components
			addEdgeToDigraph(edge, edges);
			updateComponents(edge, vertices, l);
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