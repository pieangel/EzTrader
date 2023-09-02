#include "stdafx.h"
#include "SmOutSystemManager.h"
#include "../Util/IdGenerator.h"
#include "SmOutSignalDef.h"
namespace DarkHorse {
	SmOutSystemManager::SmOutSystemManager()
	{
		make_out_system_signal_map();
	}


	SmOutSystemManager::~SmOutSystemManager()
	{
	}

	std::shared_ptr<SmOutSystem> SmOutSystemManager::create_out_system(
		const std::string& signal_name,
		const int seung_su,
		OrderType order_type,
		std::shared_ptr<SmAccount> account,
		std::shared_ptr<SmFund> fund,
		std::shared_ptr<SmSymbol> symbol
	)
	{
		std::shared_ptr<SmOutSystem> out_system = std::make_shared<SmOutSystem>(signal_name);
		out_system->seung_su(seung_su);
		out_system->order_type(order_type);
		out_system->account(account);
		out_system->fund(fund);
		out_system->symbol(symbol);
		out_system_vec_.push_back(out_system);

		auto it = out_system_map_.find(signal_name);
		if (it != out_system_map_.end())
		{
			SmOutSystemMap& system_map = it->second;
			system_map.insert(std::make_pair(out_system->id(), out_system));
		}
		else{
			SmOutSystemMap system_map;
			system_map.insert(std::make_pair(out_system->id(), out_system));
			out_system_map_.insert(std::make_pair(signal_name, system_map));

		}

		return out_system;
	}

	void SmOutSystemManager::remove_out_system(const std::string& signal_name)
	{
		auto found = out_system_map_.find(signal_name);
		out_system_map_.erase(found);
		std::erase_if(out_system_vec_, [&](const std::shared_ptr<SmOutSystem>& out_system) {
			return out_system->name() == signal_name; });
	}

	void SmOutSystemManager::remove_out_system_by_id(const int& system_id)
	{
		std::erase_if(out_system_vec_, [&](const std::shared_ptr<SmOutSystem>& out_system) {
			return out_system->id() == system_id; });
	}

	void SmOutSystemManager::make_out_system_signal_map()
	{
		for (int i = 0; i < 100; i++) {
			auto out_system_signal = std::make_shared<SmOutSignalDef>(IdGenerator::get_id());
			out_system_signal->name = "T" + std::to_string(i + 1);
			out_system_signal->desc = "T" + std::to_string(i + 1);
			out_system_signal_vec_.push_back(out_system_signal);
		}
	}

}
