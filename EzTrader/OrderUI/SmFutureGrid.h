#pragma once
#include "../Grid/GridCtrl.h"
#include <vector>
#include <map>
#include <set>
#include <functional>
//#include "../Global/VtDefine.h"
#include <memory>
#include <map>
#include <tuple>
#include <set>
#include <string>
#include "../Order/OrderUi/DmDefine.h"
#include "../ViewModel/VmFuture.h"
#include "../ViewModel/VmQuote.h"
#include "../ViewModel/VmFuture.h"
#include "../Order/SmOrderConst.h"
#include "../ViewModel/VmPosition.h"

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
	class QuoteControl;
	struct SmQuote;
	class SymbolPositionControl;
	struct Order;
	struct Position;
}

using symbol_p = std::shared_ptr<DarkHorse::SmSymbol>;
using account_p = std::shared_ptr<DarkHorse::SmAccount>;
using fund_p = std::shared_ptr<DarkHorse::SmFund>;
using order_p = std::shared_ptr<DarkHorse::Order>;
using position_p = std::shared_ptr<DarkHorse::Position>;
class VtOrderLeftWndHd;
class VtProductSection;
class VtOrderConfigManager;
class VtSymbol;
struct VtPosition;
struct VtOrder;
class VtProductOrderManager;

class SmFutureGrid : public CGridCtrl
{
public:
	SmFutureGrid();
	~SmFutureGrid();

	
	void RegisterMasterCallback();
	void RegisterQuoteCallback();
	void RegisterOrderCallback();

	void OnMasterEvent(symbol_p sym);
	void OnOrderEvent(VtOrder* order);
	void OnQuoteEvent(symbol_p sym);

	// 콜백 함수 취소
	void UnregisterAllCallback();
	
	void Init();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnOrderChangedMessage(WPARAM wParam, LPARAM lParam);
	
	void SetColTitle();
	int _ColCount = 3;
	int _RowCount = 50;
	int _CellHeight = 21;
	CFont _defFont;
public:
	void InitGrid();

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	std::map<std::string, std::tuple<int, int, symbol_p>> _FutureCurrentValueRowMap;
	int _Mode = 1;
	COLORREF _SellColor;
	COLORREF _BuyColor;
	COLORREF _DefaultColor;
	void ShowPosition(bool init, int acptCnt, VtPosition* posi, std::string symCode);
	void GetSymbolMaster(symbol_p sym);

	void QuickSetBackColor(int row, int col, COLORREF color);
	void QuickSetText(int row, int col, LPCTSTR text);
	void QuickSetNumber(int row, int col, int number);
	void QuickSetTextColor(int row, int col, COLORREF color);

public:
	void OnReceiveQuote(symbol_p sym);
	void OnSymbolMaster(symbol_p sym);
	void SetRemain(VtPosition* posi);
	void SetRemain(VtOrder* order);
	void SetRemain(std::string symbol_code);
	void ShowRemain(symbol_p sym);
	void ShowCurrent(symbol_p sym, int row);
	void OnOrderFilled(VtOrder* order);
	void OnExpected(symbol_p sym);
	void ShowExpected(symbol_p sym, int row);
public:
	void update_expected_direct(std::shared_ptr<DarkHorse::SmQuote> quote);
	void update_quote_direct(std::shared_ptr<DarkHorse::SmQuote> quote);
	std::shared_ptr<DarkHorse::SmFund> Fund() const { return _Fund; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val);
	void init_dm_future();
	void update_quote();
	void set_view_mode(ViewMode view_mode);
	void on_update_position_vm_future(const VmPosition& position);
	void on_update_position_direct(position_p position);
public:
	void refresh_values();
	void update_order(order_p order, DarkHorse::OrderEvent order_event);
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return _Account; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }
	int order_window_id() const { return order_window_id_; }
	void order_window_id(int val) { order_window_id_ = val; }
private:
	void set_background_color(const int row, const int col, const DarkHorse::VmFuture& option_info);
	std::string get_position_text(const DarkHorse::VmFuture& future_info);
	void get_future_info(DarkHorse::VmFuture& future_info);
	void get_init_accepted_order_count(DarkHorse::VmFuture& future_info);
	DarkHorse::OrderType order_type_{ DarkHorse::OrderType::None };
	int order_window_id_ = 0;
	void set_position(DarkHorse::VmFuture& future_info);
	void update_position();
	void on_update_position();
	void on_update_quote();
	void update_close(const DarkHorse::VmQuote& quote);
	void show_values();
	void show_value(const int row, const int col, const DarkHorse::VmFuture& future_info);
	ViewMode view_mode_{ ViewMode::VM_Close };
	std::shared_ptr<DarkHorse::QuoteControl> quote_control_;
	std::shared_ptr<DarkHorse::SymbolPositionControl> position_control_;
	void register_symbol_to_server(std::shared_ptr<DarkHorse::SmSymbol> symbol);
	void register_symbols();
	bool registered_{ false };
	// 0 : account, 1 : fund
	//int _Mode = 0;
	bool enable_show_{ true };
	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	std::shared_ptr<DarkHorse::SmFund> _Fund = nullptr;

	std::vector<DarkHorse::VmFuture> symbol_vec_;
	// key : symbol code, value : row index.
	std::map<std::string, int> symbol_row_index_map_;
	int id_{ 0 };
};

