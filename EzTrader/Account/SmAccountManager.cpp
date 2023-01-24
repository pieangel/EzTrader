#include "stdafx.h"
#include "SmAccountManager.h"
#include "SmAccount.h"
using namespace DarkHorse;

int SmAccountManager::_Id = 0;

SmAccountManager::SmAccountManager()
{
	//AddTestAccounts();
}

SmAccountManager::~SmAccountManager()
{

}

void SmAccountManager::AddAccount(std::shared_ptr<SmAccount> account)
{
	if (!account) return;
	auto old_account = FindAccount(account->No());
	if (old_account) return;

	// Set the id for the account
	account->Id(GetId());
	_AccountMap[account->No()] = account;
	_AccountIdMap[account->Id()] = account;
}

void DarkHorse::SmAccountManager::GetUnusedAccountForFund(std::vector<std::shared_ptr<SmAccount>>& unused_acnt_vector)
{
	for (auto it = _AccountMap.begin(); it != _AccountMap.end(); it++) {
		if (!it->second->UsedForFund())
			unused_acnt_vector.push_back(it->second);
	}
}

std::shared_ptr<DarkHorse::SmAccount> DarkHorse::SmAccountManager::FindAccount(const std::string& account_no)
{
	auto found = _AccountMap.find(account_no);
	if (found != _AccountMap.end())
		return found->second;
	else
		return nullptr;
}

std::shared_ptr<DarkHorse::SmAccount> DarkHorse::SmAccountManager::FindAccountById(const int& account_id)
{
	auto found = _AccountIdMap.find(account_id);
	if (found != _AccountIdMap.end())
		return found->second;
	else
		return nullptr;
}

void DarkHorse::SmAccountManager::AddTestAccounts()
{
	for (int i = 0; i < 10; i++) {
		std::shared_ptr<SmAccount> account = std::make_shared<SmAccount>();
		std::string account_no("00370");
		account_no.append(std::to_string(i));
		account->No(account_no);
		std::string user("user");
		user.append(std::to_string(i + 1));
		account->Name(user);
		account->Type("1");
		AddAccount(account);
	}
}

void DarkHorse::SmAccountManager::SetPassword(const std::string& account_no, const std::string& pwd)
{
	auto found = _AccountMap.find(account_no);
	if (found == _AccountMap.end()) return;

	found->second->Pwd(pwd);
}
