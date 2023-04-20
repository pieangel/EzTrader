#pragma once
#include "../ViewModel/VmPosition.h"
#include "../Util/IdGenerator.h"
#include <memory>
#include <functional>
namespace DarkHorse {
	struct Position;
	class SymbolPositionControl
	{
	public:
		SymbolPositionControl();
		~SymbolPositionControl();
		void update_position(std::shared_ptr<Position> quote);
		const VmPosition& get_position()
		{
			return position_;
		}
		const int get_id()
		{
			return id_;
		}
		void set_symbol_id(const int symbol_id) {
			symbol_id_ = symbol_id;
		}
		void set_account_id(const int account_id) {
			symbol_id_ = account_id;
		}
		void set_event_handler(std::function<void()> event_handler) {
			event_handler_ = event_handler;
		}
	private:
		int id_{ 0 };
		int symbol_id_{ 0 };
		int account_id_{ 0 };
		VmPosition position_;
		void subscribe_position_control();
		std::function<void()> event_handler_;
	};
}
