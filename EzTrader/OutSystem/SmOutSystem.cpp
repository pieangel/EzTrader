#include "stdafx.h"
#include "SmOutSystem.h"
#include "../Util/IdGenerator.h"

namespace DarkHorse {
	SmOutSystem::SmOutSystem(const std::string& name)
		:name_(name), id_(IdGenerator::get_id())
	{
	}


	SmOutSystem::~SmOutSystem()
	{
	}
}
