#include "stdafx.h"
#include "DmFundOrderWindow.h"
#include "../../Util/IdGenerator.h"
namespace DarkHorse {

DmFundOrderWindow::DmFundOrderWindow()
{
	_id = DarkHorse::IdGenerator::get_id();
}

DmFundOrderWindow::~DmFundOrderWindow()
{

}

}