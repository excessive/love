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

#ifndef LOVE_MATH_MODMATH_H
#define LOVE_MATH_MODMATH_H

#include "RandomGenerator.h"

// LOVE
#include "common/Module.h"
#include "common/math.h"
#include "common/int.h"
#include "common/StringMap.h"

// Noise
#include "libraries/noise1234/simplexnoise1234.h"

// STL
#include <limits>
#include <vector>

namespace love
{
namespace math
{

class Math : public Module
{
private:

	RandomGenerator rng;

public:

	virtual ~Math()
	{}

	/**
	 * @copydoc RandomGenerator::randomseed()
	 **/
	inline void randomseed(uint64 seed)
	{
		rng.randomseed(seed);
	}

	/**
	 * @copydoc RandomGenerator::random()
	 **/
	inline double random()
	{
		return rng.random();
	}

	/**
	 * @copydoc RandomGenerator::random(double)
	 **/
	inline double random(double max)
	{
		return rng.random(max);
	}

	/**
	 * @copydoc RandomGenerator::random(double,double)
	 **/
	inline double random(double min, double max)
	{
		return rng.random(min, max);
	}

	/**
	 * @copydoc RandomGenerator::randomnormal()
	 **/
	inline double randomnormal(double stddev)
	{
		return rng.randomnormal(stddev);
	}

	/**
	 * Create a new random number generator.
	 **/
	RandomGenerator *newRandomGenerator();

	virtual const char *getName() const
	{
		return "love.math";
	}

	/**
	 * Triangulate a simple polygon.
	 *
	 * @param polygon Polygon to triangulate. Must not intersect itself.
	 * @return List of triangles the polygon is composed of.
	 **/
	std::vector<Triangle> triangulate(const std::vector<vertex> &polygon);

	/**
	 * Calculate Simplex noise for the specified coordinate(s).
	 *
	 * @return Noise value in the range of [0,1].
	 **/
	float simplexNoise1(float x) const;
	float simplexNoise2(float x, float y) const;
	float simplexNoise3(float x, float y, float z) const;
	float simplexNoise4(float x, float y, float z, float w) const;

	static Math instance;

private:

	Math();

}; // Math

inline float Math::simplexNoise1(float x) const
{
	return SimplexNoise1234::noise(x);
}

inline float Math::simplexNoise2(float x, float y) const
{
	return SimplexNoise1234::noise(x, y);
}

inline float Math::simplexNoise3(float x, float y, float z) const
{
	return SimplexNoise1234::noise(x, y, z);
}

inline float Math::simplexNoise4(float x, float y, float z, float w) const
{
	return SimplexNoise1234::noise(x, y, z, w);
}

} // math
} // love

#endif
