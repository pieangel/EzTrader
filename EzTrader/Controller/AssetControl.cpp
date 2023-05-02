#include "stdafx.h"
#include "AssetControl.h"
#include "../Util/IdGenerator.h"
namespace DarkHorse {

AssetControl::AssetControl()
	: id_(IdGenerator::get_id())
{

}

AssetControl::~AssetControl()
{

}

}