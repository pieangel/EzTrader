#include "stdafx.h"
#include "AssetControl.h"
#include "../Util/IdGenerator.h"
#include "../Account/SmAccount.h"
#include "../Account/SmAccountManager.h"
#include "../Global/SmTotalManager.h"
namespace DarkHorse {

AssetControl::AssetControl()
	: id_(IdGenerator::get_id())
{

}

AssetControl::~AssetControl()
{

}

void AssetControl::load_position_from_account(const std::string& account_no)
{
	auto account = mainApp.AcntMgr()->FindAccount(account_no);
	if (!account) return;

	asset_.entrust_total = account->Asset.EntrustTotal; // ��Ź���Ѿ�
	asset_.balance = account->Asset.Balance; // ��Ź���ܾ�
	asset_.outstanding_deposit = account->Asset.OutstandingDeposit; // �̰������ű�
	asset_.order_deposit = account->Asset.OrderDeposit; // �ֹ����ű�
	asset_.entrust_deposit = account->Asset.EntrustDeposit; // ��Ź���ű�
	asset_.maintenance_margin = account->Asset.MaintenanceMargin; // �������ű�
	asset_.trade_profit_loss = account->Asset.TradeProfitLoss; // û�����
	asset_.fee = account->Asset.Fee; // �����ɼǼ�����
	asset_.open_profit_loss = account->Asset.OpenProfitLoss; // �򰡼���
	asset_.open_trust_toal = account->Asset.OpenTrustTotal; // �򰡿�Ź�Ѿ�
	asset_.additional_margin = account->Asset.AdditionalMargin; // �߰����ű�
	asset_.order_margin = account->Asset.OrderMargin; // �ֹ����ɱݾ�
	asset_.currency = account->Asset.Currency;
}

}