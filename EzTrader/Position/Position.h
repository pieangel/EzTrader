#pragma once
#include <string>
#include "../Order/SmOrderConst.h"
namespace DarkHorse {
struct Position
{
	int id{ 0 };
	int fund_id{ 0 };
	int symbol_id{ 0 };
	int account_id{ 0 };
	OrderType order_source_type = OrderType::None;
	std::string fund_name;
	// �ɺ� �ڵ�
	std::string symbol_code;
	// ���� ��ȣ
	std::string account_no;
	// �������� �ܰ�
	// �������� �ܰ�� �ż��� �϶��� ���, �ŵ��� ���� ������ ǥ���Ѵ�.
	// �������� �ܰ� 0�̸� �������� ���� ���̴�.
	int open_quantity{ 0 };
	int pre_day_open_quantity{ 0 };
	double trade_profit_loss{ 0.0f }; //	�Ÿż���
	// �Ҽ��� �ݿ� ���� ����. 
	double average_price{ 0.0f };	// ��հ�*/
	double open_profit_loss{ 0.0f };		/*�򰡼���*/
	double pure_trade_profit_loss{ 0.0f }; // û�� ������
	double trade_fee{ 0.0f }; // û�� ������
};

struct AccountProfitLoss {
	double trade_profit_loss{ 0.0f };       //�Ÿ�(û��)����
	double open_profit_loss{ 0.0f };		//�򰡼���
	double pure_trade_profit_loss{ 0.0f };  // û�� ������
	double trade_fee{ 0.0f };               // û�� ������
};
}