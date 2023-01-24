#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include "../Xml/pugixml.hpp"
namespace DarkHorse {
	class SmAccount;
	class SmFund
	{
	public:
		SmFund() {};
		 SmFund(const int& id, const std::string& name) : _id(id), _Name(name) {};
		~SmFund() {};
		std::string Name() const { return _Name; }
		void Name(std::string val) { _Name = val; }
		int Id() const { return _id; }
		void Id(int val) { _id = val; }
		std::shared_ptr<SmAccount> FindAccount(const std::string& account_no);
		void AddAccount(const std::shared_ptr<SmAccount>& account);
		void RemoveAccount(const std::string& account_no);
		void RemoveAllAccounts();
		const std::vector<std::shared_ptr<SmAccount>>& GetAccountVector() {
			return _AccountVector;
		}
		size_t GetAccountCount() {
			return _AccountVector.size();
		}
		int GetRatioPercent(const std::string account_no);
		int GetTotalSeungSu();

		void MoveUp(const int& target_index);
		void MoveDown(const int& target_index);
		int GetPositionCount(const std::string& symbol_code);
		int GetFilledOrderCount(const std::string& symbol_code);
		void SaveToXml(pugi::xml_node& node);
		void LoadFromXml(pugi::xml_node& node);
	private:
		// key : account no, value : account object
		std::vector<std::shared_ptr<SmAccount>> _AccountVector;
		std::string _Name;
		int _id{ -1 };
	};
}

