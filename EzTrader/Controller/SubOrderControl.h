#pragma once
#include <map>
#include <string>
#include <memory>
#include "../ViewModel/VmOrder.h"
namespace DarkHorse {
	class SmOrder;
	struct PriceOrderMap {
		int price;
		int count;
		// key : order no, value : accepted order object.
		std::map<std::string, AcceptedOrder> order_map;
	};
	/// <summary>
	/// ���ΰ��� �� ���� ���� ���¿� ���� ������ ��� �ֹ��� �����ؾ� �Ѵ�.
	/// �ݵ� �� ���� �ݵ忡 ���� ��� ������ �ֹ��� �����ؾ� �Ѵ�. 
	/// ��Ʈ�ѿ� �� ������ id ���� ������ ������ �ֹ��� ���͸� �ؾ� �Ѵ�. 
	/// </summary>
	struct SubOrderControl
	{
		int total_count{ 0 };
		// key : price as integer, value : order list on the price. 
		std::map<int, PriceOrderMap> order_map;
	};
}

