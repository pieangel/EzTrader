#pragma once
#include <string>
#include "../Order/SmOrderConst.h"
namespace DarkHorse {
struct Position
{
	// �ɺ� �ڵ�
	std::string symbol_code;
	// ���� ��ȣ
	std::string account_no;
	// ������
	SmPositionType position_type = SmPositionType::None;
	// �������� �ܰ�
	// �������� �ܰ�� �ż��� �϶��� ���, �ŵ��� ���� ������ ǥ���Ѵ�.
	// �������� �ܰ� 0�̸� �������� ���� ���̴�.
	int open_quantaty{ 0 };
	double trade_profit_loss{ 0.0f }; //	�Ÿż���
	double average_price{ 0.0f };	// ��հ�*/
	double open_profit_loss{ 0.0f };		/*�򰡼���*/
	double pure_trade_profit_loss{ 0.0f }; // û�� ������
	double trade_fee{ 0.0f }; // û�� ������
};
}