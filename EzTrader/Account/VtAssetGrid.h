#pragma once
#include "../UGrid/VtGrid.h"
#include <vector>
class VtAssetGrid : public VtGrid
{
public:
	VtAssetGrid();
	virtual ~VtAssetGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);

	void SetRowTitle();
	int _ColCount = 4;
	int _RowCount = 5;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);
private:
	std::vector<int> _ColWidVec;
public:
	void InitGrid();
};

