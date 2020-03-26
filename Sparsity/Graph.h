#pragma once

#include <string>
#include <unordered_set>

class Edge;

class Vertex
{
public:
	Vertex(std::string name, int numPebbles);

	const std::string NAME;

	int getNumPebbles();
	void setNumPebbles(int numPebbles);
	void addEdge(Edge* edge);
	void removeEdge(Edge* edge);
	bool hasEdge(Edge* edge);

private:
	int numPebbles;
	std::unordered_set<Edge*> outEdges;
};

class Edge
{
public:
	Edge(Vertex* origin, Vertex* destination);

	Vertex* const ORIGIN;
	Vertex* const DESTINATION;

	Edge* getReverse();
};