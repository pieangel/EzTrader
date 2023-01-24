#pragma once

#include <memory>

// SmSpreadView view
namespace DarkHorse {
	class SmChartData;
	class SmSymbol;
	class SmTimeData;
	class SmCompData;
}
class SmSpreadView : public CView
{
	DECLARE_DYNCREATE(SmSpreadView)

public:
	SmSpreadView();           // protected constructor used by dynamic creation
	virtual ~SmSpreadView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void SetChartData(std::shared_ptr<DarkHorse::SmChartData> chart_data) {
		_ChartData = chart_data;
	}
private:
	HWND m_hPE;
	int m_nLastSelection = 0;
	std::shared_ptr<DarkHorse::SmChartData> _ChartData = nullptr;
public:
	void LoadData(TCHAR* pName);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};


