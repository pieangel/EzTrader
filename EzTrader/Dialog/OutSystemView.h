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
//const int grid_row_count = 100;
class OutSystemView : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(OutSystemView)
public:
	OutSystemView();
	virtual ~OutSystemView();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void add_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system);
protected:
	//{{AFX_MSG(CBasicGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	std::map<int, std::shared_ptr<DarkHorse::SmSymbol>> row_to_symbol_;
	COLORREF _DefaultBackColor;
	void ClearGrid();
	bool init_ = false;

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
	CAccountItem(const CString& strValue, OutSystemView& pOutSystemVeiw, const int mode);

	// Overrides
	virtual void OnClickButton(CPoint point);
	int mode() const { return mode_; }
	void mode(int val) { mode_ = val; }

private:
	OutSystemView& pOutSystemVeiw_;

	//std::shared_ptr<HdSymbolSelecter> _SymbolSelecter;
	int id_{ 0 };
	int mode_{ 0 };
};
