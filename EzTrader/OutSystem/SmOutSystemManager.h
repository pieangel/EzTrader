#pragma once
#include <vector>
#include <map>
#include <string>
#include <memory>
#include "SmOutSystem.h"

namespace DarkHorse {
	class SmOutSystem;
	class SmOutSignalDef;
	class SmOutSystemManager
	{
	public:
		SmOutSystemManager();
		~SmOutSystemManager();
		std::shared_ptr<SmOutSystem> create_out_system(
			const std::string& signal_name,
			const int seung_su,
			OrderType order_type,
			std::shared_ptr<SmAccount> account,
			std::shared_ptr<SmFund> fund,
			std::shared_ptr<SmSymbol> symbol
		);
		void remove_out_system(const std::string& signal_name);
		void remove_out_system_by_id(const int& system_id);
		void make_out_system_signal_map();
		const std::map<std::string, std::shared_ptr<SmOutSignalDef>>& get_out_system_signal_map() const { return out_system_signal_map_; }
	private:
		std::vector<std::shared_ptr<SmOutSystem>> out_system_vec_;
		// key : system name, value : SmOutSystem object.
		std::map<std::string, SmOutSystemMap> out_system_map_;
		// key : signal name, value : SmOutSignalDef object.
		std::map<std::string, std::shared_ptr<SmOutSignalDef>> out_system_signal_map_;
	};
}

