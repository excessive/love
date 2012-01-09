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

#include "Contact.h"
#include "World.h"
#include "Physics.h"

#include <common/Memoizer.h>

namespace love
{
namespace physics
{
namespace box2d
{
	Contact::Contact(b2Contact * contact)
		: contact(contact)
	{
		Memoizer::add(contact, this);
	}

	Contact::~Contact()
	{
		Memoizer::remove(contact);
	}

	int Contact::getPositions(lua_State * L)
	{
		love::luax_assert_argc(L, 1, 1);
		b2WorldManifold manifold;
		contact->GetWorldManifold(&manifold);
		int points = contact->GetManifold()->pointCount;
		for (int i = 0; i < points; i++)
		{
			b2Vec2 position = Physics::scaleUp(manifold.points[i]);
			lua_pushnumber(L, position.x);
			lua_pushnumber(L, position.y);
		}
		return points*2;
	}

	int Contact::getNormal(lua_State * L)
	{
		love::luax_assert_argc(L, 1, 1);
		b2WorldManifold manifold;
		contact->GetWorldManifold(&manifold);
		lua_pushnumber(L, Physics::scaleUp(manifold.normal.x));
		lua_pushnumber(L, Physics::scaleUp(manifold.normal.y));
		return 2;
	}

	float Contact::getFriction() const
	{
		return contact->GetFriction();
	}

	float Contact::getRestitution() const
	{
		return contact->GetRestitution();
	}

	bool Contact::isEnabled() const
	{
		return contact->IsEnabled();
	}

	bool Contact::isTouching() const
	{
		return contact->IsTouching();
	}

	void Contact::setFriction(float friction)
	{
		contact->SetFriction(friction);
	}

	void Contact::setRestitution(float restitution)
	{
		contact->SetRestitution(restitution);
	}

	void Contact::setEnabled(bool enabled)
	{
		contact->SetEnabled(enabled);
	}

	void Contact::resetFriction()
	{
		contact->ResetFriction();
	}

	void Contact::resetRestitution()
	{
		contact->ResetRestitution();
	}

} // box2d
} // physics
} // love
