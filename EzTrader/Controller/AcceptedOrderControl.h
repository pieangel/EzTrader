#pragma once
#include <map>
#include <string>
#include <memory>
#include "../ViewModel/VmOrder.h"
namespace DarkHorse {
	class SmOrder;
	struct PriceOrder {
		int price;
		int count;
		// key : order no, value : accepted order object.
		std::map<std::string, AcceptedOrder> order_map;
	};
	/// <summary>
	/// 메인계좌 일 때는 메인 계좌와 서브 계좌의 모든 주문을 저장해야 한다.
	/// 펀드 일 때는 펀드에 속한 모든 계좌의 주문을 저장해야 한다. 
	/// 컨트롤에 각 계좌의 id 맵을 가지고 들어오는 주문을 필터링 해야 한다. 
	/// </summary>
	class AcceptedOrderControl
	{
	public:
		int get_total_count() {
			return total_count_;
		}
		void update_order(std::shared_ptr<SmOrder> order_p);
	private:
		int total_count_{ 0 };
		// key : price as integer, value : order list on the price. 
		std::map<int, PriceOrder> order_map_;
	};
}

