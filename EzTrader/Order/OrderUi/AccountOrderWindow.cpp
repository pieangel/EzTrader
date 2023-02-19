#include "stdafx.h"
#include "AccountOrderWindow.h"
#include "../../Util/IdGenerator.h"

AccountOrderWindow::AccountOrderWindow()
{
	_id = DarkHorse::IdGenerator::get_id();
}

AccountOrderWindow::~AccountOrderWindow()
{

}
