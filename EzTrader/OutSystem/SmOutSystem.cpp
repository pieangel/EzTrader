#include "stdafx.h"
#include "SmOutSystem.h"
#include "../Util/IdGenerator.h"
#include "../Global/SmTotalManager.h"
#include "../Order/OrderRequest/OrderRequest.h"
#include "../Order/OrderRequest/OrderRequestManager.h"
#include "../Fund/SmFund.h"
#include "../Account/SmAccount.h"
#include "../Symbol/SmSymbol.h"
#include "../Log/MyLogger.h"
#include "../Controller/SymbolPositionControl.h"
#include "../Controller/QuoteControl.h"


namespace DarkHorse {
	SmOutSystem::SmOutSystem(const std::string& name)
		:name_(name), id_(IdGenerator::get_id())
	{
		position_control_ = std::make_shared<SymbolPositionControl>();
		position_control_->set_out_system_id(id_);
		quote_control_ = std::make_shared<QuoteControl>();
	}

	SmOutSystem::~SmOutSystem()
	{
	}

	void SmOutSystem::account(std::shared_ptr<SmAccount> val)
	{
		if (!val) return;
		account_ = val;
		position_control_->set_account(account_);
	}

	void SmOutSystem::fund(std::shared_ptr<DarkHorse::SmFund> val)
	{
		if (!val) return;
		fund_ = val;
		position_control_->set_fund(fund_);
	}

	void SmOutSystem::put_order(const std::string& signal_name, int order_kind, int order_amount)
	{
		if (order_type_ == OrderType::SubAccount ||
			order_type_ == OrderType::MainAccount) {
			if (!account_) return;
			put_order_each(account_, signal_name, order_kind, order_amount);
		}
		else if (order_type_ == OrderType::Fund) {
			if (!fund_) return;
			for (auto& account : fund_->GetAccountVector()) {
				put_order_each(account, signal_name, order_kind, order_amount);
			}
		}
		else return;
	}

	void SmOutSystem::put_order_each(std::shared_ptr<SmAccount> account, const std::string& signal_name, int order_kind, int order_amount)
	{
		if (!account || !symbol_) return;

		std::shared_ptr<OrderRequest> order_req = std::make_shared<OrderRequest>();
		order_req->request_id = OrderRequestManager::get_id();
		order_req->account_no = account->No();
		order_req->password = account->Pwd();
		//order_req->order_price = order_price;
		order_req->order_amount = order_amount * seung_su_;
		order_req->symbol_code = symbol_->SymbolCode();

		// 		order_req->order_type = order_type;
		// 		order_req->price_type = price_type;
		// 		order_req->fill_condition = fill_condition;
		// 활성화된 시스템에 대하여 주문을 넣는다.
		switch (order_kind)
		{
		case 1: // Buy
			//msg.Format(_T("신호이름 : %s, 신호 종류 : %d, 주문 : 매수\n"), sigName.c_str(), orderKind);
			//TRACE(msg);
			LOGINFO(CMyLogger::getInstance(), _T("신호이름 : %s, 신호 종류 : %d, 주문 : 매수"), signal_name.c_str(), order_kind);
			order_req->position_type = SmPositionType::Buy;
			break;
		case 2: { // ExitLong -> Sell
			// 매도 진입 주문으로 처리
			LOGINFO(CMyLogger::getInstance(), _T("신호이름 : %s, 신호 종류 : %d, 주문 : 매도"), signal_name.c_str(), order_kind);
			order_req->position_type = SmPositionType::Sell;
		}
			  break;
		case 3: // Sell
			LOGINFO(CMyLogger::getInstance(), _T("신호이름 : %s, 신호 종류 : %d, 주문 : 매도"), signal_name.c_str(), order_kind);
			order_req->position_type = SmPositionType::Sell;
			break;
		case 4: { // ExitShort -> Buy
			LOGINFO(CMyLogger::getInstance(), _T("신호이름 : %s, 신호 종류 : %d, 주문 : 매수"), signal_name.c_str(), order_kind);
			order_req->position_type = SmPositionType::Buy;
		}
			  break;
		default:
			break;
		}
		mainApp.order_request_manager()->add_order_request(order_req);
	}

}
