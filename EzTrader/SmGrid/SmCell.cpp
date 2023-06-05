#include "stdafx.h"
#include "SmCell.h"
#include "../Order/SmOrderRequest.h"
#include "../Order/SmOrder.h"
#include <cmath>

namespace DarkHorse {

	void SmCell::draw(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res)
	{
		switch (_CellType)
		{
		case SmCellType::CT_HEADER: draw_header(pGM, res); break;
		case SmCellType::CT_CHECK_HEADER: draw_check_header(pGM, res); break;
		case SmCellType::CT_ORDER_BUY: draw_order_buy(pGM, res);  break;
		case SmCellType::CT_ORDER_SELL: draw_order_sell(pGM, res);  break;
		case SmCellType::CT_SELECTED: draw_moving_rect(pGM, res); break;
		default: draw_cell_by_type(pGM, res);  break;
		}
	}

	size_t SmCell::GetOrderReqCount()
	{
		size_t sum = 0;
		for (auto it = _OrderReqMap.begin(); it != _OrderReqMap.end(); ++it) {
			sum += it->second->OrderAmount;
		}
		return sum;
	}

	size_t SmCell::GetOrderCount()
	{
		size_t sum = 0;
		for (auto it = _OrderMap.begin(); it != _OrderMap.end(); ++it) {
			sum += it->second->OrderAmount;
		}
		return sum;
	}

	void SmCell::AddOrderReq(const std::shared_ptr<SmOrderRequest>& order_req)
	{
		_OrderReqMap[order_req->RequestId] = order_req;
	}

	void SmCell::AddOrder(const std::shared_ptr<SmOrder>& order)
	{
		_OrderMap[order->Id()] = order;
	}

	void SmCell::ClearOrderReq()
	{
		_OrderReqMap.clear();
	}

	void SmCell::ClearOrder()
	{
		_OrderMap.clear();
	}

	CBCGPRect SmCell::GetCellRect()
	{
		return CBCGPRect(_X, _Y, _X + _Width, _Y + _Height);
	}

	void SmCell::set_show_position(const bool show_position)
	{
		show_position_ = show_position;
	}

	void SmCell::set_show_quote_sign(const bool show_quote_sign)
	{
		show_quote_sign_ = show_quote_sign;
	}

	

	void SmCell::draw_arrow(
		CBCGPGraphicsManager* pGM, 
		const SmOrderGridResource& res, 
		const CBCGPPoint& start_point, 
		const CBCGPPoint& end_point, 
		const double& stroke_width, 
		const int& head_width)
	{
		pGM->DrawLine(start_point, end_point, res.SelectedBrush, stroke_width);

		CBCGPPointsArray arrow_array;

		const int head_length = head_width; // , head_width = 6;

		const float dx = static_cast<float>(end_point.x - start_point.x);
		const float dy = static_cast<float>(end_point.y - start_point.y);
		const auto length = std::sqrt(dx * dx + dy * dy);

		// ux,uy is a unit vector parallel to the line.
		const auto ux = dx / length;
		const auto uy = dy / length;

		// vx,vy is a unit vector perpendicular to ux,uy
		const auto vx = -uy;
		const auto vy = ux;
		const auto half_width = 0.5f * head_width;

		const CBCGPPoint arrow[3] =
		{ 
			end_point,
			CBCGPPoint{ (end_point.x - head_length * ux + half_width * vx),
			(end_point.y - head_length * uy + half_width * vy) },
			CBCGPPoint{ (end_point.x - head_length * ux - half_width * vx),
			(end_point.y - head_length * uy - half_width * vy) }
		};
		for (int i = 0; i < 3; i++) arrow_array.Add(arrow[i]);

		pGM->FillGeometry(CBCGPPolygonGeometry(arrow_array), res.SelectedBrush);
	}

	void SmCell::draw_option(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res)
	{
		if (show_quote_sign_ && show_position_) {
			draw_position(pGM, res);
		}
		else if (show_quote_sign_) {
			draw_quote_sign(pGM, res);
		}
		else if (show_position_) {
			draw_position(pGM, res);
		}

		if (show_moving_rect_) {
			draw_moving_rect(pGM, res);
			CString msg;
			msg.Format("show_moving_rect_ row[%d], col[%d]\n", _Row, _Col);
			OutputDebugString(msg);
		}
	}

	void SmCell::draw_position(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res)
	{
		CBCGPRect rect(_X, _Y, _X + _Width, _Y + _Height);
		const int radius = 5;
		CBCGPRect rect_buy(rect.right - radius * 2, rect.top + rect.Height() / 2.0 - radius, rect.right, rect.top + rect.Height() / 2.0 + radius);
		CBCGPRect rect_sell(rect.left, rect.top + rect.Height() / 2.0 - radius, rect.left + radius * 2, rect.top + rect.Height() / 2.0 + radius);
		if (_CellType == CT_POSITION_BUY) {
			pGM->FillEllipse(rect_buy, res.BuyPositionBrush);
			pGM->DrawEllipse(rect_buy, res.TextBrush);
		}
		else {
			pGM->FillEllipse(rect_sell, res.SellPositionBrush);
			pGM->DrawEllipse(rect_sell, res.TextBrush);
		}
	}

	void SmCell::draw_quote_sign(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res)
	{
		CBCGPRect rect(_X, _Y, _X + _Width, _Y + _Height);
		const int text_width = 10;
		CBCGPRect text_rect(rect.left, rect.top, rect.left + text_width, rect.bottom);
		switch (_CellType) {
		case CT_QUOTE_HIGH: 
			pGM->DrawText("H", text_rect, res.QuoteTextFormat, res.QuoteHighBrush);
			break;
		case CT_QUOTE_LOW:
			pGM->DrawText("L", text_rect, res.QuoteTextFormat, res.QuoteLowBrush);
			break;
		case CT_QUOTE_OPEN: 
			pGM->DrawText("O", text_rect, res.QuoteTextFormat, res.QuoteOpenBrush);
			break;
		case CT_QUOTE_CLOSE:
			pGM->DrawText("C", text_rect, res.QuoteTextFormat, res.QuoteCloseBrush);
			break;
		}
	}

	void SmCell::draw_moving_rect(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res)
	{
		CBCGPRect rect(_X, _Y, _X + _Width, _Y + _Height);
		pGM->DrawRectangle(rect, res.SelectedBrush);
	}

	void SmCell::draw_header(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res)
	{
		CBCGPRect rect(_X, _Y, _X + _Width, _Y + _Height);
		pGM->FillRectangle(rect, res.HeaderBackBrush);

		CBCGPRect text_rect(_X + _LeftMargin, _Y, _X + _Width - _LeftMargin, _Y + _Height);

		if (_TextHAlign == 0) pGM->DrawText(_Text.c_str(), text_rect, res.LeftTextFormat, res.TextBrush);
		else if (_TextHAlign == 2) pGM->DrawText(_Text.c_str(), text_rect, res.RightTextFormat, res.TextBrush);
		else pGM->DrawText(_Text.c_str(), text_rect, res.TextFormat, res.TextBrush);

		pGM->DrawLine(rect.left, rect.top, rect.right, rect.top, res.HeaderButtonBrush);
		pGM->DrawLine(rect.left, rect.top, rect.left, rect.bottom, res.HeaderButtonBrush);
		pGM->DrawLine(rect.right, rect.top, rect.right, rect.bottom, res.HeaderButtonBrush);
		pGM->DrawLine(rect.left, rect.bottom, rect.right, rect.bottom, res.HeaderButtonBrush);
	}

	void SmCell::draw_check_header(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res)
	{
		CBCGPRect rect(_X, _Y, _X + _Width, _Y + _Height);

		pGM->FillRectangle(rect, res.HeaderBackBrush);
		pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.TextBrush);

		pGM->DrawLine(rect.left + rect.Width() / 4, rect.top + rect.Height() / 2.0, rect.left + rect.Width() / 2.0, rect.bottom - rect.Height() / 4.0, res.HeaderButtonBrush);
		pGM->DrawLine(rect.left + rect.Width() / 2.0, rect.bottom - rect.Height() / 4.0, rect.right - rect.Height() / 5, rect.top + rect.Height() / 3, res.HeaderButtonBrush);

		pGM->DrawLine(rect.left, rect.top, rect.right, rect.top, res.HeaderButtonBrush);
		pGM->DrawLine(rect.left, rect.top, rect.left, rect.bottom, res.HeaderButtonBrush);
		pGM->DrawLine(rect.right, rect.top, rect.right, rect.bottom, res.HeaderButtonBrush);
		pGM->DrawLine(rect.left, rect.bottom, rect.right, rect.bottom, res.HeaderButtonBrush);
	}

	void SmCell::draw_cell_by_type(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res)
	{
		CBCGPRect rect(_X, _Y, _X + _Width, _Y + _Height);
		const int mark_width = 5;
		CBCGPRect right_pos_rect(_X + _Width - mark_width, _Y, _X + _Width, _Y + _Height);
		switch (_CellType)
		{
		case SmCellType::CT_NORMAL: 
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.TextBrush);
			break;
		case SmCellType::CT_BUTTON_NORMAL: 
			pGM->DrawBeveledRectangle(rect, res.HeaderBackColor, 1, FALSE);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.TextBrush);
			break;
		case SmCellType::CT_BUTTON_BUY: 
			pGM->DrawBeveledRectangle(rect, res.BuyButtonColor, 1, FALSE);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.TextBrush);
			break;
		case SmCellType::CT_BUTTON_SELL: 
			pGM->DrawBeveledRectangle(rect, res.SellButtonColor, 1, FALSE);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.TextBrush);
			break;
		case SmCellType::CT_QUOTE_CLOSE: 
			pGM->FillRectangle(rect, res.SelectedBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.CloseTextBrush);
			break;
		case SmCellType::CT_QUOTE_PRECLOSE: 
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.CloseTextBrush);
			break;
		case SmCellType::CT_QUOTE_OPEN: 
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.CloseTextBrush);
			break;
		case SmCellType::CT_QUOTE_HIGH: 
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.BuyHogaTextBrush);

			pGM->DrawRectangle(rect, res.BuyHogaTextBrush);
			break;
		case SmCellType::CT_QUOTE_LOW: 
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.SellHogaTextBrush);

			pGM->DrawRectangle(rect, res.SellHogaTextBrush);
			break;
		case SmCellType::CT_POSITION_BUY: 
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.TextBrush);
			break;
		case SmCellType::CT_POSITION_SELL: 
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.TextBrush);
			break;
		case SmCellType::CT_HOGA_SELL: 
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.SellHogaTextBrush);
			break;
		case SmCellType::CT_HOGA_BUY: 
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.BuyHogaTextBrush);
			break;
		case SmCellType::CT_CHECK: 
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawLine(rect.left + rect.Width() / 4, rect.top + rect.Height() / 2.0, rect.left + rect.Width() / 2.0, rect.bottom - rect.Height() / 4.0, res.HeaderButtonBrush);
			pGM->DrawLine(rect.left + rect.Width() / 2.0, rect.bottom - rect.Height() / 4.0, rect.right - rect.Height() / 5, rect.top + rect.Height() / 3, res.HeaderButtonBrush);
			break;
		case SmCellType::CT_TICK_BUY: 
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.BuyHogaTextBrush);
			break;
		case SmCellType::CT_TICK_SELL: 
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.SellHogaTextBrush);
			break;
		case SmCellType::CT_REMAIN_BUY: 
			pGM->FillRectangle(rect, res.SellPositionBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.QuoteTextColor);
			break;
		case SmCellType::CT_REMAIN_SELL: 
			pGM->FillRectangle(rect, res.BuyPositionBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.QuoteTextColor);
			break;
		case SmCellType::CT_MARK_BUY:
			pGM->FillRectangle(right_pos_rect, res.QMBuyBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.QuoteTextColor);
			break;
		case SmCellType::CT_MARK_SELL:
			pGM->FillRectangle(right_pos_rect, res.QMSellBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.QuoteTextColor);
			break;
		case SmCellType::CT_MARK_HILO:
			pGM->FillRectangle(right_pos_rect, res.QMHighLowBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.QuoteTextColor);
			break;
		}

		draw_option(pGM, res);
	}

	void SmCell::draw_order_buy(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res)
	{
		CBCGPRect rect(_X, _Y, _X + _Width, _Y + _Height);
		const int text_width = 15;
		CBCGPRect text_rect(rect.right - text_width, rect.top, rect.right, rect.bottom);
		CBCGPRect line_rect(rect.left, rect.top, rect.right - text_width, rect.bottom);
		pGM->FillRectangle(rect, res.GridNormalBrush);
		pGM->DrawText(_Text.c_str(), text_rect, res.TextFormat, res.TextBrush);

		CBCGPPoint start_point(line_rect.right, line_rect.top + line_rect.Height() / 2.0);
		CBCGPPoint end_point(line_rect.left, line_rect.top + line_rect.Height() / 2.0);

		draw_arrow(pGM, res, start_point, end_point, 1.0, 6);

		draw_option(pGM, res);
	}

	void SmCell::draw_order_sell(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res)
	{
		CBCGPRect rect(_X, _Y, _X + _Width, _Y + _Height);
		const int text_width = 15;
		CBCGPRect text_rect(rect.left, rect.top, rect.left + text_width, rect.bottom);
		CBCGPRect line_rect(rect.left + text_width, rect.top, rect.right, rect.bottom);

		pGM->FillRectangle(rect, res.GridNormalBrush);
		pGM->DrawText(_Text.c_str(), text_rect, res.TextFormat, res.TextBrush);

		CBCGPPoint start_point(line_rect.left, line_rect.top + line_rect.Height() / 2.0);
		CBCGPPoint end_point(line_rect.right, line_rect.top + line_rect.Height() / 2.0);

		draw_arrow(pGM, res, start_point, end_point, 1.0, 6);

		draw_option(pGM, res);
	}

}