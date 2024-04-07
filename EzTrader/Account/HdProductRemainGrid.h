#pragma once
#include "../UGrid/VtGrid.h"
#include "../VtDefine.h"
#include "../UGrid/CellTypes/UGCTSeperateText.h"
#include <memory>
#include <vector>
#include <string>
#include <set>
#include <map>

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
	struct Position;
	class AccountPositionControl;
}

class DmAccountOrderWindow;
class DmFundOrderWindow;
class SmOrderCompMainDialog;
class SmFundCompMainDialog;
using position_p = std::shared_ptr<DarkHorse::Position>;

class HdAccountPLDlg;

class HdProductRemainGrid : public VtGrid
{
public:
	HdProductRemainGrid();
	virtual ~HdProductRemainGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual int  OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow);

	void SetColTitle();
	int _ColCount = 4;
	int _RowCount = 100;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);
	void InitGrid();

	HdAccountPLDlg* AcntPLDlg() const { return _AcntPLDlg; }
	void AcntPLDlg(HdAccountPLDlg* val) { _AcntPLDlg = val; }
	void ClearValues();

	std::shared_ptr<DarkHorse::SmFund> Fund() const { return fund_; }
	void Fund(std::shared_ptr<DarkHorse::SmFund> val);
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }

	std::shared_ptr<DarkHorse::SmAccount> Account() const { return account_; }
	void Account(std::shared_ptr<DarkHorse::SmAccount> val);
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val) { _Symbol = val; }

	void UpdatePositionInfo();
	void LiqSelPositions();
	void LiqAll();
	void on_update_single_position(const int position_id);
	void on_update_whole_position(const int result);

private:
	HdAccountPLDlg* _AcntPLDlg = nullptr;
	//void ShowPosition(VtPosition* posi, VtAccount* acnt, int index);
	std::map<VtCellPos, VtCellPos> _OldPosMap;
	void ClearOldValuse(std::map<VtCellPos, VtCellPos>& posMap);
	void RefreshCells(std::map<VtCellPos, VtCellPos>& posMap);
	std::vector<int> _ColWidthVector;
private:
	bool updating_ = false;
	void LiqSelPositionsForAccount();
	void LiqSelPositionsForFund();
	void LiqAllForAccount();
	void LiqAllForFund();
	SmFundCompMainDialog* _CompFundWnd = nullptr;
	DmFundOrderWindow* _FundOrderWnd = nullptr;
	DmAccountOrderWindow* _OrderWnd = nullptr;
	SmOrderCompMainDialog* _CompOrderWnd = nullptr;
	// 0 : account , 1 : fund
	int _Mode = 0;
	void update_account_position();
	void ShowPosition(int index, position_p position, const std::string& format_type);
	void update_dm_account_position(CBCGPGridRow* row, position_p position, const std::string& format_type);
	void update_ab_account_position(CBCGPGridRow* row, position_p position, const std::string& format_type);
	bool _EnableQuoteShow = false;
	bool enable_position_show_ = false;
	COLORREF _DefaultBackColor;
	bool _HeaderCheck = false;
	int _OldMaxRow = -1;
	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
	std::shared_ptr<DarkHorse::SmAccount> account_ = nullptr;
	std::shared_ptr<DarkHorse::SmFund> fund_ = nullptr;

	std::set<int> _OldContentRowSet;
	std::shared_ptr<DarkHorse::AccountPositionControl> account_position_control_;
	// key : row, value : position object.
	std::map<int, std::shared_ptr<DarkHorse::Position>> row_to_position_;
	// key : position object id, value : row.
	std::map<int, int> position_to_row_;
	std::vector<int> ab_column_widths_vector_;
	std::vector<int> dm_column_widths_vector_;
	int id_{ 0 };
	int main_window_id_ = 0;
public:
	void set_order_window_id(int id) { main_window_id_ = id; };
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

