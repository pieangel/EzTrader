
#pragma once
#include "../UGrid/VtGrid.h"
#include <list>
#include <map>
#include <memory>

namespace DarkHorse {
	class SmSymbol;
	class SmAccount;
	class SmFund;
}

using symbol_p = std::shared_ptr<DarkHorse::SmSymbol>;
using account_p = std::shared_ptr<DarkHorse::SmAccount>;
using fund_p = std::shared_ptr<DarkHorse::SmFund>;

class VtOrderConfigManager;
class CVtOrderLeftWnd;
struct VtPosition;
class VtSymbol;
class VtProductRemainGridEx : public VtGrid
{
public:
	VtProductRemainGridEx();
	~VtProductRemainGridEx();

	// 콜백 함수 취소
	void UnregisterAllCallback();

	void RegisterQuoteCallback();
	void UnregisterQuoteCallback();
	void OnQuoteEvent(VtSymbol* symbol);

	virtual void OnSetup();
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	//cell type notifications
	virtual int OnCellTypeNotify(long ID, int col, long row, long msg, long param);
	void SetColTitle();
	int _ColCount = 4;
	int _RowCount = 200;
	CFont _defFont;
	CFont _titleFont;
	CBitmap				m_OutBitmap;

	void SetFilledOrderList();
	void QuickRedrawCell(int col, long row);
	void ResetCellText();
	void SetRemainList();

	void SetOrderConfigMgr(VtOrderConfigManager* val);
	CVtOrderLeftWnd* OrderLeftWnd() const { return _OrderLeftWnd; }
	void OrderLeftWnd(CVtOrderLeftWnd* val) { _OrderLeftWnd = val; }
	void LiqSelectedPos();
	void LiqAll();
private:
	void PutOrder(VtPosition* posi, int price, bool liqud = false);
	std::map<int, VtPosition*> _RowToPositionMap;
	std::list<VtPosition*> _SelectedPosList;
	void LiqList();
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	CVtOrderLeftWnd* _OrderLeftWnd = nullptr;
	void LiqudRemain(VtPosition* posi);
};



