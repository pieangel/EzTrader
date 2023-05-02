#pragma once

#pragma once
#include <map>
#include <string>
#include <memory>
#include <functional>
#include "../ViewModel/VmAsset.h"
namespace DarkHorse {
	class SmAccount;
	class AssetControl
	{
	public:
		AssetControl();
		~AssetControl();
		void load_position_from_account(const std::string& account_no);
		void set_event_handler(std::function<void()> event_handler) {
			event_handler_ = event_handler;
		}
	private:
		std::function<void()> event_handler_;
		int id_{ 0 };
		VmAsset asset_;
	};
}
