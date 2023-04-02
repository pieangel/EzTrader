#pragma once
#include <memory>
namespace DarkHorse {
	class SmSymbol;
	struct VmFuture {
		std::shared_ptr<SmSymbol> symbol_p;
		int symbol_id{ 0 };
		int close{ 0 };
		int expected{ 0 };
		int position{ 0 };
		bool ordered{ false };
		int decimal{ 0 };
	};
}