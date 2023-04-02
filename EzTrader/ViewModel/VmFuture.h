#pragma once
#include <memory>
namespace DarkHorse {
	class SmSymbol;
	struct VmOption {
		std::shared_ptr<SmSymbol> call_symbol_p;
		std::shared_ptr<SmSymbol> put_symbol_p;
		int call_symbol_id{ 0 };
		int put_symbol_id{ 0 };
		int close{ 0 };
		int expected{ 0 };
		int position{ 0 };
		bool ordered{ false };
	};
}