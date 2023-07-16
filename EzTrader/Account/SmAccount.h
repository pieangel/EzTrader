#pragma once
#include <string>
#include <map>
#include <memory>
#include <stdexcept>
#include "SmAccountAsset.h"
namespace DarkHorse {
	/// <summary>
	/// 계좌 클래스
	/// 계좌 정보를 가지고 있습니다.
	/// </summary>
	class SmAccount
	{
	private:
		/// <summary>
		/// 고유 아이디
		/// </summary>
		int _id{ -1 };
		int parent_id_{ -1 };
		/// <summary>
		/// 계좌 이름
		/// </summary>
		std::string _Name;
		/// <summary>
		/// 계좌 번호
		/// </summary>
		std::string _No;
		/// <summary>
		/// 계좌 타입
		/// 1 : abroad, 9 : domestic
		/// </summary>
		std::string _Type;
		/// <summary>
		/// 비밀 번호
		/// </summary>
		std::string _pwd;
		
		// 0 : not confirmed, 1 : confirmed.
		int _Confirm{ 0 };
		// used for the fund
		bool _UsedForFund = false;
		int  _SeungSu = 1;
		double _Ratio = 0;

		bool is_sub_account_ = false;
		// key : sub account number, value : sub account
		std::map<std::string, std::shared_ptr<SmAccount>> sub_accounts_;
		// Function to check if a given account number already exists in the sub-accounts
		bool IsAccountNumberUnique(const std::string& accountNumber) const
		{
			return sub_accounts_.find(accountNumber) == sub_accounts_.end();
		}
	public:
		int parent_id() const { return parent_id_; }
		void parent_id(int val) { parent_id_ = val; }
		// Function to get the list of sub-accounts
		const std::map<std::string, std::shared_ptr<SmAccount>>& SubAccounts() const { return sub_accounts_; }
		// Function to add a new sub-account to the list
		void AddSubAccount(const std::shared_ptr<SmAccount>& subAccount);
		
		void make_default_sub_account() {
			if (!sub_accounts_.empty()) return;

			std::shared_ptr< SmAccount> sub_account = std::make_shared<SmAccount>();
			sub_account->Name(_Name);	
			std::string sub_account_no = _No + "_" + "1";
			sub_account->No(sub_account_no);
			sub_account->Type(_Type);
			sub_account->Pwd(_pwd);
			sub_account->is_subaccount(true);
			sub_account->parent_id(_id);
			AddSubAccount(sub_account);
		}
		SmAccount();
		~SmAccount();
		int Confirm() const { return _Confirm; }
		void Confirm(int val) { _Confirm = val; }
		std::string Name() const { return _Name; }
		void Name(std::string val) { _Name = val; }
		std::string Type() const { return _Type; }
		void Type(std::string val) { _Type = val; }
		std::string No() const { return _No; }
		void No(std::string val) { _No = val; }
		std::string Pwd() const { return _pwd; }
		void Pwd(std::string val) { _pwd = val; }

		SmAccountAsset Asset;
		int id() const { return _id; }
		bool UsedForFund() const { return _UsedForFund; }
		void UsedForFund(bool val) { _UsedForFund = val; }
		int SeungSu() const { return _SeungSu; }
		void SeungSu(int val) { _SeungSu = val; }
		double Ratio() const { return _Ratio; }
		void Ratio(double val) { _Ratio = val; }
		bool is_subaccount() const { return is_sub_account_; }
		void is_subaccount(bool val) { is_sub_account_ = val; }
	};
}

