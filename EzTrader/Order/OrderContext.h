#pragma once
#include "SmOrderConst.h"
#include <string>
namespace DarkHorse {
struct OrderContext {
	std::string parent_account_no;
	std::string fund_name;
	// Account Type. 1 : Sub Account, 2 : Main Account, 3 : Fund.
	AccountType account_type = AccountType::None;
	/// <summary>
		/// Order Source ID
		/// �ֹ��� ���۵� ���� ID. ������ �ֹ�â�� ���̵� ����Ű��, �ý����� ��� �ý����� ���̵�,
		/// �ٸ� �ҽ��� ��� �� �ҽ��� ���̵� ���´�. ���̵�� ��ġ�� ������
		/// IdGenerator���� �����Ͽ� ��ġ�� �ʰ� �����Ѵ�. 
		/// �� ���̵�� ���α׷� �ʱ⿡ �����Ǹ� ���α׷��� �����ϸ� ��� ������� 
		/// ���۵� �� �ٽ� �����Ǿ� �� �ҽ��� ��Ī�� �ȴ�. 
		/// </summary>
	int order_control_id{ 0 };
	int close;
	int virtual_filled_price;
};
}