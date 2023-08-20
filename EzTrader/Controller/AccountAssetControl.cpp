#include "stdafx.h"
#include "AccountAssetControl.h"
#include "../Util/IdGenerator.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Global/SmTotalManager.h"
#include "../Fund/SmFund.h"
namespace DarkHorse {

	AccountAssetControl::AccountAssetControl()
	: id_(IdGenerator::get_id())
{

}

	AccountAssetControl::~AccountAssetControl()
{

}

	void AccountAssetControl::load_position_from_parent_account(const std::string& account_no)
	{

	}

	void AccountAssetControl::load_position_from_fund(const std::string& fund_name)
	{

	}

void AccountAssetControl::load_position_from_account(const std::string& account_no)
{
	auto account = mainApp.AcntMgr()->FindAccount(account_no);
	if (!account) return;

	asset_.entrust_total = account->Asset.EntrustTotal; // 예탁금총액
	asset_.balance = account->Asset.EntrustTotal; // 예탁금잔액
	asset_.outstanding_deposit = account->Asset.OutstandingDeposit; // 미결제증거금
	asset_.order_deposit = account->Asset.OrderDeposit; // 주문증거금
	asset_.entrust_deposit = account->Asset.EntrustDeposit; // 위탁증거금
	asset_.maintenance_margin = account->Asset.MaintenanceMargin; // 유지증거금
	asset_.trade_profit_loss = account->Asset.TradeProfitLoss; // 청산손익
	asset_.trade_fee = account->Asset.Fee; // 선물옵션수수료
	asset_.open_profit_loss = account->Asset.OpenProfitLoss; // 평가손익
	asset_.open_trust_toal = account->Asset.OpenTrustTotal; // 평가예탁총액
	asset_.additional_margin = account->Asset.AdditionalMargin; // 추가증거금
	asset_.order_margin = account->Asset.OrderMargin; // 주문가능금액
	asset_.currency = account->Asset.Currency;
}

void AccountAssetControl::set_account(std::shared_ptr<SmAccount> account)
{
	if (!account) return;
	account_ = account;
	if (account_->is_subaccount()) {
		if (!account_->parent_account().lock()) return;
		load_position_from_account(account_->parent_account().lock()->No());
	}
	else
		load_position_from_parent_account(account_->No());
}

void AccountAssetControl::set_fund(std::shared_ptr<SmFund> fund)
{
	if (!fund) return;
	fund_ = fund;
	load_position_from_fund(fund_->Name());
}

}