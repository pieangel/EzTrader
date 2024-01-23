#pragma once
#include "../UGrid/VtGrid.h"
#include <set>
#include <map>
#include "../UGrid/CellTypes/UGCTbutn.h"
class VtFDSubAcntPage;
class VtAccount;
class VtOrderConfigManager;
class SmFavoriteGrid : public VtGrid
{
public:
	SmFavoriteGrid();
	virtual ~SmFavoriteGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed = 0);
	virtual void OnMouseLeaveFromMainGrid();
	void SetColTitle();
	int _ColCount = 2;
	int _RowCount = 100;
	CFont _defFont;
	CFont _titleFont;

	//Push Button cell type
	CUGButtonType		m_button;
	int					m_nButtonIndex;
	CBitmap				m_noteBitmap;

	void QuickRedrawCell(int col, long row);

	void InitGrid(VtAccount* acnt);
	void InitGrid();
	void ClearCells();
	VtAccount* GetSelectedAccount();
	void SetDefaultSelect();
	VtFDSubAcntPage* SubAcntPage() const { return _SubAcntPage; }
	void SubAcntPage(VtFDSubAcntPage* val) { _SubAcntPage = val; }
	VtAccount* ParentAccount() const { return _ParentAccount; }
	void ParentAccount(VtAccount* val) { _ParentAccount = val; }

	afx_msg LRESULT OnQuoteChangedMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()

	
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	int _SelRow = -2;
	int _OldSelRow = -2;
	int _ClickedRow = 0;
	int _ButtonRow = -2;
	int _OccupiedRowCount = 0;
	COLORREF _SelColor = RGB(255, 227, 132);
	COLORREF _ClickedColor = RGB(216, 234, 253);
	void ChangeSelectedRow(int oldRow, int newRow);
	std::set<std::pair<int, int>> _RefreshCells;
	VtFDSubAcntPage* _SubAcntPage;
	VtAccount* _ParentAccount;
	std::map<std::string, int> _CodeIntMap;
public:
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
};



