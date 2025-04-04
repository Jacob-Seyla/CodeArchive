/**
 * Jacob Lon
 * Cs 330 fall 2023
 * Closest pair
 */
#include "closestpair.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <cmath>
#include <iostream>
#include <utility>


std::ostream& operator<< (std::ostream& os, Point const& p) {
	os << "(" << p.x << " , " << p.y << ") ";
	return os;
}

std::istream& operator>> (std::istream& os, Point & p) {
	os >> p.x >> p.y;
	return os;
}

bool xlessthan(Point lhs, Point rhs)
{
	return lhs.x < rhs.x;
}

bool ylessthan(Point lhs, Point rhs)
{
	return lhs.y < rhs.y;
}

////////////////////////////////////////////////////////////////////////////////
float closestPair_aux(std::vector< Point > const& indices);

float distance(Point &a, Point &b)
{
	return std::sqrt(((a.x - b.x ) * (a.x - b.x ))
				+ ((a.y - b.y) * (a.y - b.y)));
}

////////////////////////////////////////////////////////////////////////////////
float closestPair( std::vector< Point > const& points )
{
	int size = points.size();

	if (size==0) throw "zero size subset";
	if (size==1) return std::numeric_limits<float>::max();
/*
    for (int i = 0; i < size; i++)
    {
        std::cout << points[i] << std::endl;
    }*/

	return closestPair_aux( points );
}

////////////////////////////////////////////////////////////////////////////////
float closestPair_aux(std::vector< Point > const& indices)
{
	int size = indices.size();

    // check of valid vector lengths
	if (size==0) throw "zero size subset";
	if (size==1) return std::numeric_limits<float>::max();

	
	
	// sort points
    std::vector<Point> temppoints = indices;

    // find smallest and largest x
    auto minmaxelement = std::minmax_element(temppoints.begin(), temppoints.end(), xlessthan);
    bool x = true;
    float range = (minmaxelement.second->x - minmaxelement.first->x);
    if (range >= -0.001f && range <= 0.001f) // if all on same vertical line
    {
        // find smallest and largest y
        minmaxelement =  std::minmax_element(temppoints.begin(), temppoints.end(), ylessthan);
        x = false;
        range = (minmaxelement.second->y - minmaxelement.first->y);
        if (range >= -0.001f && range <= 0.001f) // if all on same horizontal y
            return 0.0f; // all one point, return 0

        std::sort(temppoints.begin(), temppoints.end(), ylessthan); // otherwise sort by y
    } else
    {
        // if not all on same vertical line, sort by x
        std::sort(temppoints.begin(), temppoints.end(), xlessthan);
    }


	std::vector<Point> Left;
	std::vector<Point> Right;

    //std::cout <<"range = " << range << std::endl;

    if (x)
    {
      //  std::cout << " x sorted points " << range / 2 << std::endl;
	    for (int i = 0; i < size; i++)
	    {
		    if (temppoints[i].x < (temppoints.front().x + (range / 2)))
		    {
			    Left.push_back(temppoints[i]);
		    } else
            {
                Right.push_back(temppoints[i]);
            }
	    }
    } else
    {
        //std::cout << " x sorted points " << range / 2 << std::endl;
        for (int i = 0; i < size; i++)
        {
            if (temppoints[i].y < (temppoints.front().y + (range / 2)) )
            {
                Left.push_back(temppoints[i]);
            } else
            {
                Right.push_back(temppoints[i]);
            }
        }
    }

	
	
	// check distance
	float sizeleft = Left.size();
	float sizeright = Right.size();
	float distance1 = closestPair_aux(Left);
	float distance2 = closestPair_aux(Right);
	float min_dist = std::min(distance1, distance2);
	
    for (int i = 0; i < sizeleft && i < sizeright; i++)
    {
        int index = static_cast<int>(sizeleft) - 1 - i;
        float currdistance = distance(Left[index], Right[i]);
        if (currdistance < min_dist)
            min_dist = currdistance;
    }

	return min_dist;
}
