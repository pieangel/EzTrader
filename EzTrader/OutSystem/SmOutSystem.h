#pragma once
#include <string>
#include <memory>
#include <map>
#include "../Order/SmOrderConst.h"
namespace DarkHorse {
	class SmAccount;
	class SmSymbol;
	class SmFund;
	class SmOutSystem
	{
	public:
		explicit SmOutSystem(const std::string& name);
		~SmOutSystem();
		std::string name() const { return name_; }
		void name(std::string val) { name_ = val; }
		std::shared_ptr<SmAccount> account() const { return account_; }
		void account(std::shared_ptr<SmAccount> val) { account_ = val; }
		int id() const { return id_; }
		void id(int val) { id_ = val; }
		DarkHorse::OrderType order_type() const { return order_type_; }
		void order_type(DarkHorse::OrderType val) { order_type_ = val; }

		std::string desc() const { return desc_; }
		void desc(std::string val) { desc_ = val; }
		std::shared_ptr<DarkHorse::SmSymbol> symbol() const { return symbol_; }
		void symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { symbol_ = val; }
		std::shared_ptr<DarkHorse::SmFund> fund() const { return fund_; }
		void fund(std::shared_ptr<DarkHorse::SmFund> val) { fund_ = val; }

	private:
		OrderType order_type_{ OrderType::None };
		int id_{ 0 };
		// Signal Name. "T1", "T2" for yes.
		std::string name_;
		std::shared_ptr<SmAccount> account_{ nullptr };
		std::shared_ptr<SmSymbol> symbol_{ nullptr };
		std::shared_ptr<SmFund> fund_{ nullptr };
		std::string desc_;
	};
	// key : system id, value : SmOUtSystem object.
	typedef std::map<int, std::shared_ptr<SmOutSystem>> SmOutSystemMap;
}

