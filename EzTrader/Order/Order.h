#pragma once
#include <string>
#include <memory>
#include "SmOrderConst.h"
namespace DarkHorse {
struct Order {
	// �ֹ���ȣ
	std::string order_no;
	// ���� ��ȣ
	std::string account_no;
	// ���� �ڵ�
	std::string symbol_code;
	// 1 : �ż�, 2 : �ŵ�
	SmPositionType position_type = SmPositionType::None;
	// �ֹ� ����
	int order_price{ 0 };
	// �ֹ� ����
	int order_amount{ 0 };
	// �ֹ� �ð�
	std::string order_time;
	// 1 : �ű�, 2 : ����, 3 : ���
	SmOrderType order_type = SmOrderType::None;
	// ���ֹ� ��ȣ
	std::string original_order_no;
	// ���� ���ֹ� ��ȣ
	std::string first_order_no;
	// ü�� �ֹ� ����
	int filled_price{ 0 };
	// ü�ᷮ
	int filled_count{ 0 };
	// �̰����� - ���ʿ��� ü�ᷮ�� ����.
	int unsettled_count{ 0 };
	// ���� ����. 1 : ���尡, 2 : ������, 3 : STOP, 4 : STOP ������
	SmPriceType price_type = SmPriceType::None;
	// ü�� �ð�
	std::string filled_time;
	// �ֹ� ����
	SmOrderState order_state = SmOrderState::None;
	
	int remain_count{ 0 };
	int modified_count{ 0 };
	int cancelled_count{ 0 };

	// 1 : new, 2 : modified or cancelled
	int order_sequence{ 1 };
	// Order Request Id.
	int order_request_id = 0;
};
}