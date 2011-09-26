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

#include "ChainShape.h"

// Module
#include "Body.h"
#include "World.h"
#include "Physics.h"

#include <common/Memoizer.h>

namespace love
{
namespace physics
{
namespace box2d
{
	ChainShape::ChainShape(b2ChainShape * c, bool loop)
		: Shape(c), loop(loop)
	{
	}

	ChainShape::~ChainShape()
	{
		Memoizer::remove(shape);
		delete shape;
		shape = NULL;
	}
	
	void ChainShape::setNextVertex(float x, float y)
	{
		if (loop) {
			throw love::Exception("Physics error: Can't call setNextVertex on a loop ChainShape");
			return;
		}
		b2Vec2 v(x, y);
		b2ChainShape * c = (b2ChainShape *)shape;
		c->SetNextVertex(Physics::scaleDown(v));
	}
	
	void ChainShape::setPrevVertex(float x, float y)
	{
		if (loop) {
			throw love::Exception("Physics error: Can't call setPrevVertex on a loop ChainShape");
			return;
		}
		b2Vec2 v(x, y);
		b2ChainShape * c = (b2ChainShape *)shape;
		c->SetNextVertex(Physics::scaleDown(v));
	}
	
	EdgeShape * ChainShape::getChildEdge(int index) const
	{
		b2ChainShape * c = (b2ChainShape *)shape;
		b2EdgeShape e;
		c->GetChildEdge(&e, index);
		EdgeShape * edge = (EdgeShape *)Memoizer::find(&e);
		if (!edge) return new EdgeShape(&e);
		else {
			edge->retain();
			return edge;
		}
	}
	
	int ChainShape::getChildCount() const
	{
		b2ChainShape * c = (b2ChainShape *)shape;
		return c->GetChildCount();
	}
	
	int ChainShape::getVertexCount() const
	{
		b2ChainShape * c = (b2ChainShape *)shape;
		return c->m_count;
	}
	
	b2Vec2 ChainShape::getPoint(int index) const
	{
		b2ChainShape * c = (b2ChainShape *)shape;
		const b2Vec2 & v = c->m_vertices[index];
		return Physics::scaleUp(v);
	}
	
	const b2Vec2 * ChainShape::getPoints() const
	{
		b2ChainShape * c = (b2ChainShape *)shape;
		return c->m_vertices;
	}

} // box2d
} // physics
} // love
