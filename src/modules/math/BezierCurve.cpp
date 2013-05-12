/**
 * Copyright (c) 2006-2013 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

// LOVE
#include "BezierCurve.h"
#include "common/Exception.h"

#include <iostream>

using namespace std;

namespace
{

/**
 * Subdivide Bezier polygon.
 **/
void subdivide(vector<love::Vector> &points, int k)
{
	if (k <= 0)
		return;

	// subdivision using de casteljau - subdivided control polygons are
	// on the 'edges' of the computation scheme, e.g:
	//
	// ------LEFT------->
	// b00  b10  b20  b30
	// b01  b11  b21 .---
	// b02  b12 .---'
	// b03 .---'RIGHT
	// <--'
	//
	// the subdivided control polygon is:
	// b00, b10, b20, b30, b21, b12, b03
	vector<love::Vector> left, right;
	left.reserve(points.size());
	right.reserve(points.size());

	for (size_t step = 1; step < points.size(); ++step)
	{
		left.push_back(points[0]);
		right.push_back(points[points.size() - step]);
		for (size_t i = 0; i < points.size() - step; ++i)
			points[i] = (points[i] + points[i+1]) * .5;
	}
	left.push_back(points[0]);
	right.push_back(points[0]);

	// recurse
	subdivide(left, k-1);
	subdivide(right, k-1);

	// merge (right is in reversed order)
	points.resize(left.size() + right.size() - 1);
	for (size_t i = 0; i < left.size(); ++i)
		points[i] = left[i];
	for (size_t i = 1; i < right.size(); ++i)
		points[i-1 + left.size()] = right[right.size() - i - 1];
}

}

namespace love
{
namespace math
{

BezierCurve::BezierCurve(const vector<Vector> &pts)
	: controlPoints(pts)
{
}

const Vector &BezierCurve::getControlPoint(int i) const
{
	if (i < 0)
		i += controlPoints.size();

	if (i < 0 || (size_t) i >= controlPoints.size())
		throw Exception("Invalid control point index");

	return controlPoints[i];
}

void BezierCurve::setControlPoint(int i, const Vector &point)
{
	if (i < 0)
		i += controlPoints.size();

	if (i < 0 || (size_t) i >= controlPoints.size())
		throw Exception("Invalid control point index");

	controlPoints[i] = point;
}

void BezierCurve::insertControlPoint(const Vector &point, int pos)
{
	if (pos < 0)
		pos += controlPoints.size() + 1;

	if (pos < 0 ||(size_t)  pos > controlPoints.size())
		throw Exception("Invalid control point index");

	controlPoints.insert(controlPoints.begin() + pos, point);
}

Vector BezierCurve::eval(double t) const
{
	if (t < 0 || t > 1)
		throw Exception("Invalid evaluation parameter: must be between 0 and 1");

	// de casteljau
	vector<Vector> points(controlPoints);
	for (size_t step = 1; step < controlPoints.size(); ++step)
		for (size_t i = 0; i < controlPoints.size() - step; ++i)
			points[i] = points[i] * (1-t) + points[i+1] * t;
	
	return points[0];
}

vector<Vector> BezierCurve::render(size_t accuracy) const
{
	vector<Vector> vertices(controlPoints);
	subdivide(vertices, accuracy);
	return vertices;
}


} // namespace math
} // namespace love
