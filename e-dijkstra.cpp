/**
 * e-dijkstra.cpp
 * Jacob Lon
 *
 * /brief
 *      This data structure finds every node that can be traversed by an "electric vehicle" with a battery
 *      that has a given max range and recharge limitation.
 *      Find shortest path to each node with a max range on the battery and recharges, which fill the battery back up
 *      max capacity aka max range
 *      My process for solving this included first finding each traversable node, then with range and no recharge limit,
 *      then eventually added recharge limit
 *
 */
#include "stdio.h"
#include "iostream"
#include "fstream"
#include <vector>
#include <algorithm>
#include <map>
#include "limits"

//////////////////////////// Data Structures //////////////////////////////////////////////////////////////////////////

/**
 * Defines connections between nodes
 */
class Edge
{
	public:
		
		Edge() : start(0), end(0), weight(0)
		{}
		
		Edge(int start_, int end_, int weight_) : start(start_), end(end_), weight(weight_)
		{}
		
		bool operator<(Edge rhs) { return this->weight < rhs.weight; }
		
		int start; // represents parent node
		int end; // represents adjacent node
		int weight; // represents "distance to travel" between nodes
	private:
};

/**
 * represents the electric vehicles battery
 *
 * /brief
 *      within the algorithm multiple instances of "Battery" are created to represent the same vehicles different
 *      possible paths and steps
 */
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

        // if battery doesnt have enough range, recharge
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
		
		int charges; // amount of times the vehicle can recharge
		int range; // the max range of one charge
	private:
};

/**
 * Node, or intersection or city or station etc., that the vehicle will be traveling to
 */
class Vertex
{
	public:
		
		Vertex() : id(0), evaluated(false), neighbors()
        {}
		
		explicit Vertex(int id_) : id(id_), evaluated(false), neighbors()
		{}
		
		int id; // node value read in from input
		bool evaluated;
		std::vector<Edge> neighbors;
	private:
};

//////////////////////////// Data Structures End //////////////////////////////////////////////////////////////////////


// function head for primary algorithm
bool dijkstra(const std::vector<Vertex*>& vertices, Vertex* source, int range, int charges);

/**
 * operator overload for Battery values
 * @param lhs
 * @param rhs
 * @return true if lhs has used less range
 */
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

//////////////////////////// Helper Functions /////////////////////////////////////////////////////////////////////////

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

/**
 *
 * @param options
 * @param source
 * @param maxrange
 * @return pointer to next best vertex to solve for
 */
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

//////////////////////////// Helper Functions End /////////////////////////////////////////////////////////////////////

/**
 * loading file and reading input
 * @param filename
 * @param range
 * @return false if file load failed
 */
bool e_dijkstra( char const * filename, int range)
{
	std::fstream input(filename, std::fstream::in);
	
	if (!input.is_open())
	{
		std::cout << "File failed to open" << std::endl;
		return false;
	}
	
	int locations = 0; // vertex
	int recharges = 0;
	int numedges = 0; // total number of edges that exist in the whole map
	std::vector<Vertex*> vertices2;
	
	input >> locations;
	input >> recharges;
	input >> numedges;

    for (int i = 0; i < locations; i++)
    {
        vertices2.push_back(new Vertex(i));
    }

    // data stored by edge, one line per edge
    // each line represents one edge between two vertices, vertices maybe repeat, edges do not
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

/**
 *
 * @param vertices list of node pointers
 * @param source starting node
 * @param range max range of batteries
 * @param charges max charges of batteries
 * @return true if all nodes traversable
 */
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

    // foreach isn't necessary but I like/liked doing this and it didn't have any important drawbacks
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