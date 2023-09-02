#pragma once

#include <BCGCBProInc.h>
#include <map>
#include <memory>	

namespace DarkHorse
{
	class SmSymbol;
}
const int grid_row_count3 = 100;
class ActiveOutSystemView : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(ActiveOutSystemView)
public:
	ActiveOutSystemView();
	virtual ~ActiveOutSystemView();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
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
