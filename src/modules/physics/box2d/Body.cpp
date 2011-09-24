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

#include "Body.h"

#include <common/math.h>

#include "Shape.h"
#include "Fixture.h"
#include "World.h"
#include "Physics.h"

namespace love
{
namespace physics
{
namespace box2d
{
	Body::Body(World * world, b2Vec2 p, Body::Type type)
		: world(world)
	{
		world->retain();
		b2BodyDef def;
		def.position = Physics::scaleDown(p);
		body = world->world->CreateBody(&def);
		this->setType(type);
	}

	Body::~Body()
	{
		world->world->DestroyBody(body);
		world->release();
		body = 0;
	}

	float Body::getX()
	{
		return Physics::scaleUp(body->GetPosition().x);
	}

	float Body::getY()
	{
		return Physics::scaleUp(body->GetPosition().y);
	}

	void Body::getPosition(float & x_o, float & y_o)
	{
		b2Vec2 v = Physics::scaleUp(body->GetPosition());
		x_o = v.x;
		y_o = v.y;
	}

	void Body::getLinearVelocity(float & x_o, float & y_o)
	{
		b2Vec2 v = Physics::scaleUp(body->GetLinearVelocity());
		x_o = v.x;
		y_o = v.y;
	}

	float Body::getAngle()
	{
		return body->GetAngle();
	}

	void Body::getWorldCenter(float & x_o, float & y_o)
	{
		b2Vec2 v = Physics::scaleUp(body->GetWorldCenter());
		x_o = v.x;
		y_o = v.y;
	}

	void Body::getLocalCenter(float & x_o, float & y_o)
	{
		b2Vec2 v = Physics::scaleUp(body->GetLocalCenter());
		x_o = v.x;
		y_o = v.y;
	}

	float Body::getAngularVelocity() const
	{
		return body->GetAngularVelocity();
	}

	float Body::getMass() const
	{
		return body->GetMass();
	}

	float Body::getInertia() const
	{
		return body->GetInertia();
	}
	
	int Body::getMassData(lua_State * L)
	{
		b2MassData data;
		body->GetMassData(&data);
		b2Vec2 center = Physics::scaleUp(data.center);
		lua_pushnumber(L, center.x);
		lua_pushnumber(L, center.y);
		lua_pushnumber(L, data.mass);
		lua_pushnumber(L, data.I);
		return 4;
	}

	float Body::getAngularDamping() const
	{
		return body->GetAngularDamping();
	}

	float Body::getLinearDamping() const
	{
		return body->GetLinearDamping();
	}
	
	float Body::getGravityScale() const
	{
		return body->GetGravityScale();
	}
	
	Body::Type Body::getType() const
	{
		switch (body->GetType()) {
			case b2_staticBody:
				return BODY_STATIC;
				break;
			case b2_dynamicBody:
				return BODY_DYNAMIC;
				break;
			case b2_kinematicBody:
				return BODY_KINEMATIC;
				break;
			default:
				return BODY_INVALID;
				break;
		}
	}

	void Body::applyLinearImpulse(float jx, float jy)
	{
		body->ApplyLinearImpulse(Physics::scaleDown(b2Vec2(jx, jy)), body->GetWorldCenter());
	}

	void Body::applyLinearImpulse(float jx, float jy, float rx, float ry)
	{
		body->ApplyLinearImpulse(Physics::scaleDown(b2Vec2(jx, jy)), Physics::scaleDown(b2Vec2(rx, ry)));
	}
	
	void Body::applyAngularImpulse(float impulse)
	{
		body->ApplyAngularImpulse(Physics::scaleDown(impulse));
	}

	void Body::applyTorque(float t)
	{
		body->ApplyTorque(t);
	}

	void Body::applyForce(float fx, float fy, float rx, float ry)
	{
		body->ApplyForce(b2Vec2(fx, fy), Physics::scaleDown(b2Vec2(rx, ry)));
	}

	void Body::applyForce(float fx, float fy)
	{
		body->ApplyForce(b2Vec2(fx, fy), body->GetWorldCenter());
	}

	void Body::setX(float x)
	{
		body->SetTransform(Physics::scaleDown(b2Vec2(x, getY())), getAngle());
	}

	void Body::setY(float y)
	{
		body->SetTransform(Physics::scaleDown(b2Vec2(getX(), y)), getAngle());
	}

	void Body::setLinearVelocity(float x, float y)
	{
		body->SetLinearVelocity(Physics::scaleDown(b2Vec2(x, y)));
	}

	void Body::setAngle(float d)
	{
		body->SetTransform(body->GetPosition(), d);
	}

	void Body::setAngularVelocity(float r)
	{
		body->SetAngularVelocity(r);
	}

	void Body::setPosition(float x, float y)
	{
		body->SetTransform(Physics::scaleDown(b2Vec2(x, y)), body->GetAngle());
	}

	void Body::setAngularDamping(float d)
	{
		body->SetAngularDamping(d);
	}

	void Body::setLinearDamping(float d)
	{
		body->SetLinearDamping(d);
	}

	void Body::resetMassData()
	{
		body->ResetMassData();
	}

	void Body::setMassData(float x, float y, float m, float i)
	{
		b2MassData massData;
		massData.center = Physics::scaleDown(b2Vec2(x, y));
		massData.mass = m;
		massData.I = i;
		body->SetMassData(&massData);
	}

	void Body::setInertia(float i)
	{
		b2MassData massData;
		massData.center = body->GetLocalCenter();
		massData.mass = body->GetMass();
		massData.I = i;
		body->SetMassData(&massData);
	}
	
	void Body::setGravityScale(float scale)
	{
		body->SetGravityScale(scale);
	}
	
	void Body::setType(Body::Type type)
	{
		switch (type) {
			case Body::BODY_STATIC:
				body->SetType(b2_staticBody);
				break;
			case Body::BODY_DYNAMIC:
				body->SetType(b2_dynamicBody);
				break;
			case Body::BODY_KINEMATIC:
				body->SetType(b2_kinematicBody);
				break;
			default:
				break;
		}
	}

	void Body::getWorldPoint(float x, float y, float & x_o, float & y_o)
	{
		b2Vec2 v = Physics::scaleUp(body->GetWorldPoint(Physics::scaleDown(b2Vec2(x, y))));
		x_o = v.x;
		y_o = v.y;
	}

	void Body::getWorldVector(float x, float y, float & x_o, float & y_o)
	{
		b2Vec2 v = Physics::scaleUp(body->GetWorldVector(Physics::scaleDown(b2Vec2(x, y))));
		x_o = v.x;
		y_o = v.y;
	}

	void Body::getLocalPoint(float x, float y, float & x_o, float & y_o)
	{
		b2Vec2 v = Physics::scaleUp(body->GetLocalPoint(Physics::scaleDown(b2Vec2(x, y))));
		x_o = v.x;
		y_o = v.y;
	}

	void Body::getLocalVector(float x, float y, float & x_o, float & y_o)
	{
		b2Vec2 v = Physics::scaleUp(body->GetLocalVector(Physics::scaleDown(b2Vec2(x, y))));
		x_o = v.x;
		y_o = v.y;
	}

	void Body::getLinearVelocityFromWorldPoint(float x, float y, float & x_o, float & y_o)
	{
		b2Vec2 v = Physics::scaleUp(body->GetLinearVelocityFromWorldPoint(Physics::scaleDown(b2Vec2(x, y))));
		x_o = v.x;
		y_o = v.y;
	}

	void Body::getLinearVelocityFromLocalPoint(float x, float y, float & x_o, float & y_o)
	{
		b2Vec2 v = Physics::scaleUp(body->GetLinearVelocityFromLocalPoint(Physics::scaleDown(b2Vec2(x, y))));
		x_o = v.x;
		y_o = v.y;
	}

	bool Body::isBullet() const
	{
		return body->IsBullet();
	}

	void Body::setBullet(bool bullet)
	{
		return body->SetBullet(bullet);
	}

	bool Body::isActive() const
	{
		return body->IsActive();
	}

	bool Body::isAwake() const
	{
		return body->IsAwake();
	}

	void Body::setSleepingAllowed(bool allow)
	{
		body->SetSleepingAllowed(allow);
	}

	bool Body::isSleepingAllowed() const
	{
		return body->IsSleepingAllowed();
	}
	
	void Body::setActive(bool active)
	{
		body->SetActive(active);
	}

	void Body::setAwake(bool awake)
	{
		body->SetAwake(awake);
	}

	void Body::setFixedRotation(bool fixed)
	{
		body->SetFixedRotation(fixed);
	}

	bool Body::isFixedRotation() const
	{
		return body->IsFixedRotation();
	}

	World * Body::getWorld() const
	{
		return world;
	}
	
	Fixture * Body::createFixture(Shape * shape)
	{
		return new Fixture(this, shape);
	}
	
	void Body::destroyFixture(Fixture * fixture)
	{
		fixture->release();
	}

	b2Vec2 Body::getVector(lua_State * L)
	{
		love::luax_assert_argc(L, 2, 2);
		b2Vec2 v((float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2));
		lua_pop(L, 2);
		return v;
	}

	int Body::pushVector(lua_State * L, const b2Vec2 & v)
	{
		lua_pushnumber(L, v.x);
		lua_pushnumber(L, v.y);
		return 2;
	}

} // box2d
} // physics
} // love
