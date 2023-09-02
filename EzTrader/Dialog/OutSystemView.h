#pragma once
#include <BCGCBProInc.h>
#include <map>
#include <memory>	

namespace DarkHorse
{
	class SmSymbol;
	class SmOutSystem;
}
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
