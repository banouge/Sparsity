#include "Graph.h"

Vertex::Vertex(std::string name, int numPebbles): NAME(name)
{
	this->numPebbles = numPebbles;
}

int Vertex::getNumPebbles()
{
	return numPebbles;
}

void Vertex::setNumPebbles(int numPebbles)
{
	this->numPebbles = numPebbles;
}

void Vertex::addEdge(Edge* edge)
{
	outEdges.emplace(edge);
}

void Vertex::removeEdge(Edge* edge)
{
	outEdges.erase(edge);
}

bool Vertex::hasEdge(Edge* edge)
{
	return outEdges.count(edge);
}

Edge::Edge(Vertex* origin, Vertex* destination): ORIGIN(origin), DESTINATION(destination)
{
}

Edge* Edge::getReverse()
{
	return new Edge(DESTINATION, ORIGIN);
}