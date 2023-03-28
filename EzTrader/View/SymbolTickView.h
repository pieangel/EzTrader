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
namespace DarkHorse {
	class SmGrid;
	class SmSymbol;
	class SmCell;
	class SymbolTickControl;
}

class SymbolTickView : public CBCGPStatic
{
public:
	SymbolTickView();
	~SymbolTickView();
	void SetUp();

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	void Clear();
	std::shared_ptr<DarkHorse::SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<DarkHorse::SmSymbol> val);
	void UpdateSymbolInfo();
	void OnQuoteEvent(const std::string& symbol_code);
	void update_tick();
private:
	bool _EnableQuoteShow = false;
	SmOrderGridResource _Resource;
	void CreateResource();
	void InitHeader();
	std::vector<std::string> _HeaderTitles;
	std::shared_ptr<DarkHorse::SmGrid> _Grid = nullptr;
	std::shared_ptr<DarkHorse::SymbolTickControl> tick_control_;

	CBCGPGraphicsManager* m_pGM = nullptr;

	std::shared_ptr<DarkHorse::SmSymbol> _Symbol = nullptr;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};



