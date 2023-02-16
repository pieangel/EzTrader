#pragma once
#include <string>
#include <memory>
#include "SmOrderConst.h"
namespace DarkHorse {
struct Order {
	// 주문번호
	std::string order_no;
	// 계좌 번호
	std::string account_no;
	// 종목 코드
	std::string symbol_code;
	// 1 : 매수, 2 : 매도
	SmPositionType position_type = SmPositionType::None;
	// 주문 가격
	int order_price{ 0 };
	// 주문 수량
	int order_amount{ 0 };
	// 주문 시간
	std::string order_time;
	// 1 : 신규, 2 : 정정, 3 : 취소
	SmOrderType order_type = SmOrderType::None;
	// 원주문 번호
	std::string original_order_no;
	// 최초 원주문 번호
	std::string first_order_no;
	// 체결 주문 가격
	int filled_price{ 0 };
	// 체결량
	int filled_count{ 0 };
	// 미결제량 - 최초에는 체결량과 같다.
	int unsettled_count{ 0 };
	// 가격 유형. 1 : 시장가, 2 : 지정가, 3 : STOP, 4 : STOP 지정가
	SmPriceType price_type = SmPriceType::None;
	// 체결 시간
	std::string filled_time;
	// 주문 상태
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