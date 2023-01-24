#include "stdafx.h"
#include "SmCell.h"
#include "../Order/SmOrderRequest.h"
#include "../Order/SmOrder.h"

size_t DarkHorse::SmCell::GetOrderReqCount()
{
	size_t sum = 0;
	for (auto it = _OrderReqMap.begin(); it != _OrderReqMap.end(); ++it) {
		sum += it->second->OrderAmount;
	}
	return sum;
}

size_t DarkHorse::SmCell::GetOrderCount()
{
	size_t sum = 0;
	for (auto it = _OrderMap.begin(); it != _OrderMap.end(); ++it) {
		sum += it->second->OrderAmount;
	}
	return sum;
}

void DarkHorse::SmCell::AddOrderReq(const std::shared_ptr<SmOrderRequest>& order_req)
{
	_OrderReqMap[order_req->RequestId] = order_req;
}

void DarkHorse::SmCell::AddOrder(const std::shared_ptr<SmOrder>& order)
{
	_OrderMap[order->Id()] = order;
}

void DarkHorse::SmCell::ClearOrderReq()
{
	_OrderReqMap.clear();
}

void DarkHorse::SmCell::ClearOrder()
{
	_OrderMap.clear();
}

CBCGPRect DarkHorse::SmCell::GetCellRect()
{
	return CBCGPRect(_X, _Y, _X + _Width, _Y + _Height);
}
