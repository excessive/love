/**
 * Copyright (c) 2006-2014 LOVE Development Team
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
#include "MathModule.h"
#include "common/Vector.h"
#include "BezierCurve.h"

// STL
#include <cmath>
#include <list>
#include <iostream>

using std::list;
using std::vector;
using love::Vertex;

namespace
{
	// check if an angle is oriented counter clockwise
	inline bool is_oriented_ccw(const Vertex &a, const Vertex &b, const Vertex &c)
	{
		// return det(b-a, c-a) >= 0
		return ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)) >= 0;
	}

	// check if a and b are on the same side of the line c->d
	bool on_same_side(const Vertex &a, const Vertex &b, const Vertex &c, const Vertex &d)
	{
		float px = d.x - c.x, py = d.y - c.y;
		// return det(p, a-c) * det(p, b-c) >= 0
		float l = px * (a.y - c.y) - py * (a.x - c.x);
		float m = px * (b.y - c.y) - py * (b.x - c.x);
		return l * m >= 0;
	}

	// checks is p is contained in the triangle abc
	inline bool point_in_triangle(const Vertex &p, const Vertex &a, const Vertex &b, const Vertex &c)
	{
		return on_same_side(p,a, b,c) && on_same_side(p,b, a,c) && on_same_side(p,c, a,b);
	}

	// checks if any vertex in `vertices' is in the triangle abc.
	bool any_point_in_triangle(const list<const Vertex *> &vertices, const Vertex &a, const Vertex &b, const Vertex &c)
	{
		list<const Vertex *>::const_iterator it, end = vertices.end();
		for (it = vertices.begin(); it != end; ++it)
		{
			const Vertex *p = *it;
			if ((p != &a) && (p != &b) && (p != &c) && point_in_triangle(*p, a,b,c)) // oh god...
				return true;
		}

		return false;
	}

	inline bool is_ear(const Vertex &a, const Vertex &b, const Vertex &c, const list<const Vertex *> &vertices)
	{
		return is_oriented_ccw(a,b,c) && !any_point_in_triangle(vertices, a,b,c);
	}
}

namespace love
{
namespace math
{

Math Math::instance;

Math::Math()
	: rng()
{
	// prevent the runtime from free()-ing this
	retain();
}

RandomGenerator *Math::newRandomGenerator()
{
	return new RandomGenerator();
}

BezierCurve *Math::newBezierCurve(const vector<Vector> &points)
{
	return new BezierCurve(points);
}

vector<Triangle> Math::triangulate(const vector<Vertex> &polygon)
{
	if (polygon.size() < 3)
		throw love::Exception("Not a polygon");
	else if (polygon.size() == 3)
		return vector<Triangle>(1, Triangle(polygon[0], polygon[1], polygon[2]));

	// collect list of connections and record leftmost item to check if the polygon
	// has the expected winding
	vector<size_t> next_idx(polygon.size()), prev_idx(polygon.size());
	size_t idx_lm = 0;
	for (size_t i = 0; i < polygon.size(); ++i)
	{
		const Vertex &lm = polygon[idx_lm], &p = polygon[i];
		if (p.x < lm.x || (p.x == lm.x && p.y < lm.y))
			idx_lm = i;
		next_idx[i] = i+1;
		prev_idx[i] = i-1;
	}
	next_idx[next_idx.size()-1] = 0;
	prev_idx[0] = prev_idx.size()-1;

	// check if the polygon has the expected winding and reverse polygon if needed
	if (!is_oriented_ccw(polygon[prev_idx[idx_lm]], polygon[idx_lm], polygon[next_idx[idx_lm]]))
		next_idx.swap(prev_idx);

	// collect list of concave polygons
	list<const Vertex *> concave_vertices;
	for (size_t i = 0; i < polygon.size(); ++i)
	{
		if (!is_oriented_ccw(polygon[prev_idx[i]], polygon[i], polygon[next_idx[i]]))
			concave_vertices.push_back(&polygon[i]);
	}

	// triangulation according to kong
	vector<Triangle> triangles;
	size_t n_vertices = polygon.size();
	size_t current = 1, skipped = 0, next, prev;
	while (n_vertices > 3)
	{
		next = next_idx[current];
		prev = prev_idx[current];
		const Vertex &a = polygon[prev], &b = polygon[current], &c = polygon[next];
		if (is_ear(a,b,c, concave_vertices))
		{
			triangles.push_back(Triangle(a,b,c));
			next_idx[prev] = next;
			prev_idx[next] = prev;
			concave_vertices.remove(&b);
			--n_vertices;
			skipped = 0;
		}
		else if (++skipped > n_vertices)
		{
			throw love::Exception("Cannot triangulate polygon.");
		}
		current = next;
	}
	next = next_idx[current];
	prev = prev_idx[current];
	triangles.push_back(Triangle(polygon[prev], polygon[current], polygon[next]));

	return triangles;
}

bool Math::isConvex(const std::vector<Vertex> &polygon)
{
	if (polygon.size() < 3)
		return false;

	// a polygon is convex if all corners turn in the same direction
	// turning direction can be determined using the cross-product of
	// the forward difference vectors
	size_t i = polygon.size() - 2, j = polygon.size() - 1, k = 0;
	Vector p(polygon[j].x - polygon[i].x, polygon[j].y - polygon[i].y);
	Vector q(polygon[k].x - polygon[j].x, polygon[k].y - polygon[j].y);
	float winding = p ^ q;

	while (k+1 < polygon.size())
	{
		i = j; j = k; k++;
		p.x = polygon[j].x - polygon[i].x;
		p.y = polygon[j].y - polygon[i].y;
		q.x = polygon[k].x - polygon[j].x;
		q.y = polygon[k].y - polygon[j].y;

		if ((p^q) * winding < 0)
			return false;
	}
	return true;
}

} // math
} // love
