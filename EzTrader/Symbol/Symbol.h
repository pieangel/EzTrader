#pragma once
#include <string>
namespace DarkHorse {
	struct Symbol {
		std::string symbol_code;
		std::string full_code;
		std::string name_en;
		std::string name_kr;
		int remain_days;
		std::string last_trade_day;
		std::string high_limit_price;
		std::string low_limit_price;
		std::string preday_close;
		std::string standard_price;
		std::string strike;
		/// <summary>
		/// 0 : future, 1 : atm , 2 : itm, 3 : otm
		/// </summary>
		int atm_type;
		/// <summary>
		/// 1 : 최근원물, 선물 스프레드, 2 : 2째월물, 3등등.
		/// </summary>
		int recent_month;
		std::string expire_day;
		int id{ 0 };
		int seung_su{ 250000 };
		int decimal{ 2 };
		double contract_size{ 0.05 };
		double tick_value{ 12500 };
		double tick_size{ 0.05 };
		std::string market_name;
		std::string product_code;
		int total_volume{ 0 };
		int preday_volume{ 0 };
		std::string deposit;
		std::string start_time;
		std::string end_time;
		std::string preday_updown_rate;
		std::string currency;
		std::string exchange;
	};
}