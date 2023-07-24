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

	SmAccount::SmAccount()
		: _id(IdGenerator::get_id())
	{
		position_ = std::make_shared<Position>();
	}
	SmAccount::~SmAccount()
	{

	}
}


