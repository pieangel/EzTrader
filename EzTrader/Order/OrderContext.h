#pragma once
#include "SmOrderConst.h"
#include <string>
namespace DarkHorse {
struct OrderContext {
	std::string parent_account_no;
	std::string fund_name;
	// Account Type. 1 : Sub Account, 2 : Main Account, 3 : Fund.
	AccountType account_type = AccountType::None;
	// The id of the window that the order came from.
	int order_control_id;
	int close;
	int virtual_filled_price;
};
}