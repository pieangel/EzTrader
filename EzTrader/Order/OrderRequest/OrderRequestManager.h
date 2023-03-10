#pragma once
#include <memory>
#include <map>
#include <array>
#include <mutex>
#include "../../Common/common.h"
#include "../../Common/BlockingCollection.h"
#include "../SmOrderConst.h"
using namespace code_machina;
namespace DarkHorse {
	constexpr int BulkOrderRequestSize = 500;
	struct OrderRequest;
	using order_request_p = std::shared_ptr<OrderRequest>;
	class OrderRequestManager : public Runnable
	{
	public:
		OrderRequestManager() {};
		virtual ~OrderRequestManager() {};

		// Static Members
		static int id_;
		static int get_id() { return id_++; }

		order_request_p find_order_request(const int order_request_id);
		void add_order_request(order_request_p order_request);
		void remove_order_request(const int order_request_id);
		void start_handle_order_request() noexcept;
		void stop_handle_order_request() noexcept;
		unsigned int ThreadHandlerProc(void) override;
		bool enable() const { return enable_; }
		void enable(bool val) { enable_ = val; }

		static order_request_p make_order_request(
			const int order_price,
			const int order_amount,
			const std::string& symbol_code,
			const SmPositionType& position_type,
			const SmOrderType& order_type,
			const SmPriceType& price_type,
			const SmFilledCondition& fill_condition = SmFilledCondition::Day);

		static order_request_p make_order_request(
			const std::string& account_no,
			const std::string& password,
			const int order_price,
			const int order_amount,
			const std::string& symbol_code,
			const SmPositionType& position_type,
			const SmOrderType& order_type,
			const SmPriceType& price_type,
			const SmFilledCondition& fill_condition = SmFilledCondition::Day);

		static order_request_p make_default_sell_order_request(
			const std::string& symbol_code,
			const int order_price,
			const SmPositionType& position_type = SmPositionType::Sell,
			const int order_amount = 1,
			const SmOrderType& order_type = SmOrderType::New,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmFilledCondition& fill_condition = SmFilledCondition::Day);

		static order_request_p make_default_sell_order_request(
			const std::string& account_no,
			const std::string& password,
			const std::string& symbol_code,
			const int order_price,
			const int order_amount = 1,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmPositionType& position_type = SmPositionType::Sell,
			const SmOrderType& order_type = SmOrderType::New,
			const SmFilledCondition& fill_condition = SmFilledCondition::Day);

		static order_request_p make_default_buy_order_request(
			const std::string& symbol_code,
			const int order_price,
			const SmPositionType& position_type = SmPositionType::Buy,
			const int order_amount = 1,
			const SmOrderType& order_type = SmOrderType::New,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmFilledCondition& fill_condition = SmFilledCondition::Day);

		static order_request_p make_default_buy_order_request(
			const std::string& account_no,
			const std::string& password,
			const std::string& symbol_code,
			const int order_price,
			const int order_amount = 1,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmPositionType& position_type = SmPositionType::Buy,
			const SmOrderType& order_type = SmOrderType::New,
			const SmFilledCondition& fill_condition = SmFilledCondition::Day);

		static order_request_p  make_change_order_request(
			const std::string& account_no,
			const std::string& password,
			const std::string& symbol_code,
			const std::string& ori_order_no,
			const int order_price,
			const SmPositionType& position_type = SmPositionType::Buy,
			const int order_amount = 1,
			const SmOrderType& order_type = SmOrderType::Modify,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmFilledCondition& fill_condition = SmFilledCondition::Day);

		static order_request_p make_cancel_order_request(
			const std::string& account_no,
			const std::string& password,
			const std::string& symbol_code,
			const std::string& ori_order_no,
			const int order_price,
			const SmPositionType& position_type = SmPositionType::Buy,
			const int order_amount = 1,
			const SmOrderType& order_type = SmOrderType::Cancel,
			const SmPriceType& price_type = SmPriceType::Price,
			const SmFilledCondition& fill_condition = SmFilledCondition::Day);
	private:
		void clear_order_requests() noexcept;
		bool enable_{ true };
		std::mutex order_request_map_mutex_;
		BlockingCollection<order_request_p> order_request_q;
		// key : request id, value : request object.
		// ???? ?????????? ?????? ?????????? ?????? ?????????? ?????? ??, mutex?? lock?? ???????? ????. 
		std::map<int, order_request_p> order_request_map;
		// arr : ???????? ???? ???? ????, taken : ???? ???????? ???? ???? ????
		bool handle_order_request(std::array<order_request_p, BulkOrderRequestSize>& arr, int taken);
		order_request_p make_dummy_order_request();
		void add_order_request_map(order_request_p order_request);

	public:
		order_request_p find_order_request(const std::string& custom_info); 
	};
}
