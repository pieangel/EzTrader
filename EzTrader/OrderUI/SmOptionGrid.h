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
//#include "../Global/VtDefine.h"
#include <set>
#include <string>
//#include "../Symbol/SmRunInfo.h"
#include "../ViewModel/VmOption.h"

#include "../SmGrid/SmGridResource.h"
#include "../ViewModel/VmOption.h"
#include "../Order/OrderUi/DmDefine.h"
#include "../ViewModel/VmQuote.h"
#include "../Order/SmOrderConst.h"
#include "../ViewModel/VmPosition.h"
namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
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

class VtOrderLeftWndHd;
class VtProductSection;
class VtOrderConfigManager;
class VtSymbol;
struct VtPosition;
struct VtOrder;
class VtProductOrderManager;

class SmOptionGrid : public CGridCtrl
{
public:
	using order_p = std::shared_ptr<DarkHorse::Order>;
	SmOptionGrid();
	~SmOptionGrid();
	void RegisterMasterCallback();
	void RegisterQuoteCallback();
	void RegisterOrderCallback();

	void OnMasterEvent(symbol_p sym);
	void OnOrderEvent(VtOrder* order);
	void OnQuoteEvent(symbol_p sym);

	// 콜백 함수 취소
	void UnregisterAllCallback();
	
	// 그리드 초기화
	void Init();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	afx_msg LRESULT OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOrderChangedMessage(WPARAM wParam, LPARAM lParam);

	void SetColTitle();
	int _ColCount = 3;
	int _RowCount = 100;
	int _CellHeight = 18;
	CFont _defFont;
	CFont _titleFont;
	CPen m_Pen;
public:
	symbol_p _EqualSymbol = nullptr;
	void InitGrid();
	void InitSymbol();
	void InitYearMonth();
	void SetYearMonth();
	void SetProductSection();
	void GetSymbolMaster();
	void RefreshMode();
	VtOrderLeftWndHd* LeftWnd() const { return _LeftWnd; }
	void LeftWnd(VtOrderLeftWndHd* val) { _LeftWnd = val; }
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
	
	void SetRemain(VtPosition* posi);
	void SetRemain(VtOrder* order);

	void InitGrid(int height);
	void calcMaxRow(const int height);
	void calcMaxSymbol();
	int findAtmIndex();
	int calcStartIndex();
	void SetRemain2();
	void SetCurrent2();
	void SetExpected2();
private:
	int _SelectedProduct = 0;
	//std::vector<SmRunInfo> _RunInfo;
	CCellID _EqualCell;
	VtOrderLeftWndHd* _LeftWnd = nullptr;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	VtProductSection* _CurPrdtSec = nullptr;
	void SetEqualRow(int equalRow);
	int GetMaxRow();

	std::map<std::string, std::tuple<int, int, symbol_p>> _SymbolRowMap;
	void MakeSymbolRowMap(int start_index, int max_row);
public:
	void refresh_values();
	void OnSymbolMaster(symbol_p sym);
	void OnOrderFilled(VtOrder* order);
	void OnReceiveQuote(symbol_p sym);
	void OnExpected(symbol_p sym);
	std::pair<int, int> FindValueStartRow(int height);
private:
	void ClearAllText();
	int _ValueStartRow = 0;
	int _ValueMaxRow = 0;
	int _EqualIndex = 0;
	int _MaxIndex = 0;
	int _MaxRow = 0;
	//int _Mode = 1;
	std::set<std::pair<int, int>> _RemainPos;
	void ResetRemainCells();
	void ShowPosition(bool init, int acptCnt, VtPosition* posi, std::string symCode);
	void ShowCurrent(symbol_p sym);
	void ShowExpected(symbol_p sym);
	void ShowExpected(int row, int col, symbol_p sym);
	void ShowCurrent(int row, int col, symbol_p sym);
	void SetRemain(symbol_p sym);
	void QuickSetBackColor(int row, int col, COLORREF color);
	void QuickSetText(int row, int col, LPCTSTR text);
	void QuickSetNumber(int row, int col, int number);
	void QuickSetTextColor(int row, int col, COLORREF color);
public:
	void set_option_view(
		const int option_market_index,
		const std::string& year_month_name
	);
public:
	void update_order(order_p order, DarkHorse::OrderEvent order_event);
	std::shared_ptr<DarkHorse::SmFund> Fund() const { return _Fund; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val);
	//int Mode() const { return _Mode; }
	//void Mode(int val) { _Mode = val; }
	std::shared_ptr<DarkHorse::SmAccount> Account() const { return _Account; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }
	void UpdateSymbolInfo();
	void OnQuoteEvent(const std::string& symbol_code);
	void OnOrderEvent(const std::string& account_no, const std::string& symbol_code);
	void set_view_mode(ViewMode view_mode);
	int order_window_id() const { return order_window_id_; }
	void order_window_id(int val) { order_window_id_ = val; }
private:
	void showValues();
	void set_option_info(
		const size_t index,
		DarkHorse::VmOption& option_info,
		std::shared_ptr<DarkHorse::SmSymbol> symbol,
		std::shared_ptr<DarkHorse::SmQuote> quote);
	std::string get_position_text(const DarkHorse::VmOption& option_info);
	void get_option_position_info(DarkHorse::VmOption& option_info);
	void get_init_accepted_order_count(DarkHorse::VmOption& option_info);
	void set_call_put_area();
	int order_window_id_ = 0;
	void update_expected(std::shared_ptr<DarkHorse::SmQuote> quote);
	void update_quote();
	void update_close(const DarkHorse::VmQuote& quote);
	void update_position();
	void on_update_position_vm(const VmPosition& position);
	void update_value_cell(const int symbol_id, const DarkHorse::VmOption& option_info);
	void on_update_quote();
	void register_symbols(const int option_market_index);
	void register_symbol(const std::string symbol_code);
	void show_value(const int row, const int col, const DarkHorse::VmOption& option_info);
	void set_background_color(std::shared_ptr<DarkHorse::SmCell> cell, const DarkHorse::VmOption& option_info);
	void set_background_color(const int row, const int col, const DarkHorse::VmOption& option_info);
	void show_strike(const int row, const int col, const DarkHorse::VmOption& option_info);
	void show_values();
	ViewMode view_mode_{ ViewMode::VM_Close };
	void set_option_info(
		const int option_market_index,
		const std::string& year_month_name
	);
	void make_symbol_vec(bool call_side = true);
	void init_strike_index();

	std::shared_ptr<DarkHorse::QuoteControl> quote_control_;
	std::shared_ptr<DarkHorse::SymbolPositionControl> position_control_;
	void register_symbols_to_server();
	void set_option_view();
	void set_strike_start_index(const int distance);
	void set_strike();
	DarkHorse::OrderType order_type_{ DarkHorse::OrderType::None };
	int atm_index_{ 0 };
	// 0 : account, 1 : fund
	int _Mode = 0;
	int option_market_index_ = 0;
	std::string year_month_name_;
	int strike_start_index_ = 1;
	bool enable_show_{ true };
	SmOrderGridResource _Resource;
	std::vector<std::string> _HeaderTitles;
	std::shared_ptr<DarkHorse::SmGrid> _Grid = nullptr;

	std::vector<DarkHorse::VmOption> call_symbol_vector_;
	std::vector<DarkHorse::VmOption> put_symbol_vector_;

	// key : (row, col), value : VmOption object
	std::map<std::pair<int, int>, std::shared_ptr<DarkHorse::SmSymbol>> symbol_map_;
	// key : symbol id, value : (row, col)
	std::map<int, std::pair<int, int>> row_col_map_;
	// key : symbol code, value : symbol vector index.
	std::map<std::string, int> symbol_vector_index_map_;

	std::map<int, int> registered_map_;

	int id_{ 0 };

	CBCGPGraphicsManager* m_pGM = nullptr;

	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> _Account = nullptr;
	std::shared_ptr<DarkHorse::SmFund> _Fund = nullptr;

	int _maxRow{ 0 };
	int _maxSymbol{ 0 };
	int _startIndex{ 0 };
	int _atmIndex{ 0 };
};

