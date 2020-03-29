#include "Graph.h"

Vertex::Vertex(std::string name, int numPebbles): NAME(name)
{
	this->numPebbles = numPebbles;
}

const std::unordered_set<Edge*>& Vertex::getInEdges()
{
	return inEdges;
}

const std::unordered_set<Edge*>& Vertex::getOutEdges()
{
	return outEdges;
}

int Vertex::getNumPebbles()
{
	return numPebbles;
}

int Vertex::getInDegree()
{
	return inEdges.size();
}

int Vertex::getOutDegree()
{
	return outEdges.size();
}

void Vertex::addVerticesThatShareComponent(std::unordered_set<Vertex*>& componentVertices)
{
	for (Vertex* vertex : componentVertices)
	{
		verticesThatShareComponent.emplace(vertex);
	}
}

void Vertex::addPebble()
{
	++numPebbles;
}

void Vertex::addInEdge(Edge* edge)
{
	inEdges.emplace(edge);
}

void Vertex::addOutEdge(Edge* edge)
{
	outEdges.emplace(edge);
}

void Vertex::removePebble()
{
	--numPebbles;
}

void Vertex::removeInEdge(Edge* edge)
{
	inEdges.erase(edge);
}

void Vertex::removeOutEdge(Edge* edge)
{
	outEdges.erase(edge);
}

bool Vertex::doesVertexShareComponent(Vertex* vertex)
{
	return verticesThatShareComponent.count(vertex);
}

bool Vertex::hasInEdge(Edge* edge)
{
	return inEdges.count(edge);
}

bool Vertex::hasOutEdge(Edge* edge)
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