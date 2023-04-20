#include "stdafx.h"
#include "SmAccount.h"
#include "../Util/IdGenerator.h"
namespace DarkHorse {
	SmAccount::SmAccount()
		: _id(IdGenerator::get_id())
	{
	}
	SmAccount::~SmAccount()
	{

	}
}


