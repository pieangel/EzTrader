/*
#pragma once
class SymbolTickView
{
};
*/
#pragma once
#include <memory>
#include <vector>
#include <string>

#include "../SmGrid/SmGridResource.h"
#include "../Graphic.h"
namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SymbolTickControl;
}
struct WinInfo;
class VtOrderConfigManager;
class SmOrderPanel;
class SymbolTickView : public CBCGPStatic
{
public:
	SymbolTickView();
	~SymbolTickView();
	void SetUp();
	void SetUp(std::shared_ptr<WinInfo> parent_win_info);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	void Init();
	int get_id() {
		return id_;
	}
	void Clear();
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return symbol_; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val);
	void OnQuoteEvent(const std::string& symbol_code);
	void set_parent(CWnd* parent) {
		parent_ = parent;
	}
	void set_center_window_id(const int center_window_id) {
		center_window_id_ = center_window_id;
	}
	std::shared_ptr<WinInfo> Win_info() const { return win_info_; }
	void Win_info(std::shared_ptr<WinInfo> val) { win_info_ = val; }
	int GetGridWidth();
	void SetOrderConfigMgr(VtOrderConfigManager* val);
	SmOrderPanel* CenterWnd() const { return _CenterWnd; }
	void CenterWnd(SmOrderPanel* val) { _CenterWnd = val; }
	int MaxRow();
	void MaxRow(int val);
private:
	int colWidth[3] = { 60, 55, 58 };
	int _RowCount = 26;
	SmOrderPanel* _CenterWnd = nullptr;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	std::shared_ptr<WinInfo> win_info_{ nullptr };
	int center_window_id_{0};
	int id_{0};
	CWnd* parent_{ nullptr };
	void draw_tick(const int row, const int col, const std::string& value, const int up_down);
	void on_update_tick();
	void update_tick();
	bool _EnableQuoteShow = false;
	SmOrderGridResource _Resource;
	void CreateResource();
	void InitHeader();
	std::vector<std::string> _HeaderTitles;
	std::shared_ptr<DarkHorse::SmGrid> _Grid = nullptr;
	std::shared_ptr<DarkHorse::SymbolTickControl> tick_control_;

	CBCGPGraphicsManager* m_pGM = nullptr;

	std::shared_ptr<DarkHorse::SmSymbol> symbol_ = nullptr;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
private:
	bool _InitResource = false;
	CGraphics* g = NULL;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};



