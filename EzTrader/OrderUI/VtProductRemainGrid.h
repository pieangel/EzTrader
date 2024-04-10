#pragma once
#include "../stdafx.h"
#include "../UGrid/VtGrid.h"
#include "../UGrid/CellTypes/UGCTSeperateText.h"
#include "../VtDefine.h"
#include "../Position/PositionConst.h"
#include "../Symbol/SymbolConst.h"
#include <memory>

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
	class SymbolPositionControl;
	class QuoteControl;
}
using symbol_p = std::shared_ptr<DarkHorse::SmSymbol>;
using account_p = std::shared_ptr<DarkHorse::SmAccount>;
using fund_p = std::shared_ptr<DarkHorse::SmFund>;
class VtSymbol;
class VtAccount;
struct VtQuote;
struct VtPosition;
class VtOrderConfigManager;
class VtOrderCenterWndHd;
class SmOrderPanel;
class SmOrderPanelOut;
struct VtOrder;
class VtProductRemainGrid : public VtGrid
{
public:
	void UnregisterAllCallback();

	void RegisterQuoteCallback();
	void OnQuoteEvent(const symbol_p symbol);

	void RegisterOrderallback();
	void OnOrderEvent(const VtOrder* order);


	VtProductRemainGrid();
	~VtProductRemainGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual int  OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow);

	void SetColTitle();
	int _ColCount = 6;
	int _RowCount = 1;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);

	void ShowPosition(VtPosition* posi);
	void ShowPosition();
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	SmOrderPanelOut* CenterWnd() const { return _CenterWnd; }
	void CenterWnd(SmOrderPanelOut* val) { _CenterWnd = val; }
	//void SetSymbol(symbol_p sym);
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
private:
	void ShowSinglePosition();
	void ShowFundPosition();
	SmOrderPanelOut* _CenterWnd;
	void ResetColTitle();
	void SetColTitle(int level);
	int FindLevel(int ParentWidth);
	int FindLevel(std::vector<bool>& colOptions);
	void ShowPosition(VtPosition* posi, symbol_p sym);
public:
	int SetColTitleForLevel(int parentWidth);
	int SetColTitleForLevel(std::vector<bool>& colOptions);
	void OnReceiveRealtimeQuote(VtQuote* quote);
	void InitPosition();
	void ClearPosition();
	void OnReceiveQuote(symbol_p sym);
	void SetOrderConfigMgr(VtOrderConfigManager* val);
	void OnRemain(VtPosition* posi);
	void SetShowAvage(bool flag);
	void SetShowRemainType(bool flag);
	void OnReceiveAccountInfo();
	int GetGridWidth(int parentWidth);
	int GetGridWidth(std::vector<bool>& colOptions);
public:
	void Clear();
	symbol_p Symbol() const { return symbol_; }
	void Symbol(symbol_p val);
	void update_quote();
	account_p Account() const { return account_; }
	void Account(account_p val);
	void fund(std::shared_ptr<DarkHorse::SmFund> val);
	void OnOrderChanged(const int& account_id, const int& symbol_id);
	void OnQuoteEvent(const std::string& symbol_code);
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	void Refresh() {
		enable_position_show_ = true;
		enable_quote_show_ = true;
		Invalidate();
	}
	void on_update_quote();
	void on_update_position();
	DarkHorse::PositionType position_type() const { return position_type_; }
	void position_type(DarkHorse::PositionType val) { position_type_ = val; }
	DarkHorse::SymbolType symbol_type() const { return symbol_type_; }
	void symbol_type(DarkHorse::SymbolType val) { symbol_type_ = val; }
	void refresh_position();
private:
	DarkHorse::SymbolType symbol_type_{ DarkHorse::SymbolType::None };
	void set_position();
	DarkHorse::PositionType position_type_{ DarkHorse::PositionType::None };
	void update_position();
	std::shared_ptr<DarkHorse::SymbolPositionControl> position_control_;
	std::shared_ptr<DarkHorse::QuoteControl> quote_control_;
	bool enable_position_show_ = false;
	bool enable_quote_show_ = false;
	bool _Editing{ false };
	std::shared_ptr<DarkHorse::SmSymbol> symbol_{ nullptr };
	std::shared_ptr<DarkHorse::SmAccount> account_{ nullptr };
	std::shared_ptr<DarkHorse::SmFund> fund_{ nullptr };
private:
	bool _Init = false;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	// 모드 : 0 : 국내, 1 : 해외
	int _Mode = 0;
public:
	afx_msg LRESULT OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOrderChangedMessage(WPARAM wParam, LPARAM lParam);
public:
	DECLARE_MESSAGE_MAP()
};

