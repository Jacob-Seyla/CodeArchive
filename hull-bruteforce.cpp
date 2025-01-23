/*
 * Jacob Lon
 * CS 330 F23
 * note: im not gonna lie this is a copy from when i did it last year
 */

#include "hull-bruteforce.h"
#include <algorithm>
#include <iostream>
#include <limits>

bool Point::operator==( Point const& arg2 ) const {
    return ( (x==arg2.x) && (y==arg2.y) );
}

std::ostream& operator<< (std::ostream& os, Point const& p) {
	os << "(" << p.x << " , " << p.y << ") ";
	return os;
}

std::istream& operator>> (std::istream& os, Point & p) {
	os >> p.x >> p.y;
	return os;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//return value is (on left, on right)
std::pair<bool,bool> get_location(
		float const& p1x, //check which side of the line (p1x,p1y)-->(p2x,p2y)
		float const& p1y, //point (qx,qy) is on
		float const& p2x,
		float const& p2y,
		float const& qx,
		float const& qy )
{
	Point dir   = {p2x-p1x,p2y-p1y};
	Point norm  = {dir.y, -dir.x};
	Point point = {qx-p1x,qy-p1y};
	//scalar product is positive if on right side
	float scal_prod = norm.x*point.x + norm.y*point.y;
	return std::make_pair( (scal_prod<0), (scal_prod>0));
}

//returns a set of indices of points that form convex hull
std::set<int> hullBruteForce ( std::vector< Point > const& points )
{
	int num_points = points.size();
	//std::cout << "number of points " << num_points << std::endl;
	if ( num_points < 3 ) throw "bad number of points";

	std::set<int> hull_indices;
	
	for (int i = 0; i < num_points - 1; i++)
	{
		for (int j = i + 1; j < num_points; j++)
		{
			if (i == j) continue; // if same point leave
			//std::cout << points[i] << "- " << points[j] << ": ";
			
			// Record of which side the points land on
			std::pair<bool, bool> sameside = {false, false};
			
			// look for all other points that are not the ends of the line
			for (int k = 0; k < num_points; k++)
			{
				if ((k == i) | (k == j)) continue; // if same point as ends of line
				
				auto sides = get_location(points[i].x,points[i].y,
										  				points[j].x, points[j].y,
										  				points[k].x, points[k].y);
				
				// save which side the points end up on
				if (sides.first == true) sameside.first = true;
				if (sides.second == true) sameside.second = true;
				
				// if this point is not on the line, not necessary since should always be true
				if ((sides.first == true) | (sides.second == true))
				{
					//std::cout << points[k] << "x: " << sides.first << " y: " << sides.second << " || ";
				}
                if (sameside.first == true && sameside.second == true)
                {
                    break;
                }
			}
			
			if ((sameside.first == true) && (sameside.second == true))
			{
				//std::cout << " Not on all points are on the same side ";
			} else
			{
                /*
                if (hull_indices.find(i) != hull_indices.end() && hull_indices.find(j) != hull_indices.end())
                {
                    hull_indices.insert(i);
                    hull_indices.insert(j);
                    return hull_indices;
                } */

				hull_indices.insert(i);
				hull_indices.insert(j);


                //break;
				
				//std::cout << " All points on same side, add points to convex hull";
			}
			
			//std::cout << std::endl; // end of the print statement for this line of points
		}
	}
	

		
	return hull_indices;
}

/*
// sort by x coordinate from left to right
bool sortfromlefttoright(std::pair<Point, int> one, std::pair<Point, int> two)
{
    if (one.first.x < two.first.x) return true;
    else return false;
}*/

std::vector<int> hullBruteForce2 ( std::vector< Point > const& points ) {
	int num_points = points.size();
	if ( num_points < 3 ) throw "bad number of points";

	std::vector<int> hull_indices;

    int indexcurr = 0; // current point of the hull
    int indexstart = 0; // first point of the hull (left most)
    int smallestx = std::numeric_limits<int>::max(); // left most x

    // finds the left most point
    for (int i = 0; i < num_points; i++)
    {
        if (points[i].x < smallestx)
        {
            indexcurr = i;
            smallestx = points[i].x;
        }
    }


    indexstart = indexcurr;
    //std::cout << "leftmost is " << points[indexcurr] << std::endl;
    hull_indices.push_back(indexcurr);

    bool complete = false; // if we find a closed hull
    int indexnext = indexcurr; // initialize


    while (!complete)
    {
        indexcurr = indexnext;

        for (int j = 0; j < num_points; j++)
        {
            if (indexcurr == j) continue; // if same point leave
            //if (std::find(hull_indices.begin(), hull_indices.end(), j) != hull_indices.end()) continue;
            //std::cout << points[i] << "- " << points[j] << ": ";

            // Record of which side the points land on
            std::pair<bool, bool> sameside = {false, false};

            // look for all other points that are not the ends of the line
            for (int k = 0; k < num_points; k++)
            {
                if ((k == indexcurr) | (k == j)) continue; // if same point as ends of line

                auto sides = get_location(points[indexcurr].x,points[indexcurr].y,
                                          points[j].x, points[j].y,
                                          points[k].x, points[k].y);

                // save which side the points end up on
                if (sides.first == true) sameside.first = true;
                if (sides.second == true) sameside.second = true;

                // if this point is not on the line, not necessary since should always be true
                if ((sides.first == true) | (sides.second == true))
                {
                    //std::cout << points[k] << "x: " << sides.first << " y: " << sides.second << " || ";
                }

                if (sameside.first == true && sameside.second == true)
                {
                    break;
                }
            }

            if ((sameside.first == true) && (sameside.second == true))
            {
                //std::cout << " Not on all points are on the same side ";
            }

            if (sameside.first == true && sameside.second == false)
            {
                indexnext = j;
                //std::cout << points[j] << " added to hull " << std::endl;


                if (indexnext == indexstart)
                {
                    complete = true;
                    break;
                }

                hull_indices.push_back(j);
                break;
            }

            //std::cout << std::endl; // end of the print statement for this line of points
        }
    }

	return hull_indices;
}
