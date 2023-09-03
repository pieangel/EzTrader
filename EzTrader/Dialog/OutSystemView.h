#pragma once
#include <BCGCBProInc.h>
#include <map>
#include <memory>	

namespace DarkHorse
{
	class SmSymbol;
	class SmOutSystem;
}

class HdSymbolSelecter;
class VtAccountFundSelector;
class OutSystemView : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(OutSystemView)
public:
	OutSystemView();
	virtual ~OutSystemView();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void add_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system);
	void remove_out_system();
protected:
	//{{AFX_MSG(CBasicGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void init_grid();
	void remap_row_to_out_system();
	// key : row, value : out system
	std::map<int, std::shared_ptr<DarkHorse::SmOutSystem>> row_to_out_system_;
	COLORREF _DefaultBackColor;
	void ClearGrid();
	int out_system_row_count = 300;
	int selected_row_ = -1;
	void create_out_system_cells(CBCGPGridRow* pRow, std::shared_ptr<DarkHorse::SmOutSystem> out_system);
	CBCGPGridRow* create_or_get_row(const int row_index);
};

class  CSymbolItem : public CBCGPGridItem
{
	// Construction
public:
	CSymbolItem(const CString& strValue, OutSystemView& pOutSystemVeiw);

	// Overrides
	virtual void OnClickButton(CPoint point);
private:
	OutSystemView& pOutSystemVeiw_;

	int id_{ 0 };
	void set_symbol_from_out(const int window_id, std::shared_ptr<DarkHorse::SmSymbol> symbol);
};


class  CAccountItem : public CBCGPGridItem
{
	// Construction
public:
	CAccountItem(const CString& strValue, OutSystemView& pOutSystemVeiw);

	// Overrides
	virtual void OnClickButton(CPoint point);

private:
	OutSystemView& pOutSystemVeiw_;

	//std::shared_ptr<HdSymbolSelecter> _SymbolSelecter;
	int id_{ 0 };
};
