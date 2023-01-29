#pragma once
#include <string>
#include "../SmOrderConst.h"
namespace DarkHorse {
	struct OrderRequest {
		int request_id{ 0 };
		// �ݴ� ������ ��ž ��û ���̵�. ��ž�� ������ �� �ݵ�� �̰͵� ���־� �Ѵ�.
		int counter_request_id{ 0 };
		std::string password{ "" };
		std::string symbol_code{ "" };
		std::string account_no{ "" };
		int order_price{ 0 };
		int order_amount{ 0 };
		std::string original_order_no{ "" };
		// for loss and profit cut value
		// order with slippage for loss and profit cut 
		int cut_slip{ 2 };
		int loss_cut_tick{ 2 };
		int profit_cut_tick{ 2 };
		// 0 : none, 1 : profit cut, 2 : loss cut , 3 : both(profit & loss) cut
		SmCutMode cut_mode{ SmCutMode::None };
		SmPriceType cut_price_type{ SmPriceType::Price };
		SmPositionType position_type{ SmPositionType::None };
		SmOrderType order_type{ SmOrderType::None };
		SmPriceType price_type{ SmPriceType::None };
		SmFilledCondition fill_condition{ SmFilledCondition::Fas };
	};
}