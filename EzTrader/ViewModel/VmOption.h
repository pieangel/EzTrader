#pragma once
#include <memory>
#include <string>
namespace DarkHorse {
	class SmSymbol;
	struct VmOption {
		std::shared_ptr<SmSymbol> symbol_p{ nullptr };
		int symbol_id{ 0 };
		int close{ 0 };
		int expected{ 0 };
		int position{ 0 };
		int atm{ 0 };
		bool ordered{ false };
		std::string strike;
		int decimal;
	};
}