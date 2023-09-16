#pragma once
#include "Skel/VtGrid.h"
#include "UGrid/CellTypes/UGCTSeperateText.h"
#include "Global/VtDefine.h"
#include <map>
#include <vector>
#include "UGrid/CellTypes/ugctelps.h"
#include "UGrid/CellTypes/ugctspin.h"
#include "UGrid/CellTypes/UGCTbutn.h"
#include "VtOutSystemManager.h"

#define ELLIPSISBUTTON_CLICK_ACNT		150
#define ELLIPSISBUTTON_CLICK_PRDT		151
#define SPIN_TYPE_SEUNGSU               152
class VtAccount;
class VtFund;
class VtSystem;
class VtTotalSignalGrid;
class VtSymbol;
class VtOrderLogDlg;
class VtSignalConnectionGrid : public VtGrid
{
public:
	VtSignalConnectionGrid();
	virtual ~VtSignalConnectionGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed = 0);
	//cell type notifications
	int OnCellTypeNotify(long ID, int col, long row, long msg, long param);
	virtual void OnMouseLeaveFromMainGrid();
	void SetColTitle();
	int _ColCount = 9;
	int _RowCount = 100;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);
	void InitGrid();

	void ClearCells();

	//Ellipsis Button cell type
	CUGEllipsisType		m_ellipsis;
	int					m_nEllipsisIndex;
	//Push Button cell type
	CUGButtonType		m_button;
	int					m_nButtonIndex;

	//Spin Button cell type
	CUGSpinButtonType	m_spin;
	int					m_nSpinIndex;
	void SetTargetAcntOrFund(std::tuple<int, VtAccount*, VtFund*>& selItem);
	void SetSymbol(VtSymbol* sym);
	void AddSystem(SharedSystem sys);
	void RemoveSystem();
	VtTotalSignalGrid* TotalGrid() const { return _TotalGrid; }
	void TotalGrid(VtTotalSignalGrid* val) { _TotalGrid = val; }
	void Refresh();
	void SetCheck(bool flag);
	void RefreshOrders();
	void ClearCheck(VtSystem* sys);
private:
	int _SelRow = -2;
	int _OldSelRow = -2;
	int _ClickedRow = -2;
	int _ButtonRow = -2;
	int _OccupiedRowCount = 0;
	COLORREF _SelColor = RGB(255, 227, 132);
	COLORREF _ClickedColor = RGB(216, 234, 253);
	//celltype notification handlers
	int OnDropList(long ID, int col, long row, long msg, long param);
	int OnCheckbox(long ID, int col, long row, long msg, long param);
	int OnEllipsisButton(long ID, int col, long row, long msg, long param);
	int OnSpinButton(long ID, int col, long row, long msg, long param);
	int OnLogButton(int col, long row);
	int OnButton(long ID, int col, long row, long msg, long param);
	// 키 : 행인덱스, 값 : 시스템 객체
	std::map<int, SharedSystem> _SystemMap;
	// 키 : 시스템 아이디, 값 : 행 인덱스
	std::map<int, int> _SystemToRowMap;
	VtTotalSignalGrid* _TotalGrid = nullptr;
	VtOrderLogDlg* _LogDlg = nullptr;
};


