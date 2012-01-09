/**
* Copyright (c) 2006-2012 LOVE Development Team
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

#ifndef LOVE_PHYSICS_BOX2D_WELD_JOINT_H
#define LOVE_PHYSICS_BOX2D_WELD_JOINT_H

// Module
#include "Joint.h"

namespace love
{
namespace physics
{
namespace box2d
{
	/**
	* A WeldJoint essentially glues two bodies together.
	**/
	class WeldJoint : public Joint
	{
	private:

		// The Box2D weld joint object.
		b2WeldJoint * joint;

	public:

		/**
		* Creates a new WeldJoint connecting body1 and body2.
		**/
		WeldJoint(Body * body1, Body * body2, float xA, float yA, float xB, float yB, bool collideConnected);

		virtual ~WeldJoint();

		/**
		* Sets the response speed.
		**/
		void setFrequency(float hz);

		/**
		* Gets the response speed.
		**/
		float getFrequency() const;

		/**
		* Sets the damping ratio.
		* 0 = no damping, 1 = critical damping.
		**/
		void setDampingRatio(float d);

		/**
		* Gets the damping ratio.
		* 0 = no damping, 1 = critical damping.
		**/
		float getDampingRatio() const;

	};

} // box2d
} // physics
} // love

#endif // LOVE_PHYSICS_BOX2D_WELD_JOINT_H
