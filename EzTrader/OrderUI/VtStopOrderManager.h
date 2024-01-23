#pragma once
#include <map>
#include <list>
#include "../Order/VtOrder.h"

class VtStopOrderManager
{
public:
	VtStopOrderManager();
	~VtStopOrderManager();
	
	std::map<int, VtOrderRequest*> StopOrderMap;
	void AddOrder(VtOrderRequest* req);
	VtOrderRequest* FindOrder(int reqID);
	void RemoveOrder(int reqID);
	void RemoveAll();
	void RemoveBuyAll();
	void RemoveSellAll();
	int GetOrderID()
	{
		return _OrderID++;
	}
	std::list<VtOrderRequest*> FinishedOrderList;

	std::map<int, std::shared_ptr<HdOrderRequest>> StopOrderMapHd;
	void AddOrderHd(std::shared_ptr<HdOrderRequest> req);
	//std::list<HdOrderRequest*> FinishedOrderListHd;

	std::shared_ptr<HdOrderRequest> FindOrderHd(int reqID);
	void RemoveOrderHd(int reqID);
	void RemoveAllHd();
	void RemoveBuyAllHd();
	void RemoveSellAllHd();
	int Slip() const { return _Slip; }
	void Slip(int val) { _Slip = val; }
private:
	int _OrderID = 0;
	int _Slip = 2;
};

