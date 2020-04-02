#pragma once

#include <string>
#include <unordered_set>

class Edge;

class Vertex
{
public:
	Vertex(std::string name, int numPebbles);

	const std::string NAME;

	const std::unordered_set<Edge*>& getInEdges();
	const std::unordered_set<Edge*>& getOutEdges();
	int getNumPebbles();
	int getInDegree();
	int getOutDegree();
	void addVerticesThatShareComponent(std::unordered_set<Vertex*>& componentVertices);
	void addPebble();
	void addInEdge(Edge* edge);
	void addOutEdge(Edge* edge);
	void removePebble();
	void removeInEdge(Edge* edge);
	void removeOutEdge(Edge* edge);
	bool doesVertexShareComponent(Vertex* vertex);
	bool hasInEdge(Edge* edge);
	bool hasOutEdge(Edge* edge);

private:
	int numPebbles;
	std::unordered_set<Edge*> inEdges;
	std::unordered_set<Edge*> outEdges;
	std::unordered_set<Vertex*> verticesThatShareComponent;
};

class Edge
{
public:
	Edge(Vertex* origin, Vertex* destination);

	Vertex* const ORIGIN;
	Vertex* const DESTINATION;
	bool const IS_LOOP;

	Edge* getReverse();
	int getNumPebbles();
};