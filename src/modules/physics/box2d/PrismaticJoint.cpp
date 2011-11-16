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

#include "PrismaticJoint.h"

// Module
#include "Body.h"
#include "World.h"
#include "Physics.h"

namespace love
{
namespace physics
{
namespace box2d
{
	PrismaticJoint::PrismaticJoint(Body * body1, Body * body2, float xA, float yA, float xB, float yB, float ax, float ay, bool collideConnected)
		: Joint(body1, body2), joint(NULL)
	{
		b2PrismaticJointDef def;

		def.Initialize(body1->body, body2->body, Physics::scaleDown(b2Vec2(xA,yA)), b2Vec2(ax,ay));
		def.localAnchorB = body2->body->GetLocalPoint(Physics::scaleDown(b2Vec2(xB, yB)));
		def.lowerTranslation = 0.0f;
		def.upperTranslation = 100.0f;
		def.enableLimit = true;
		def.collideConnected = collideConnected;
		joint = (b2PrismaticJoint*)createJoint(&def);
	}

	PrismaticJoint::~PrismaticJoint()
	{
		destroyJoint(joint);
		joint = 0;
	}

	float PrismaticJoint::getJointTranslation() const
	{
		return Physics::scaleUp(joint->GetJointTranslation());
	}

	float PrismaticJoint::getJointSpeed() const
	{
		return Physics::scaleUp(joint->GetJointSpeed());
	}

	void PrismaticJoint::enableMotor(bool motor)
	{
		return joint->EnableMotor(motor);
	}

	bool PrismaticJoint::isMotorEnabled() const
	{
		return joint->IsMotorEnabled();
	}

	void PrismaticJoint::setMaxMotorForce(float force)
	{
		joint->SetMaxMotorForce(Physics::scaleDown(force));
	}

	void PrismaticJoint::setMotorSpeed(float speed)
	{
		joint->SetMotorSpeed(Physics::scaleDown(speed));
	}

	float PrismaticJoint::getMotorSpeed() const
	{
		return Physics::scaleUp(joint->GetMotorSpeed());
	}

	float PrismaticJoint::getMotorForce(float inv_dt) const
	{
		return Physics::scaleUp(joint->GetMotorForce(inv_dt));
	}

	float PrismaticJoint::getMaxMotorForce() const
	{
		return Physics::scaleUp(joint->GetMaxMotorForce());
	}

	void PrismaticJoint::enableLimit(bool limit)
	{
		joint->EnableLimit(limit);
	}

	bool PrismaticJoint::isLimitEnabled() const
	{
		return joint->IsLimitEnabled();
	}

	void PrismaticJoint::setUpperLimit(float limit)
	{
		joint->SetLimits(joint->GetLowerLimit(), Physics::scaleDown(limit));
	}

	void PrismaticJoint::setLowerLimit(float limit)
	{
		joint->SetLimits(Physics::scaleDown(limit), joint->GetUpperLimit());
	}

	void PrismaticJoint::setLimits(float lower, float upper)
	{
		joint->SetLimits(Physics::scaleDown(lower), Physics::scaleDown(upper));
	}

	float PrismaticJoint::getLowerLimit() const
	{
		return Physics::scaleUp(joint->GetLowerLimit());
	}

	float PrismaticJoint::getUpperLimit() const
	{
		return Physics::scaleUp(joint->GetUpperLimit());
	}

	int PrismaticJoint::getLimits(lua_State * L)
	{
		lua_pushnumber(L, Physics::scaleUp(joint->GetLowerLimit()));
		lua_pushnumber(L, Physics::scaleUp(joint->GetUpperLimit()));
		return 2;
	}


} // box2d
} // physics
} // love
