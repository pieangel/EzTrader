#pragma once
#include <map>
#include <list>
#include <tuple>
#include "../Order/VtOrder.h"
#include "../Global/VtDefine.h"

struct VtPosition;
class VtSymbol;
class VtFund;
class VtAccount;
class VtOrderConfigManager;
class VtOrderManager;
class VtCutManager
{
public:
	VtCutManager();
	~VtCutManager();

	bool EnableCutProfit() const { return _EnableCutProfit; }
	void EnableCutProfit(bool val) { _EnableCutProfit = val; }
	bool EnableCutLoss() const { return _EnableCutLoss; }
	void EnableCutLoss(bool val) { _EnableCutLoss = val; }
	int CutProfit() const { return _CutProfit; }
	void CutProfit(int val) { _CutProfit = val; }
	int CutLoss() const { return _CutLoss; }
	void CutLoss(int val) { _CutLoss = val; }
	int OrderType() const { return _OrderType; }
	void OrderType(int val) { _OrderType = val; }
	int OrderTypeSlip() const { return _OrderTypeSlip; }
	void OrderTypeSlip(int val) { _OrderTypeSlip = val; }

	void AddOrderHd(std::shared_ptr<HdOrderRequest> req);
	//std::list<HdOrderRequest*> FinishedOrderListHd;

	std::shared_ptr<HdOrderRequest> FindOrderHd(int reqID);
	void RemoveOrderHd(int reqID, bool resetProfitLoss = true);
	void RemoveAllHd();
	void RemoveBuyAllHd();
	void RemoveSellAllHd();


	/// <summary>
	/// mode : 0 -> 익절, 1 ->손절
	/// </summary>
	/// <param name="mode"></param>
	
	void MakePositionStop(int mode, VtSymbol* sym);
	void MakeAutoStopList(int mode, VtPosition* posi, VtSymbol* sym, VtFund* fund);
	std::shared_ptr<HdOrderRequest> MakePositionStopOrderRequest(int mode, VtPosition* posi, VtSymbol* sym, VtAccount* acnt, VtFund* fund);
	void MakePairStopOrderRequest(VtPosition* posi, VtSymbol* sym, VtAccount* acnt, VtFund* fund);
	void AddStopOrderForFilled(VtSymbol* sym, VtOrder* order);
	void MakePositionStopByRemain(int mode, VtSymbol* sym);
	std::shared_ptr<HdOrderRequest> MakePositionStopOrderRequest(int mode, VtOrder* order, VtSymbol* sym, VtAccount* acnt, VtFund* fund);
	std::shared_ptr<HdOrderRequest> MakePositionStopOrderRequest(int mode, VtOrder* order, VtSymbol* sym, VtAccount* acnt);
	void MakePairStopOrderRequest(VtOrder* order, VtSymbol* sym, VtAccount* acnt);
	void MakePositionStopByRemain(VtSymbol* sym);
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	void UpdateCutInfo();
	std::map<int, std::shared_ptr<HdOrderRequest>>& GetStopOrderMap()
	{
		return StopOrderMapHd;
	}
	void ApplyProfitLossForPosition(VtSymbol* sym);
	std::shared_ptr<HdOrderRequest> FindStop(int reqId);
	void ResetFlag(VtOrder* order);
	static bool CheckCutLoss(VtPosition* posi, VtSymbol* sym, int size);
	static bool CheckCutProfit(VtPosition* posi, VtSymbol* sym, int size);
	static bool CheckCutLossByPercent(VtPosition* posi, VtSymbol* sym, double percent);
	static bool CheckCutProfitByPercent(VtPosition* posi, VtSymbol* sym, double percent);
	bool CheckCutLoss(VtOrder* order, VtSymbol* sym, int size);
	bool CheckCutProfit(VtOrder* order, VtSymbol* sym, int size);
	bool CheckProfitLoss(VtSymbol* sym);
	// 모두 청산을 눌렀을 때 작동
	void LiqudAll(VtSymbol* sym, VtPriceType priceType, int price);
private:
	std::pair<int, VtOrder*> GetTotalRemain(VtSymbol* sym, VtOrderManager* orderMgr);
	int GetAvg(VtSymbol* sym);
	void RecalPosByAvg(VtSymbol* sym);
	VtPosition* FindPositionByOrder(VtOrder* order);
	std::map<int, std::shared_ptr<HdOrderRequest>> StopOrderMapHd;
	// account number, symbol code, profit or loss cut.
	std::map<std::tuple<std::string, std::string, int>, std::shared_ptr<HdOrderRequest>> CutInfoMap;
	void RemoveCutInfo(std::shared_ptr<HdOrderRequest> req, bool resetProfitLoss);
	bool _EnableCutProfit = false;
	bool _EnableCutLoss = false;

	int _CutProfit = 1;
	int _CutLoss = 1;
	int _OrderType = 1;
	int _OrderTypeSlip = 2;

	VtFilledCondition _FillCondition = VtFilledCondition::Fas;

	VtOrderConfigManager* _OrderConfigMgr;
	void PutOrder(VtOrder* order, VtSymbol* sym, int reqType, VtPriceType priceType = VtPriceType::Price);
	void LiqudOrder(VtSymbol* sym, VtOrder* topOrder, int totalRemain, VtPriceType priceType, int price);
};

