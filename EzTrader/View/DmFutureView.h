
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>

#include "../SmGrid/SmGridResource.h"
#include "../Order/OrderUi/DmDefine.h"
#include "../ViewModel/VmFuture.h"
#include "../ViewModel/VmQuote.h"
#include "../ViewModel/VmFuture.h"

namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SmAccount;
	class SmFund;
	class QuoteControl;
	struct SmQuote;
}

class DmFutureView : public CBCGPStatic
{
public:
	DmFutureView();
	~DmFutureView();
	void SetUp();

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	void update_expected(std::shared_ptr<DarkHorse::SmQuote> quote);
	std::shared_ptr<DarkHorse::SmFund> Fund() const { return _Fund; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val) { _Fund = val; }
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
	void init_dm_future();
	void update_quote();
	void set_view_mode(ViewMode view_mode);
public:
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return _Account; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val) { _Account = val; }
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }
	void UpdateSymbolInfo();
	void UpdateAssetInfo();
	void OnQuoteEvent(const std::string& symbol_code);
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
private:
	void update_close(const DarkHorse::VmQuote& quote);
	void update_close_cell(const int row, const DarkHorse::VmFuture& future_info);
	void update_expected_cell(const int row, const DarkHorse::VmFuture& future_info);
	void show_values();
	void show_value(const int row, const int col, const DarkHorse::VmFuture& future_info);
	ViewMode view_mode_{ ViewMode::VM_Close };
	std::shared_ptr<DarkHorse::QuoteControl> quote_control_;
	void register_symbol_to_server(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void UpdateAccountAssetInfo();
	void UpdateFundAssetInfo();
	void register_symbols();
	bool registered_{ false };
	// 0 : account, 1 : fund
	int _Mode = 0;
	bool _EnableOrderShow = false;
	bool _EnableQuoteShow = false;
	SmOrderGridResource _Resource;
	void CreateResource();
	void InitHeader();
	std::vector<std::string> _HeaderTitles;
	std::shared_ptr<DarkHorse::SmGrid> _Grid = nullptr;

	CBCGPGraphicsManager* m_pGM = nullptr;

	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	std::shared_ptr<DarkHorse::SmFund> _Fund = nullptr;
	
	std::vector<DarkHorse::VmFuture> symbol_vec_;
	// key : symbol code, value : row index.
	std::map<std::string, int> symbol_row_index_map_;
	int id_{ 0 };
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};






