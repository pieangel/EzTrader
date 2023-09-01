#include "stdafx.h"
#include "SmOutSystemManager.h"

namespace DarkHorse {
	SmOutSystemManager::SmOutSystemManager()
	{
	}


	SmOutSystemManager::~SmOutSystemManager()
	{
	}

	std::shared_ptr<SmOutSystem> SmOutSystemManager::create_out_system(
		const std::string& system_name,
		const std::string& desc,
		OrderType order_type,
		std::shared_ptr<SmAccount> account,
		std::shared_ptr<SmFund> fund,
		std::shared_ptr<SmSymbol> symbol
	)
	{
		if (system_name.empty()) return nullptr;
		if (order_type == OrderType::None) return nullptr;
		if (order_type == OrderType::Fund && !account) return nullptr;
		if (order_type == OrderType::Fund && !fund) return nullptr;
		if (!symbol) return nullptr;

		std::shared_ptr<SmOutSystem> out_system = std::make_shared<SmOutSystem>(system_name);
		out_system->desc(desc);
		out_system->order_type(order_type);
		out_system->account(account);
		out_system->fund(fund);
		out_system->symbol(symbol);
		out_system_vec_.push_back(out_system);

		auto it = out_system_map_.find(system_name);
		if (it != out_system_map_.end())
		{
			SmOutSystemMap& system_map = it->second;
			system_map.insert(std::make_pair(out_system->id(), out_system));
		}
		else{
			SmOutSystemMap system_map;
			system_map.insert(std::make_pair(out_system->id(), out_system));
			out_system_map_.insert(std::make_pair(system_name, system_map));

		}

		return out_system;
	}

	void SmOutSystemManager::remove_out_system(const std::string& system_name)
	{
		auto found = out_system_map_.find(system_name);
		out_system_map_.erase(found);
		std::erase_if(out_system_vec_, [&](const std::shared_ptr<SmOutSystem>& out_system) {
			return out_system->name() == system_name; });
	}

	void SmOutSystemManager::remove_out_system_by_id(const int& system_id)
	{
		std::erase_if(out_system_vec_, [&](const std::shared_ptr<SmOutSystem>& out_system) {
			return out_system->id() == system_id; });
	}

}
