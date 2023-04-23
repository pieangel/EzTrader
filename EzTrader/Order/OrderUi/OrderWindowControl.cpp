#include "stdafx.h"
#include <functional>
#include "OrderWindowControl.h"
#include "../../Global/SmTotalManager.h"
#include "../../Event/EventHub.h"

namespace DarkHorse {

OrderWindowControl::OrderWindowControl(const int id)
	: _id(id)
{
	subscribe_remove_order_event();
}

OrderWindowControl::~OrderWindowControl()
{

}

void OrderWindowControl::remove_order(const std::string& order_no)
{

}

void OrderWindowControl::update_position(std::shared_ptr<Position> position)
{

}

void OrderWindowControl::subscribe_remove_order_event()
{
	
}

}