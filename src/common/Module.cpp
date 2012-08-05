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

// LOVE
#include "Module.h"
#include "Exception.h"

// std
#include <map>
#include <utility>
#include <string>

namespace
{
	std::map<std::string, love::Module*> registry;
} // anonymous namespace

namespace love
{
	void Module::registerInstance(Module *instance)
	{
		if (instance == NULL)
			throw Exception("Module instance is NULL");

		std::string name(instance->getName());

		std::map<std::string, Module*>::iterator it = registry.find(name);
		if (registry.end() != it)
			throw Exception("Module %s already registered!", instance->getName());

		registry.insert(make_pair(name, instance));
	}

	Module *Module::getInstance(const char *name)
	{
		std::map<std::string, Module*>::iterator it = registry.find(std::string(name));

		if (registry.end() == it)
			return NULL;

		return it->second;
	}
} // namespace love
