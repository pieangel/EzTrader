
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>

#include "../SmGrid/SmGridResource.h"
#include "../Order/OrderUi/DmDefine.h"
namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SmAccount;
	class SmFund;
	class QuoteControl;
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
	ViewMode view_mode_{ ViewMode::VM_Close };
	std::shared_ptr<DarkHorse::QuoteControl> quote_control_;
	void register_symbol_to_server(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void UpdateAccountAssetInfo();
	void UpdateFundAssetInfo();
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
	/// <summary>
	/// key : row index, value : symbol object.
	/// </summary>
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> symbol_map_;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};






