#pragma once
#include <vector>
#include <map>
#include <string>
#include <memory>
#include "SmOutSystem.h"

namespace DarkHorse {
	class SmOutSystem;
	class SmOutSystemManager
	{
	public:
		SmOutSystemManager();
		~SmOutSystemManager();
		std::shared_ptr<SmOutSystem> create_out_system(
			const std::string& system_name,
			const std::string& desc,
			OrderType order_type,
			std::shared_ptr<SmAccount> account,
			std::shared_ptr<SmFund> fund,
			std::shared_ptr<SmSymbol> symbol
		);
		void remove_out_system(const std::string& system_name);
		void remove_out_system_by_id(const int& system_id);
	private:
		std::vector<std::shared_ptr<SmOutSystem>> out_system_vec_;
		// key : system name, value : SmOutSystem object map.
		std::map<std::string, SmOutSystemMap> out_system_map_;
	};
}

