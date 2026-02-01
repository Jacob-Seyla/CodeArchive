//
// Created by jacob on 11/16/2022.
//
#include "stdio.h"
#include "iostream"
#include "fstream"
#include <vector>
#include <algorithm>
#include <map>
#include "limits"


class Edge
{
	public:
		
		Edge() : start(0), end(0), weight(0)
		{}
		
		Edge(int start_, int end_, int weight_) : start(start_), end(end_), weight(weight_)
		{}
		
		bool operator<(Edge rhs) { return this->weight < rhs.weight; }
		
		int start;
		int end;
		int weight;
	private:
};

class Battery
{
	public:
		
		Battery() : charges(0), range(0)
		{}
		
		Battery(int charges_, int range_) : charges(charges_), range(range_)
		{}

        int value(const int maxrange)
        {
            if (range > 200000 || charges > 200000)
                return std::numeric_limits<int>::max();


            return range + (charges * maxrange);
        }

        void increment(int weight, const int maxrange, Battery prev)
        {
            if (prev.range + weight > maxrange)
            {
                charges = prev.charges + 1;
                range = maxrange - weight;
            } else
            {
                charges = prev.charges;
                range = prev.range + weight;
            }
        }
		
		int charges;
		int range;
	private:
};

class Vertex
{
	public:
		
		Vertex() : id(0), evaluated(false), neighbors()
        {}
		
		explicit Vertex(int id_) : id(id_), evaluated(false), neighbors()
		{}
		
		int id;
		bool evaluated;
		std::vector<Edge> neighbors;
	private:
};

bool dijkstra(const std::vector<Vertex*>& vertices, Vertex* source, int range, int charges);

bool operator<(Battery lhs, Battery rhs)
{
	if (lhs.charges < rhs.charges)
	{
		return true;
	} else if (lhs.charges == rhs.charges)
	{
		if (lhs.range >= rhs.range)
		{
			return true;
		}
		return false;
	} else if (lhs.charges > rhs.charges)
	{
		return false;
	}
	
	return false;
}

bool edgeweight(Edge lhs, Edge rhs) { return lhs < rhs; }

bool edgestart(Edge lhs, Edge rhs) { return lhs.start < rhs.start; }

bool edgeend(Edge lhs, Edge rhs) { return lhs.end < rhs.end; }

void printvertices(std::map<int, std::vector<Edge>> vertices)
{
	for (auto & key : vertices)
	{
		std::cout << key.first << ": ";
		int size = key.second.size();
		for (int i = 0; i < size; i++)
		{
			std::cout << key.second[i].end << " ";
		}
		std::cout << std::endl;
	}
}

bool e_dijkstra( char const * filename, int range)
{
	std::fstream input(filename, std::fstream::in);
	
	if (!input.is_open())
	{
		std::cout << "File failed to open" << std::endl;
		return false;
	}
	
	int locations = 0;
	int recharges = 0;
	int numedges = 0;
	std::vector<Vertex*> vertices2;
	
	input >> locations;
	input >> recharges;
	input >> numedges;

    for (int i = 0; i < locations; i++)
    {
        vertices2.push_back(new Vertex(i));
    }

	for (int i = 0; i < numedges; i++)
	{
		Edge edge;
		input >> edge.start;
		input >> edge.end;
		input >> edge.weight;

		if (vertices2[edge.start] == nullptr)
			vertices2[edge.start] = new Vertex(edge.start);

		vertices2[edge.start]->neighbors.push_back(edge);

		if (vertices2[edge.end] == nullptr)
			vertices2[edge.end] = new Vertex(edge.end);

		vertices2[edge.end]->neighbors.push_back(Edge(edge.end, edge.start, edge.weight));
	}

    bool result = true;

    for (auto vertex : vertices2)
    {
        result = dijkstra(vertices2, vertex, range, recharges);
    }

    for (auto var : vertices2)
    {
        delete var;
    }

    input.close();
    return result;
}

Vertex* GetBestVertex(std::map<Vertex*, Battery> options, Vertex* source, const int maxrange)
{
    Battery tempB(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    Vertex* tempV = source;

    for (auto option : options)
    {
        if (option.second.value(maxrange) < tempB.value(maxrange) && !option.first->evaluated)
        {
            tempB = option.second;
            tempV = option.first;
        }
    }

    return tempV;
}

bool dijkstra(const std::vector<Vertex*>& vertices, Vertex* source, const int range, const int charges)
{
	std::map<Vertex*, Battery> distances;

	// map of every vertice; vertex is a location and its edges
	for (auto vertex : vertices)
	{
        vertex->evaluated = false;
		distances[vertex] = Battery(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
	}
	
	distances[source] = Battery(0,0);

    for (auto vertex : vertices)
    {
        Vertex* vert = vertex; // it keeps complaining and I cant be bothered to come up with a use for it
        vert->evaluated = vert->evaluated;

        Vertex* current = GetBestVertex(distances, source, range);

        if (current->evaluated) continue;

        for (auto edge : current->neighbors)
        {
            if (edge.weight > range) continue;
            int end = edge.end;
            if (vertices[end]->id == source->id) continue;

            Vertex* currpoint = vertices[end];
            if (currpoint->evaluated)
            {
                Battery temp = distances[currpoint];
                temp.increment(edge.weight, range, distances[current]);

                if (temp.value(range) < distances[currpoint].value(range))
                {
                    distances[currpoint] = temp;
                }
            } else
            {
                distances[currpoint].increment(edge.weight, range, distances[current]);
            }
        }
        current->evaluated = true;
    }

    int size = vertices.size();

    for (int i = 0; i < size; i++)
    {
        if (!vertices[i]->evaluated) return false;
        if (distances[vertices[i]].value(range) > range + (charges * range)) return false;
    }

    return true;
}
///////////////////////////////////////////////////////////////////////