#include "stdafx.h"
#include "AcceptedOrderControl.h"
#include "../Order/SmOrder.h"
namespace DarkHorse {
	void AcceptedOrderControl::update_order(std::shared_ptr<SmOrder> order_p)
	{
		AcceptedOrder order;
		order.account_no = order_p->AccountNo;
		order.symbol_code = order_p->SymbolCode;
	}
}