#pragma once
#include "../UGrid/VtGrid.h"
#include "../UGrid/CellTypes/UGCTSeperateText.h"
//#include "Global/VtDefine.h"
#include "../UGrid/CellTypes/UGCTbutn.h"
#include <map>
#include <vector>
#include <memory>
namespace DarkHorse {
	class QuoteControl;
	class SmOutSystem;
}
class VtSignalConnectionGrid;
class VtTotalSignalGrid : public VtGrid
{
public:
	VtTotalSignalGrid();
	virtual ~VtTotalSignalGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	//cell type notifications
	int OnCellTypeNotify(long ID, int col, long row, long msg, long param);
	//Push Button cell type
	CUGButtonType		m_button;
	int					m_nButtonIndex;

	void SetColTitle();
	int _ColCount = 10;
	int _RowCount = 100;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);
	void Refresh();
	void RefreshOrders();
	void ClearCells();
	VtSignalConnectionGrid* ConnectGrid() const { return _ConnectGrid; }
	void ConnectGrid(VtSignalConnectionGrid* val) { _ConnectGrid = val; }
private:
	void on_update_quote();
	std::shared_ptr<DarkHorse::QuoteControl> quote_control_;
	int OnButton(long ID, int col, long row, long msg, long param);
	void InitGrid();
	int _RowNumber = 0;
	// key : row index, value : out system object.
	std::map<int, std::shared_ptr<DarkHorse::SmOutSystem>> out_system_map_;
	VtSignalConnectionGrid* _ConnectGrid = nullptr;
};


