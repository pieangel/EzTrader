#pragma once
#include "../ViewModel/VmPosition.h"
#include "../Util/IdGenerator.h"
#include <memory>
#include <functional>
namespace DarkHorse {
	struct Position;
	struct SmQuote;
	class SymbolPositionControl
	{
	public:
		SymbolPositionControl();
		~SymbolPositionControl();
		void update_profit_loss(std::shared_ptr<SmQuote> quote);
		void update_position(std::shared_ptr<Position> position);
		const VmPosition& get_position()
		{
			return position_;
		}
		const int get_id()
		{
			return id_;
		}
		void set_symbol_id(const int symbol_id);
		void set_account_id(const int account_id);
		void set_event_handler(std::function<void()> event_handler) {
			event_handler_ = event_handler;
		}
	private:
		void reset_position();
		int symbol_seung_su_{ 1 };
		int symbol_decimal_{ 1 };
		int id_{ 0 };
		int symbol_id_{ 0 };
		int account_id_{ 0 };
		VmPosition position_;
		void subscribe_position_control();
		std::function<void()> event_handler_;
	};
}
