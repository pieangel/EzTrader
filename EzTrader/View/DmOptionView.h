
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>

#include "../SmGrid/SmGridResource.h"
namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SmAccount;
	class SmFund;
}

class DmOptionView : public CBCGPStatic
{
public:
	DmOptionView();
	~DmOptionView();
	void SetUp();

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	std::shared_ptr<DarkHorse::SmFund> Fund() const { return _Fund; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val) { _Fund = val; }
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
public:
	void set_option_view(
		const int option_market_index, 
		const std::string& year_month_name);
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return _Account; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val) { _Account = val; }
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }
	void UpdateSymbolInfo();
	void UpdateAssetInfo();
	void OnQuoteEvent(const std::string& symbol_code);
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
private:
	void set_option_view();
	int get_atm_index(const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& symbol_vec);
	void set_strike_start_index(const int distance);
	void set_strike
	(
		const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& call_symbol_vec,
		const std::vector<std::shared_ptr<DarkHorse::SmSymbol>>& put_symbol_vec
	);
	void UpdateAccountAssetInfo();
	void UpdateFundAssetInfo();
	// 0 : account, 1 : fund
	int _Mode = 0;
	int option_market_index_ = 0;
	std::string year_month_name_;
	int strike_start_index_ = 1;
	size_t max_symbol_count = 0;
	bool _EnableOrderShow = false;
	bool _EnableQuoteShow = false;
	SmOrderGridResource _Resource;
	void CreateResource();
	void InitHeader();
	std::vector<std::string> _HeaderTitles;
	std::shared_ptr<DarkHorse::SmGrid> _Grid = nullptr;

	std::vector<std::shared_ptr<DarkHorse::SmSymbol>> call_symbol_vector_;
	std::vector<std::shared_ptr<DarkHorse::SmSymbol>> put_symbol_vector_;
	/// <summary>
	/// key : row index, value : the index of the symbol vector.
	/// </summary>
	//std::map<int, int> symbol_index_map_;

	std::map<std::pair<int, int>, std::shared_ptr<DarkHorse::SmSymbol>> call_symbol_map_;
	std::map<std::pair<int, int>, std::shared_ptr<DarkHorse::SmSymbol>> put_symbol_map_;

	CBCGPGraphicsManager* m_pGM = nullptr;

	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	std::shared_ptr<DarkHorse::SmFund> _Fund = nullptr;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};





