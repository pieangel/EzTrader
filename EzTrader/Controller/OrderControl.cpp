#include "stdafx.h"
#include "OrderControl.h"
namespace DarkHorse {
	void OrderControl::on_order_accepted(std::shared_ptr<SmOrder> order)
	{
		if (!order) return;
	}
	void OrderControl::on_order_unfilled(std::shared_ptr<SmOrder> order)
	{
		if (!order) return;
	}
}