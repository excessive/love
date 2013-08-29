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
#include "wrap_Physics.h"
#include "wrap_World.h"
#include "wrap_Contact.h"
#include "wrap_Body.h"
#include "wrap_Fixture.h"
#include "wrap_Shape.h"
#include "wrap_CircleShape.h"
#include "wrap_PolygonShape.h"
#include "wrap_EdgeShape.h"
#include "wrap_ChainShape.h"
#include "wrap_Joint.h"
#include "wrap_MouseJoint.h"
#include "wrap_DistanceJoint.h"
#include "wrap_PrismaticJoint.h"
#include "wrap_RevoluteJoint.h"
#include "wrap_PulleyJoint.h"
#include "wrap_GearJoint.h"
#include "wrap_FrictionJoint.h"
#include "wrap_WeldJoint.h"
#include "wrap_WheelJoint.h"
#include "wrap_RopeJoint.h"

namespace love
{
namespace physics
{
namespace box2d
{

static Physics *instance = 0;

int w_newWorld(lua_State *L)
{
	float gx = (float)luaL_optnumber(L, 1, 0);
	float gy = (float)luaL_optnumber(L, 2, 0);
	bool sleep = luax_optboolean(L, 3, true);

	World *w;
	ASSERT_GUARD(w = instance->newWorld(gx, gy, sleep);)
	luax_pushtype(L, "World", PHYSICS_WORLD_T, w);

	return 1;
}

int w_newBody(lua_State *L)
{
	World *world = luax_checktype<World>(L, 1, "World", PHYSICS_WORLD_T);
	float x = (float)luaL_optnumber(L, 2, 0.0);
	float y = (float)luaL_optnumber(L, 3, 0.0);

	Body::Type btype = Body::BODY_STATIC;
	const char *typestr = lua_isnoneornil(L, 4) ? 0 : lua_tostring(L, 4);
	if (typestr && !Body::getConstant(typestr, btype))
		return luaL_error(L, "Invalid Body type: %s", typestr);

	Body *body;
	ASSERT_GUARD(body = instance->newBody(world, x, y, btype);)
	luax_pushtype(L, "Body", PHYSICS_BODY_T, body);
	return 1;
}

int w_newFixture(lua_State *L)
{
	Body *body = luax_checkbody(L, 1);
	Shape *shape = luax_checkshape(L, 2);
	float density = (float)luaL_optnumber(L, 3, 1.0f);
	Fixture *fixture;
	ASSERT_GUARD(fixture = instance->newFixture(body, shape, density);)
	luax_pushtype(L, "Fixture", PHYSICS_FIXTURE_T, fixture);
	return 1;
}

int w_newCircleShape(lua_State *L)
{
	int top = lua_gettop(L);

	if (top == 1)
	{
		float radius = (float)luaL_checknumber(L, 1);
		CircleShape *shape;
		ASSERT_GUARD(shape = instance->newCircleShape(radius);)
		luax_pushtype(L, "CircleShape", PHYSICS_CIRCLE_SHAPE_T, shape);
		return 1;
	}
	else if (top == 3)
	{
		float x = (float)luaL_checknumber(L, 1);
		float y = (float)luaL_checknumber(L, 2);
		float radius = (float)luaL_checknumber(L, 3);
		CircleShape *shape;
		ASSERT_GUARD(shape = instance->newCircleShape(x, y, radius);)
		luax_pushtype(L, "CircleShape", PHYSICS_CIRCLE_SHAPE_T, shape);
		return 1;
	}
	else
		return luaL_error(L, "Incorrect number of parameters");
}

int w_newRectangleShape(lua_State *L)
{
	int top = lua_gettop(L);

	if (top == 2)
	{
		float w = (float)luaL_checknumber(L, 1);
		float h = (float)luaL_checknumber(L, 2);
		PolygonShape *shape;
		ASSERT_GUARD(shape = instance->newRectangleShape(w, h);)
		luax_pushtype(L, "PolygonShape", PHYSICS_POLYGON_SHAPE_T, shape);
		return 1;
	}
	else if (top == 4 || top == 5)
	{
		float x = (float)luaL_checknumber(L, 1);
		float y = (float)luaL_checknumber(L, 2);
		float w = (float)luaL_checknumber(L, 3);
		float h = (float)luaL_checknumber(L, 4);
		float angle = (float)luaL_optnumber(L, 5, 0);
		PolygonShape *shape;
		ASSERT_GUARD(shape = instance->newRectangleShape(x, y, w, h, angle);)
		luax_pushtype(L, "PolygonShape", PHYSICS_POLYGON_SHAPE_T, shape);
		return 1;
	}
	else
		return luaL_error(L, "Incorrect number of parameters");
}

int w_newEdgeShape(lua_State *L)
{
	float x1 = (float)luaL_checknumber(L, 1);
	float y1 = (float)luaL_checknumber(L, 2);
	float x2 = (float)luaL_checknumber(L, 3);
	float y2 = (float)luaL_checknumber(L, 4);
	EdgeShape *shape;
	ASSERT_GUARD(shape = instance->newEdgeShape(x1, y1, x2, y2);)
	luax_pushtype(L, "EdgeShape", PHYSICS_EDGE_SHAPE_T, shape);
	return 1;
}

int w_newPolygonShape(lua_State *L)
{
	ASSERT_GUARD(return instance->newPolygonShape(L);)
}

int w_newChainShape(lua_State *L)
{
	ASSERT_GUARD(return instance->newChainShape(L);)
}

int w_newDistanceJoint(lua_State *L)
{
	Body *body1 = luax_checktype<Body>(L, 1, "Body", PHYSICS_BODY_T);
	Body *body2 = luax_checktype<Body>(L, 2, "Body", PHYSICS_BODY_T);
	float x1 = (float)luaL_checknumber(L, 3);
	float y1 = (float)luaL_checknumber(L, 4);
	float x2 = (float)luaL_checknumber(L, 5);
	float y2 = (float)luaL_checknumber(L, 6);
	bool collideConnected = luax_optboolean(L, 7, false);
	DistanceJoint *j;
	ASSERT_GUARD(j = instance->newDistanceJoint(body1, body2, x1, y1, x2, y2, collideConnected);)
	luax_pushtype(L, "DistanceJoint", PHYSICS_DISTANCE_JOINT_T, j);
	return 1;
}

int w_newMouseJoint(lua_State *L)
{
	Body *body = luax_checktype<Body>(L, 1, "Body", PHYSICS_BODY_T);
	float x = (float)luaL_checknumber(L, 2);
	float y = (float)luaL_checknumber(L, 3);
	MouseJoint *j;
	ASSERT_GUARD(j = instance->newMouseJoint(body, x, y);)
	luax_pushtype(L, "MouseJoint", PHYSICS_MOUSE_JOINT_T, j);
	return 1;
}

int w_newRevoluteJoint(lua_State *L)
{
	Body *body1 = luax_checktype<Body>(L, 1, "Body", PHYSICS_BODY_T);
	Body *body2 = luax_checktype<Body>(L, 2, "Body", PHYSICS_BODY_T);
	float x = (float)luaL_checknumber(L, 3);
	float y = (float)luaL_checknumber(L, 4);
	bool collideConnected = luax_optboolean(L, 5, false);
	RevoluteJoint *j;
	ASSERT_GUARD(j = instance->newRevoluteJoint(body1, body2, x, y, collideConnected);)
	luax_pushtype(L, "RevoluteJoint", PHYSICS_REVOLUTE_JOINT_T, j);
	return 1;
}

int w_newPrismaticJoint(lua_State *L)
{
	Body *body1 = luax_checktype<Body>(L, 1, "Body", PHYSICS_BODY_T);
	Body *body2 = luax_checktype<Body>(L, 2, "Body", PHYSICS_BODY_T);
	float xA = (float)luaL_checknumber(L, 3);
	float yA = (float)luaL_checknumber(L, 4);
	float xB, yB, ax, ay;
	bool collideConnected;
	if (lua_gettop(L) >= 8)
	{
		xB = (float)luaL_checknumber(L, 5);
		yB = (float)luaL_checknumber(L, 6);
		ax = (float)luaL_checknumber(L, 7);
		ay = (float)luaL_checknumber(L, 8);
		collideConnected = luax_optboolean(L, 9, false);
	}
	else
	{
		xB = xA;
		yB = yA;
		ax = (float)luaL_checknumber(L, 5);
		ay = (float)luaL_checknumber(L, 6);
		collideConnected = luax_optboolean(L, 7, false);
	}
	PrismaticJoint *j;
	ASSERT_GUARD(j = instance->newPrismaticJoint(body1, body2, xA, yA, xB, yB, ax, ay, collideConnected);)
	luax_pushtype(L, "PrismaticJoint", PHYSICS_PRISMATIC_JOINT_T, j);
	return 1;
}

int w_newPulleyJoint(lua_State *L)
{
	Body *body1 = luax_checktype<Body>(L, 1, "Body", PHYSICS_BODY_T);
	Body *body2 = luax_checktype<Body>(L, 2, "Body", PHYSICS_BODY_T);
	float gx1 = (float)luaL_checknumber(L, 3);
	float gy1 = (float)luaL_checknumber(L, 4);
	float gx2 = (float)luaL_checknumber(L, 5);
	float gy2 = (float)luaL_checknumber(L, 6);
	float x1 = (float)luaL_checknumber(L, 7);
	float y1 = (float)luaL_checknumber(L, 8);
	float x2 = (float)luaL_checknumber(L, 9);
	float y2 = (float)luaL_checknumber(L, 10);
	float ratio = (float)luaL_optnumber(L, 11, 1.0);
	bool collideConnected = luax_optboolean(L, 12, true); // PulleyJoints default to colliding connected bodies, see b2PulleyJoint.h

	PulleyJoint *j;
	ASSERT_GUARD(j = instance->newPulleyJoint(body1, body2, b2Vec2(gx1,gy1), b2Vec2(gx2,gy2), b2Vec2(x1,y1), b2Vec2(x2,y2), ratio, collideConnected);)
	luax_pushtype(L, "PulleyJoint", PHYSICS_PULLEY_JOINT_T, j);
	return 1;
}

int w_newGearJoint(lua_State *L)
{
	Joint *joint1 = luax_checktype<Joint>(L, 1, "Joint", PHYSICS_JOINT_T);
	Joint *joint2 = luax_checktype<Joint>(L, 2, "Joint", PHYSICS_JOINT_T);
	float ratio = (float)luaL_optnumber(L, 3, 1.0);
	bool collideConnected = luax_optboolean(L, 4, false);

	GearJoint *j;
	ASSERT_GUARD(j = instance->newGearJoint(joint1, joint2, ratio, collideConnected);)
	luax_pushtype(L, "GearJoint", PHYSICS_GEAR_JOINT_T, j);
	return 1;
}

int w_newFrictionJoint(lua_State *L)
{
	Body *body1 = luax_checktype<Body>(L, 1, "Body", PHYSICS_BODY_T);
	Body *body2 = luax_checktype<Body>(L, 2, "Body", PHYSICS_BODY_T);
	float xA = (float)luaL_checknumber(L, 3);
	float yA = (float)luaL_checknumber(L, 4);
	float xB, yB;
	bool collideConnected;
	if (lua_gettop(L) >= 6)
	{
		xB = (float)luaL_checknumber(L, 5);
		yB = (float)luaL_checknumber(L, 6);
		collideConnected = luax_optboolean(L, 7, false);
	}
	else
	{
		xB = xA;
		yB = yA;
		collideConnected = luax_optboolean(L, 5, false);
	}
	FrictionJoint *j;
	ASSERT_GUARD(j = instance->newFrictionJoint(body1, body2, xA, yA, xB, yB, collideConnected);)
	luax_pushtype(L, "FrictionJoint", PHYSICS_FRICTION_JOINT_T, j);
	return 1;
}

int w_newWeldJoint(lua_State *L)
{
	Body *body1 = luax_checktype<Body>(L, 1, "Body", PHYSICS_BODY_T);
	Body *body2 = luax_checktype<Body>(L, 2, "Body", PHYSICS_BODY_T);
	float xA = (float)luaL_checknumber(L, 3);
	float yA = (float)luaL_checknumber(L, 4);
	float xB, yB;
	bool collideConnected;
	if (lua_gettop(L) >= 6)
	{
		xB = (float)luaL_checknumber(L, 5);
		yB = (float)luaL_checknumber(L, 6);
		collideConnected = luax_optboolean(L, 7, false);
	}
	else
	{
		xB = xA;
		yB = yA;
		collideConnected = luax_optboolean(L, 5, false);
	}
	WeldJoint *j;
	ASSERT_GUARD(j = instance->newWeldJoint(body1, body2, xA, yA, xB, yB, collideConnected);)
	luax_pushtype(L, "WeldJoint", PHYSICS_WELD_JOINT_T, j);
	return 1;
}

int w_newWheelJoint(lua_State *L)
{
	Body *body1 = luax_checktype<Body>(L, 1, "Body", PHYSICS_BODY_T);
	Body *body2 = luax_checktype<Body>(L, 2, "Body", PHYSICS_BODY_T);
	float xA = (float)luaL_checknumber(L, 3);
	float yA = (float)luaL_checknumber(L, 4);
	float xB, yB, ax, ay;
	bool collideConnected;
	if (lua_gettop(L) >= 8)
	{
		xB = (float)luaL_checknumber(L, 5);
		yB = (float)luaL_checknumber(L, 6);
		ax = (float)luaL_checknumber(L, 7);
		ay = (float)luaL_checknumber(L, 8);
		collideConnected = luax_optboolean(L, 9, false);
	}
	else
	{
		xB = xA;
		yB = yA;
		ax = (float)luaL_checknumber(L, 5);
		ay = (float)luaL_checknumber(L, 6);
		collideConnected = luax_optboolean(L, 7, false);
	}

	WheelJoint *j;
	ASSERT_GUARD(j = instance->newWheelJoint(body1, body2, xA, yA, xB, yB, ax, ay, collideConnected);)
	luax_pushtype(L, "WheelJoint", PHYSICS_WHEEL_JOINT_T, j);
	return 1;
}

int w_newRopeJoint(lua_State *L)
{
	Body *body1 = luax_checktype<Body>(L, 1, "Body", PHYSICS_BODY_T);
	Body *body2 = luax_checktype<Body>(L, 2, "Body", PHYSICS_BODY_T);
	float x1 = (float)luaL_checknumber(L, 3);
	float y1 = (float)luaL_checknumber(L, 4);
	float x2 = (float)luaL_checknumber(L, 5);
	float y2 = (float)luaL_checknumber(L, 6);
	float maxLength = (float)luaL_checknumber(L, 7);
	bool collideConnected = luax_optboolean(L, 8, false);
	RopeJoint *j;
	ASSERT_GUARD(j = instance->newRopeJoint(body1, body2, x1, y1, x2, y2, maxLength, collideConnected);)
	luax_pushtype(L, "RopeJoint", PHYSICS_ROPE_JOINT_T, j);
	return 1;
}

int w_getDistance(lua_State *L)
{
	return instance->getDistance(L);
}

int w_setMeter(lua_State *L)
{
	int arg1 = luaL_checkint(L, 1);
	try
	{
		Physics::setMeter(arg1);
	}
	catch(love::Exception &e)
	{
		return luaL_error(L, e.what());
	}
	return 0;

}
int w_getMeter(lua_State *L)
{
	lua_pushinteger(L, Physics::getMeter());
	return 1;
}

// List of functions to wrap.
static const luaL_Reg functions[] =
{
	{ "newWorld", w_newWorld },
	{ "newBody", w_newBody },
	{ "newFixture", w_newFixture },
	{ "newCircleShape", w_newCircleShape },
	{ "newRectangleShape", w_newRectangleShape },
	{ "newPolygonShape", w_newPolygonShape },
	{ "newEdgeShape", w_newEdgeShape },
	{ "newChainShape", w_newChainShape },
	{ "newDistanceJoint", w_newDistanceJoint },
	{ "newMouseJoint", w_newMouseJoint },
	{ "newRevoluteJoint", w_newRevoluteJoint },
	{ "newPrismaticJoint", w_newPrismaticJoint },
	{ "newPulleyJoint", w_newPulleyJoint },
	{ "newGearJoint", w_newGearJoint },
	{ "newFrictionJoint", w_newFrictionJoint },
	{ "newWeldJoint", w_newWeldJoint },
	{ "newWheelJoint", w_newWheelJoint },
	{ "newRopeJoint", w_newRopeJoint },
	{ "getDistance", w_getDistance },
	{ "getMeter", w_getMeter },
	{ "setMeter", w_setMeter },
	{ 0, 0 },
};

static const lua_CFunction types[] =
{
	luaopen_world,
	luaopen_contact,
	luaopen_body,
	luaopen_fixture,
	luaopen_shape,
	luaopen_circleshape,
	luaopen_polygonshape,
	luaopen_edgeshape,
	luaopen_chainshape,
	luaopen_joint,
	luaopen_mousejoint,
	luaopen_distancejoint,
	luaopen_prismaticjoint,
	luaopen_revolutejoint,
	luaopen_pulleyjoint,
	luaopen_gearjoint,
	luaopen_frictionjoint,
	luaopen_weldjoint,
	luaopen_wheeljoint,
	luaopen_ropejoint,
	0
};

extern "C" int luaopen_love_physics(lua_State *L)
{
	if (instance == 0)
	{
		try
		{
			instance = new Physics();
		}
		catch(Exception &e)
		{
			return luaL_error(L, e.what());
		}
	}
	else
		instance->retain();

	WrappedModule w;
	w.module = instance;
	w.name = "physics";
	w.flags = MODULE_T;
	w.functions = functions;
	w.types = types;

	return luax_register_module(L, w);
}

} // box2d
} // physics
} // love
