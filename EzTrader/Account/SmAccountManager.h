#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include "../Json/json.hpp"
namespace DarkHorse {
	class SmAccount;
	// ���� ������
	class SmAccountManager
	{
	public:
		// ������
		SmAccountManager();
		// �Ҹ���
		~SmAccountManager();
		// Static Members
		static int _Id;
		static int GetId() { return _Id++; }
		/// <summary>
		/// ���¸� �߰��Ѵ�.
		/// </summary>
		/// <param name="account">���� ��ü</param>
		void AddAccount(std::shared_ptr<SmAccount> account);
		/// <summary>
		/// ���� ����� �����ɴϴ�.
		/// </summary>
		/// <returns></returns>
		const std::unordered_map<std::string, std::shared_ptr<SmAccount>>& GetAccountMap() {
			return _AccountMap;
		}
		void GetUnusedAccountForFund(std::vector<std::shared_ptr<SmAccount>>& unused_acnt_vector);
		std::shared_ptr<SmAccount> FindAccount(const std::string& account_no);
		std::shared_ptr<SmAccount> FindAccountById(const int& account_id);
		void AddTestAccounts();
		void SetPassword(const std::string& account_no, const std::string& pwd);
		void on_account_asset(nlohmann::json&& arg);
	private:
		
		/// <summary>
		/// Account map
		/// Key : Account No, Value : Account Object Pointer.
		/// </summary>
		std::unordered_map<std::string, std::shared_ptr<SmAccount>> _AccountMap;
		// key : account id, value : account object
		std::map<int, std::shared_ptr<SmAccount>> _AccountIdMap;
	};
}

