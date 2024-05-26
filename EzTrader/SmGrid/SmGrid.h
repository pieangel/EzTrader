#pragma once
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include "SmGridResource.h"
#include "SmCellType.h"
#include "SmGridConsts.h"
#include "SmButtonId.h"

#define SM_INPLACE_CONTROL   8                  // ID of inplace edit controls


class CGraphics;
namespace DarkHorse {
	
	class SmCell;
	class SmRow;
	class SmGrid
	{
	public:
		SmGrid(SmOrderGridResource& res, const int& row_count, const int& col_count)
			: m_pEditWnd(nullptr), _Res(res), _RowCount(row_count), _ColCount(col_count)
		{
		  _defaultRowHeight = DefaultRowHeight;
		}
		~SmGrid() {}

		void SetOrderHeaderTitles();
		int RecalRowCount(const int& height, bool change_close_row);
		void MakeColWidthMap();
		void MakeRowHeightMap();
		void ResetAllHeightMap(const int height);
		void SetMergeCells();
		void AddMergeCell(const int& row, const int& col, const int& row_span, const int& col_span);
		int index_row() const { return index_row_; }
		void index_row(int val) { index_row_ = val; }
		DarkHorse::SmHeaderMode HeaderMode() const { return _HeaderMode; }
		void HeaderMode(DarkHorse::SmHeaderMode val) { _HeaderMode = val; }
		DarkHorse::SmHeaderPriority HeaderPriority() const { return _HeaderPriority; }
		void HeaderPriority(DarkHorse::SmHeaderPriority val) { _HeaderPriority = val; }

		int GridLineWidth() const { return _GridLineWidth; }
		void GridLineWidth(int val) { _GridLineWidth = val; }
		void Clear();
		void SetDefaultRowHeight(const int value) 
		{
		  _defaultRowHeight = value;
		}
		int GetDefaultRowHeight() const {
		  return _defaultRowHeight;
		}
	private:
	  int _defaultRowHeight;
		int _GridLineWidth = 1;
		CWnd* m_pEditWnd;
		const SmOrderGridResource& _Res;
		std::vector<std::string> _RowHeaderTitles;
		std::vector<std::string> _ColHeaderTitles;
		int _Height = 0;
		int _Width = 0;
		std::set<std::pair<int, int>> _ButtonSet;
		std::map<int, int> _RowHeightMap;
		std::map<int, int> _ColWidthMap;
		//std::map <std::pair<int, int>, std::shared_ptr<SmCell>> _CellMap;
		// Key : <row, col> pair, Value : <row_span, col_span> pair
		std::map<std::pair<int, int>, std::pair<int, int>> _MergedCellMap;

		std::map<int, std::shared_ptr<SmRow>> _RowMap;
		
		int _RowCount = 0;
		int _ColCount = 0;
		bool _ShowGrid = true;
		int _RowGridLineHeight = 1;
		int _ColGridWidth = 1;
		int _GridHeight = 0;
		int index_row_{ 15 };
		int _HeaderRow = 0;
		int _HeaderCol = 0;
		//int cell_height_ = DefaultRowHeight;
		SmHeaderMode _HeaderMode = SmHeaderMode::HeaderColOnly;
		SmHeaderPriority _HeaderPriority = SmHeaderPriority::ColFirst;
		int IsInMergedCells(const int& row, const int& col);
		void DrawHeader(CBCGPGraphicsManager* pGM,  std::shared_ptr<SmCell> cell);
		void DrawCheckHeader(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		//void DrawVerticalHeader(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		
		void DrawNormalCell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawButtonBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawButtonSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawButtonNormal(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawQuoteClose(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawQuotePreClose(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawQuoteOpen(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawQuoteHigh(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawQuoteLow(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawPositionBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawPositionSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawHogaSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawHogaBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawOrderBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawOrderSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawCheck(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawTickBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawTickSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawRemainSell(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		void DrawRemainBuy(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
		//void DrawHeader(CBCGPGraphicsManager* pGM, std::shared_ptr<SmCell> cell);
	public:
		void SetAllRowHeight(const int& height);
		void OnEndEdit();
		bool Edit(CWnd* wnd, int  nRow , int nCol , CRect  rect, UINT nID , UINT  nChar );
		void RecalCells();
		//void RegisterOrderButtons();
		void RegisterOrderButtons(std::map<std::shared_ptr<DarkHorse::SmCell>, BUTTON_ID>& order_button_map);
		void SetColCellType(const int& col, const SmCellType& cell_type);
		void SetRowHeaderTitles(const std::vector<std::string>& row_titles);
		void SetColHeaderTitles(const std::vector<std::string>& col_titles);
		void SetColHeaderTitles(const std::vector<std::string>& col_titles, const std::vector< SmCellType>& cell_types);
		//void ReleaseOrderButtons();
		void ReleaseOrderButtons(const std::map<std::shared_ptr<DarkHorse::SmCell>, BUTTON_ID>& order_button_map);
		
		//void DrawHorizontalHeader(CBCGPGraphicsManager* pGM, const std::vector<std::string>& header_titles, const int& header_row);
		//void DrawVerticalHeader(CBCGPGraphicsManager* pGM, const std::vector<std::string>& header_titles, const int& header_col, const int& start_col = 0);
		// x : x coordinate, y : y coordinate
		std::pair<int, int> FindRowCol(const int& x, const int& y);
		void DrawCell(std::shared_ptr<SmCell> cell, CBCGPGraphicsManager* pGM, 
			const CBCGPBrush& back_brush, 
			const CBCGPBrush& border_brush,
			const CBCGPBrush& text_brush,
			const CBCGPTextFormat& format, 
			const bool& fill,
			const bool& draw_border, const bool& draw_text );
		std::shared_ptr<SmCell> FindCellByPos(const int& x, const int& y);
		std::shared_ptr<SmCell> FindCell(const int& row, const int& col);
		void SetCellMark(const int& row, const int& col, const bool& show_mark);
		void SetCellType(const int& row, const int& col, const SmCellType& cell_type);
		void SetCellText(const int& row, const int& col, const std::string& text);
		void SetTextHAlign(const int& row, const int& col, const int h_align);
		void SetLeftMargin(const int& row, const int& col, const int& margin);
		void SetRightMargin(const int& row, const int& col, const int& margin);
		void CreateGrids();
		//void CreatePositionGrids();
		void DrawGrid(CBCGPGraphicsManager* pGM, CRect& wnd_area);
		void DrawGrid(CGraphics* g, CRect& wnd_area);
		void DrawBorder(CGraphics* g, CRect& wnd_area, const bool& selected = false);
		void DrawBorder(CBCGPGraphicsManager* pGM, CRect& wnd_area, const bool& selected = false);
		//void DrawOrderCells(CBCGPGraphicsManager* pGM, CRect& wnd_area);
		//void DrawCells(CBCGPGraphicsManager* pGM, CRect& wnd_area);
		void DrawCells(CBCGPGraphicsManager* pGM, CRect& wnd_area, const bool& use_hor_header = false, const bool& use_ver_header = false);
		void draw_cells(CBCGPGraphicsManager* pGM, CRect& wnd_area, const bool& use_hor_header = false, const bool& use_ver_header = false);
		void draw_cells(CGraphics* g, CRect& wnd_area, const bool& use_hor_header = false, const bool& use_ver_header = false);
		void DrawSelectedCell(CBCGPGraphicsManager* pGM, const CBCGPBrush& fill_brush, const CBCGPBrush& select_brush, bool selected);
		void SetRowHeight(const int& row, const int& height);
		void SetColWidth(const int& col, const int& width);
		int RowCount() const { return _RowCount; }
		void RowCount(int val) { _RowCount = val; }
		int ColCount() const { return _ColCount; }
		void ColCount(int val) { _ColCount = val; }
		bool ShowGrid() const { return _ShowGrid; }
		void ShowGrid(bool val) { _ShowGrid = val; }
		int ColGridWidth() const { return _ColGridWidth; }
		void ColGridWidth(int val) { _ColGridWidth = val; }
		int Width() const { return _Width; }
		void Width(int val) { _Width = val; }
		int Height() const { return _Height; }
		void Height(int val) { _Height = val; }
		int RowGridHeight() const { return _RowGridLineHeight; }
		void RowGridHeight(int val) { _RowGridLineHeight = val; }
	};
}

