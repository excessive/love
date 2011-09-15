/**
* Copyright (c) 2006-2011 LOVE Development Team
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

#include "FrictionJoint.h"

#include <common/math.h>

// Module
#include "Body.h"
#include "World.h"

namespace love
{
namespace physics
{
namespace box2d
{
	FrictionJoint::FrictionJoint(Body * body1, Body * body2, float x, float y, bool collideConnected)
		: Joint(body1, body2), joint(NULL)
	{	
		b2FrictionJointDef def;
		def.Initialize(body1->body, body2->body, world->scaleDown(b2Vec2(x,y)));
		def.collideConnected = collideConnected;
		joint = (b2FrictionJoint*)createJoint(&def);
	}

	FrictionJoint::~FrictionJoint()
	{
		destroyJoint(joint);
		joint = 0;
	}
	
	void FrictionJoint::setMaxForce(float force)
	{
		joint->SetMaxForce(force);
	}

	float FrictionJoint::getMaxForce() const
	{
		return joint->GetMaxForce();
	}
	
	void FrictionJoint::setMaxTorque(float torque)
	{
		joint->SetMaxTorque(torque);
	}
	
	float FrictionJoint::getMaxTorque() const
	{
		return joint->GetMaxTorque();
	}


} // box2d
} // physics
} // love
