#include "stdafx.h"
#include "SmAccount.h"
#include "../Util/IdGenerator.h"
#include "../Global/SmTotalManager.h"
#include "SmAccountManager.h"
#include "../Position/Position.h"
namespace DarkHorse {

	void SmAccount::AddSubAccount(const std::shared_ptr<SmAccount>& subAccount)
	{
		// Check if the account number is unique before adding
		if (!IsAccountNumberUnique(subAccount->No()))
		{
			// Handle the duplication case (e.g., throw an exception or return an error status)
			AfxMessageBox("Account number already exists.");
		}

		sub_accounts_[subAccount->No()] = subAccount;

		mainApp.AcntMgr()->AddAccount(subAccount);
	}

	void SmAccount::make_default_sub_account()
	{
		if (!sub_accounts_.empty()) return;
		for (int i = 0; i < 10; i++) {
			std::shared_ptr< SmAccount> sub_account = std::make_shared<SmAccount>();
			sub_account->Name(_Name);
			std::string sub_account_no = _No + "_" + std::to_string(i+1);
			sub_account->No(sub_account_no);
			sub_account->Type(_Type);
			sub_account->Pwd(_pwd);
			sub_account->is_subaccount(true);
			sub_account->parent_id(_id);
			AddSubAccount(sub_account);
		}
	}

	SmAccount::SmAccount()
		: _id(IdGenerator::get_id())
	{
		position_ = std::make_shared<Position>();
	}
	SmAccount::~SmAccount()
	{

	}
}


