#include "stdafx.h"
#include "VtOrderConfigManager.h"
//#include "../Order/VtOrderManager.h"
//#include "../Order/VtOrderManagerSelector.h"
//#include "../Account/VtAccount.h"
//#include "../Symbol/VtSymbol.h"
//#include "../Order/VtProductOrderManagerSelector.h"
//#include "../Order/VtProductOrderManager.h"
#include <string>
//#include "VtOrderDialogManager.h"
//#include "../Symbol/VtRealtimeRegisterManager.h"
//#include "../Global/MainBeetle.h"
//#include "../Account/VtAccountManager.h"
//#include "../Order/VtFundOrderManager.h"
//#include "../Order/VtFundOrderManagerSelecter.h"
//#include "../Fund/VtFund.h"
//#include "../Symbol/SmMarketManager.h"
#include "../Global/SmTotalManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VtOrderConfigManager::VtOrderConfigManager()
{
	_Fund = nullptr;
	_Account = nullptr;
	_Symbol = nullptr;
	_Hoga = nullptr;
	_Quote = nullptr;
	_Master = nullptr;
	_Position = nullptr;
	_OrderMgr = nullptr;
	_OldAccount = nullptr;
	_OldFund = nullptr;
}


VtOrderConfigManager::~VtOrderConfigManager()
{
}

void VtOrderConfigManager::Account(account_p val)
{
	_OldAccount = _Account;
	_Account = val;
	//_OrderMgr = mainApp.TotalOrderMgr().FindAddAccountOrderManger(_Account->AccountNo);
}

void VtOrderConfigManager::Symbol(VtSymbol* val)
{
	_Symbol = val;
	if (_OrderMgr)
	{
		//_PrdtOrderMgr = _OrderMgr->FindAddProductOrderManager(_Symbol->FullCode);
		//_PrdtOrderMgr->OrderMgr(_OrderMgr);
		//_PrdtOrderMgr->Account(_Account);
		int i = 0;
	}
}

void VtOrderConfigManager::RegisterRealtimeSymbol()
{
	if (!_Symbol || !_HdOrderWnd)
		return;

	//VtOrderDialogManager* oldDlgMgr = VtOrderDialogManager::GetInstance();
	//oldDlgMgr->AddSymbolWindow(_Symbol->ShortCode, _HdOrderWnd);

	//mainApp.RealtimeRegisterMgr().RegisterProduct(_Symbol->ShortCode, 0);
}

void VtOrderConfigManager::UnregisterRealtimeSymbol()
{
	if (!_Symbol || !_HdOrderWnd)
		return;
	//mainApp.RealtimeRegisterMgr().UnregisterProduct(_Symbol->ShortCode, 0);

	//VtOrderDialogManager* oldDlgMgr = VtOrderDialogManager::GetInstance();
	//oldDlgMgr->RemoveSymbolWindow(_Symbol->ShortCode, _HdOrderWnd);
}

void VtOrderConfigManager::Fund(fund_p val)
{
	if (!val)
		return;

	_OldFund = _Fund;
	_Fund = val;

	//_OrderMgr = mainApp.TotalOrderMgr().FindAddFundOrderManager(_Fund->Name);
	//((VtFundOrderManager*)_OrderMgr)->UpdateOrderManagers();
}

void VtOrderConfigManager::Reset()
{
	_Account = nullptr;
	_Symbol = nullptr;
	_Quote = nullptr;
	_Master = nullptr;
	_Hoga = nullptr;
	_Position = nullptr;
}

void VtOrderConfigManager::SetDefaultSymbol()
{
	//_Symbol = mainApp.SymbolMgr().MrktMgr().GetDefaultSymbol();
}

void VtOrderConfigManager::SetDefaultAccount()
{
	int i = 0;
	/*
	for (auto it = mainApp.AcntMgr().AccountMap.begin(); it != mainApp.AcntMgr().AccountMap.end(); ++it) {
		VtAccount* acnt = it->second;
		if (i == 0) {
			_Account = acnt;
			_OldAccount = _Account;
			break;
		}
	}
	*/
}

