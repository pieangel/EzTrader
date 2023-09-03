#pragma once

#include <BCGCBProInc.h>
#include <map>
#include <memory>	
#include <vector>

namespace DarkHorse
{
	class SmSymbol;
	class SmOutSystem;
}
const int active_out_system_row = 300;
class ActiveOutSystemView : public CBCGPGridCtrl
{
	DECLARE_DYNAMIC(ActiveOutSystemView)
public:
	ActiveOutSystemView();
	virtual ~ActiveOutSystemView();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void add_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system);
	void remove_out_system(std::shared_ptr<DarkHorse::SmOutSystem> out_system);
protected:
	//{{AFX_MSG(CBasicGridCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void init_grid();
	// key : out system id, value : row, 
	std::map<int, int> row_to_out_system_;
	std::vector<std::shared_ptr<DarkHorse::SmOutSystem>> out_systems_;
	void remap_row_to_out_system();
	COLORREF _DefaultBackColor;
	void ClearGrid();
	bool init_ = false;
};
