#include "stdafx.h"
#include "FundOrderWindow.h"
#include "../../Util/IdGenerator.h"
namespace DarkHorse {

FundOrderWindow::FundOrderWindow()
{
	_id = DarkHorse::IdGenerator::get_id();
}

FundOrderWindow::~FundOrderWindow()
{

}

}