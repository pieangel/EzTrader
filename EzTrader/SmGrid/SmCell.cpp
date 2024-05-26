#include "stdafx.h"
#include "SmCell.h"
#include "../Order/SmOrderRequest.h"
#include "../Order/SmOrder.h"
#include <cmath>
#include "../Log/MyLogger.h"
#include "../Graphic.h"
#include <map>

namespace DarkHorse {

	// Define the static cell type map at file scope
	static const std::map<SmCellType, std::pair<COLORREF, bool>> cellTypeMap = {
		{SmCellType::CT_NORMAL, {RGB(0xff, 0xff, 0xff), false}},
		{SmCellType::CT_ORDER_PRESENT, {RGB(139, 0, 0), false}},
		{SmCellType::CT_ORDER_HAS_BEEN, {RGB(0, 100, 0), false}},
		{SmCellType::CT_BUTTON_NORMAL, {RGB(0xd2, 0xe0, 0xed), false}},
		{SmCellType::CT_BUTTON_BUY, {RGB(0xf0, 0x33, 0x3a), false}},
		{SmCellType::CT_BUTTON_SELL, {RGB(0x13, 0x89, 0xf8), false}},
		{SmCellType::CT_ORDER_BUY_BACK, {RGB(0xfc, 0xe3, 0xe4), false}},
		{SmCellType::CT_ORDER_SELL_BACK, {RGB(0xda, 0xe2, 0xf5), false}},
		{SmCellType::CT_QUOTE_CLOSE, {RGB(0xff, 0xff, 0x00), false}},
		{SmCellType::CT_QUOTE_PRECLOSE, {RGB(0xff, 0xff, 0xff), false}},
		{SmCellType::CT_QUOTE_OPEN, {RGB(0xff, 0xff, 0xff), true}},
		{SmCellType::CT_QUOTE_HIGH, {RGB(0xff, 0xff, 0xff), true}},
		{SmCellType::CT_QUOTE_LOW, {RGB(0x13, 0x89, 0xf8), true}},
		{SmCellType::CT_POSITION_BUY, {RGB(255, 255, 255), true}},
		{SmCellType::CT_POSITION_SELL, {RGB(0x13, 0x89, 0xf8), true}},
		{SmCellType::CT_HOGA_SELL, {RGB(255, 255, 255), false}},
		{SmCellType::CT_SELL_HOGA1, {RGB(0x9f, 0xd6, 0xff), false}},
		{SmCellType::CT_SELL_HOGA2, {RGB(0xb9, 0xe4, 0xff), false}},
		{SmCellType::CT_SELL_HOGA3, {RGB(0xcc, 0xe6, 0xfa), false}},
		{SmCellType::CT_SELL_HOGA4, {RGB(0xdd, 0xf3, 0xff), false}},
		{SmCellType::CT_SELL_HOGA5, {RGB(0xe6, 0xf7, 0xff), false}},
		{SmCellType::CT_HOGA_BUY, {RGB(255, 255, 255), false}},
		{SmCellType::CT_BUY_HOGA1, {RGB(0xfd, 0xad, 0xb0), false}},
		{SmCellType::CT_BUY_HOGA2, {RGB(0xff, 0xc4, 0xc7), false}},
		{SmCellType::CT_BUY_HOGA3, {RGB(0xff, 0xd6, 0xd4), false}},
		{SmCellType::CT_BUY_HOGA4, {RGB(0xff, 0xe0, 0xe1), false}},
		{SmCellType::CT_BUY_HOGA5, {RGB(0xff, 0xe8, 0xe8), false}},
		{SmCellType::CT_CHECK, {RGB(255, 255, 255), false}}, // Assuming default color
		{SmCellType::CT_TICK_BUY, {RGB(255, 255, 255), false}},
		{SmCellType::CT_TICK_SELL, {RGB(255, 255, 255), false}},
		{SmCellType::CT_REMAIN_BUY, {RGB(0x13, 0x89, 0xf8), false}},
		{SmCellType::CT_REMAIN_SELL, {RGB(255, 255, 255), false}},
		{SmCellType::CT_MARK_BUY, {RGB(0xfc, 0xe2, 0xe4), false}},
		{SmCellType::CT_MARK_SELL, {RGB(0xda, 0xe2, 0xf5), false}},
		{SmCellType::CT_MARK_HILO, {RGB(0xff, 0xff, 0xff), false}},
		{SmCellType::CT_MARK_NORMAL, {RGB(0xf2, 0xf2, 0xf2), false}},
		{SmCellType::CT_OE, {RGB(212, 186, 188), false}},
		{SmCellType::CT_PE, {RGB(255, 255, 255), false}},
		{SmCellType::CT_EE, {RGB(255, 255, 255), false}},
		{SmCellType::CT_CD, {RGB(252, 226, 228), false}},
		{SmCellType::CT_PD, {RGB(218, 226, 245), false}},
		{SmCellType::CT_DEFAULT, {RGB(255, 255, 255), false}},
		{SmCellType::CT_SP_PROFIT, {RGB(255, 255, 255), false}},
		{SmCellType::CT_SP_LOSS, {RGB(255, 255, 255), false}},
		{SmCellType::CT_FUTURE, {RGB(0xd2, 0xe0, 0xed), false}},
		{SmCellType::CT_OP_HEADER_CALL, {RGB(240, 51, 58), false}},
		{SmCellType::CT_OP_HEADER_PUT, {RGB(19, 137, 255), false}},
		{SmCellType::CT_OP_HEADER_CENTER, {RGB(0, 0, 0), false}}
	};


	void SmCell::draw(CBCGPGraphicsManager* pGM, const SmOrderGridResource& res)
	{
		switch (_CellType)
		{
		case SmCellType::CT_ORDER_BUY_BACK: draw_cell_by_type(pGM, res);  break;
		case SmCellType::CT_ORDER_SELL_BACK: draw_cell_by_type(pGM, res);  break;
		case SmCellType::CT_HEADER: draw_header(pGM, res); break;
		case SmCellType::CT_CHECK_HEADER: draw_check_header(pGM, res); break;
		case SmCellType::CT_ORDER_BUY: draw_order_buy(pGM, res);  break;
		case SmCellType::CT_ORDER_SELL: draw_order_sell(pGM, res);  break;
		case SmCellType::CT_SELECTED: draw_moving_rect(pGM, res); break;
		default: draw_cell_by_type(pGM, res);  break;
		}
	}

	void SmCell::draw(CGraphics* g)
	{
		switch (_CellType)
		{
		case SmCellType::CT_ORDER_BUY_BACK: draw_cell_by_type(g);  break;
		case SmCellType::CT_ORDER_SELL_BACK: draw_cell_by_type(g);  break;
		case SmCellType::CT_HEADER: draw_header(g); break;
		case SmCellType::CT_CHECK_HEADER: draw_check_header(g); break;
		case SmCellType::CT_ORDER_BUY: draw_order_buy(g);  break;
		case SmCellType::CT_ORDER_SELL: draw_order_sell(g);  break;
		case SmCellType::CT_SELECTED: draw_moving_rect(g); break;
		default: draw_cell_by_type(g);  break;
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
			//CString msg;
			//msg.Format("show_moving_rect_ row[%d], col[%d]\n", _Row, _Col);
			//OutputDebugString(msg);
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
			//pGM->DrawText("", text_rect, res.QuoteTextFormat, res.QuoteCloseBrush);
			break;
		case CT_QUOTE_PRECLOSE:
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
		const int mark_width = 3;
		CBCGPRect right_pos_rect(_X + _Width - mark_width, _Y, _X + _Width, _Y + _Height);
		CBCGPRect left_pos_rect(_X, _Y, _X + mark_width, _Y + _Height);
		switch (_CellType)
		{
		case SmCellType::CT_NORMAL:
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.TextBrush);
			break;
		case SmCellType::CT_ORDER_PRESENT:
			pGM->FillRectangle(rect, res.OrderBuyPresentBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.TextBrush);
			break;
		case SmCellType::CT_ORDER_HAS_BEEN:
			pGM->FillRectangle(rect, res.OrderHasBennBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.TextBrush);
			break;
		case SmCellType::CT_BUTTON_NORMAL:
			pGM->DrawBeveledRectangle(rect, res.HeaderBackColor, 1, FALSE);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.TextBrush);
			break;
		case SmCellType::CT_BUTTON_BUY:
			pGM->DrawBeveledRectangle(rect, res.BuyButtonColor, 1, FALSE);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.OrderButtonTextColor);
			break;
		case SmCellType::CT_BUTTON_SELL:
			pGM->DrawBeveledRectangle(rect, res.SellButtonColor, 1, FALSE);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.OrderButtonTextColor);
			break;
		case SmCellType::CT_ORDER_BUY_BACK:
			pGM->FillRectangle(rect, res.BuyOrderBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.QuoteTextBrush);
			break;
		case SmCellType::CT_ORDER_SELL_BACK:
			pGM->FillRectangle(rect, res.SellOrderBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.QuoteTextBrush);
			break;
		case SmCellType::CT_QUOTE_CLOSE:
			pGM->FillRectangle(rect, res.QuotCloseBackBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.CloseTextBrush);
			break;
		case SmCellType::CT_QUOTE_PRECLOSE:
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.CloseTextBrush);
			break;
		case SmCellType::CT_QUOTE_OPEN:
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.CloseTextBrush);

			pGM->DrawRectangle(rect, res.QuoteTextBrush);
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
			pGM->DrawRectangle(rect, res.BuyPositionBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.TextBrush);
			break;
		case SmCellType::CT_POSITION_SELL:
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawRectangle(rect, res.SellPositionBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.TextBrush);
			break;
		case SmCellType::CT_HOGA_SELL:
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.SellHogaTextBrush);
			break;
		case SmCellType::CT_SELL_HOGA1:
			pGM->FillRectangle(rect, res.SellHogaBrush1);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.SellHogaTextBrush);
			break;
		case SmCellType::CT_SELL_HOGA2:
			pGM->FillRectangle(rect, res.SellHogaBrush2);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.SellHogaTextBrush);
			break;
		case SmCellType::CT_SELL_HOGA3:
			pGM->FillRectangle(rect, res.SellHogaBrush3);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.SellHogaTextBrush);
			break;
		case SmCellType::CT_SELL_HOGA4:
			pGM->FillRectangle(rect, res.SellHogaBrush4);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.SellHogaTextBrush);
			break;
		case SmCellType::CT_SELL_HOGA5:
			pGM->FillRectangle(rect, res.SellHogaBrush5);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.SellHogaTextBrush);
			break;
		case SmCellType::CT_HOGA_BUY:
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.BuyHogaTextBrush);
			break;
		case SmCellType::CT_BUY_HOGA1:
			pGM->FillRectangle(rect, res.BuyHogaBrush1);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.BuyHogaTextBrush);
			break;
		case SmCellType::CT_BUY_HOGA2:
			pGM->FillRectangle(rect, res.BuyHogaBrush2);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.BuyHogaTextBrush);
			break;
		case SmCellType::CT_BUY_HOGA3:
			pGM->FillRectangle(rect, res.BuyHogaBrush3);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.BuyHogaTextBrush);
			break;
		case SmCellType::CT_BUY_HOGA4:
			pGM->FillRectangle(rect, res.BuyHogaBrush4);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.BuyHogaTextBrush);
			break;
		case SmCellType::CT_BUY_HOGA5:
			pGM->FillRectangle(rect, res.BuyHogaBrush5);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.BuyHogaTextBrush);
			break;
		case SmCellType::CT_CHECK:
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawLine(rect.left + rect.Width() / 4, rect.top + rect.Height() / 2.0, rect.left + rect.Width() / 2.0, rect.bottom - rect.Height() / 4.0, res.HeaderButtonBrush);
			pGM->DrawLine(rect.left + rect.Width() / 2.0, rect.bottom - rect.Height() / 4.0, rect.right - rect.Height() / 5, rect.top + rect.Height() / 3, res.HeaderButtonBrush);
			break;
		case SmCellType::CT_TICK_BUY:
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.PositionBuyBrush);
			break;
		case SmCellType::CT_TICK_SELL:
			pGM->FillRectangle(rect, res.GridNormalBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.PositionSellBrush);
			break;
		case SmCellType::CT_REMAIN_BUY:
			pGM->FillRectangle(rect, res.DefaultBackBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.BuyPositionBrush);
			break;
		case SmCellType::CT_REMAIN_SELL:
			pGM->FillRectangle(rect, res.DefaultBackBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.SellPositionBrush);
			break;
		case SmCellType::CT_MARK_BUY:
			//pGM->FillRectangle(left_pos_rect, res.QMBuyBrush);
			//pGM->FillRectangle(right_pos_rect, res.QMBuyBrush);
			pGM->FillRectangle(rect, res.BuySiseBackBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.QuoteTextColor);
			break;
		case SmCellType::CT_MARK_SELL:
			//pGM->FillRectangle(left_pos_rect, res.QMSellBrush);
			//pGM->FillRectangle(right_pos_rect, res.QMSellBrush);
			pGM->FillRectangle(rect, res.SellSiseBackBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.QuoteTextColor);
			break;
		case SmCellType::CT_MARK_HILO:
			//pGM->FillRectangle(left_pos_rect, res.QMHighLowBrush);
			//pGM->FillRectangle(right_pos_rect, res.QMHighLowBrush);
			pGM->FillRectangle(rect, res.HiLoSiseBackBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.QuoteTextColor);
			break;
		case SmCellType::CT_MARK_NORMAL:
			pGM->FillRectangle(rect, res.NormalMarkBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.QuoteTextColor);
			break;
		case SmCellType::CT_OE:
			pGM->FillRectangle(rect, res.OrderExistBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.QuoteTextColor);
			break;
		case SmCellType::CT_PE:
			pGM->FillRectangle(rect, res.PositionExistBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.QuoteTextColor);
			break;
		case SmCellType::CT_EE:
			pGM->FillRectangle(rect, res.ExperienceExistBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.QuoteTextColor);
			break;
		case SmCellType::CT_CD:
			pGM->FillRectangle(rect, res.CallDefaultBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.QuoteTextColor);
			break;
		case SmCellType::CT_PD:
			pGM->FillRectangle(rect, res.PutDefaultBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.QuoteTextColor);
			break;
		case SmCellType::CT_DEFAULT:
			pGM->FillRectangle(rect, res.DefaultBackBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.DefaultTextColor);
			break;
		case SmCellType::CT_SP_PROFIT:
			pGM->FillRectangle(rect, res.DefaultBackBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.SymbolPositionProfit);
			break;
		case SmCellType::CT_SP_LOSS:
			pGM->FillRectangle(rect, res.DefaultBackBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.SymbolPositionLoss);
			break;
		case SmCellType::CT_FUTURE:
			pGM->FillRectangle(rect, res.HeaderBackBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.DefaultTextColor);
			break;
		case SmCellType::CT_OP_HEADER_CALL:
			pGM->FillRectangle(rect, res.OptionHeadCallBackBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.OptionHeadTextColosr);
			break;
		case SmCellType::CT_OP_HEADER_PUT:
			pGM->FillRectangle(rect, res.OptionHeadPutBackBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.OptionHeadTextColosr);
			break;
		case SmCellType::CT_OP_HEADER_CENTER:
			pGM->FillRectangle(rect, res.OptionHeadCenterBackBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.OptionHeadTextColosr);
			break;
		default:
			pGM->FillRectangle(rect, res.DefaultBackBrush);
			pGM->DrawText(_Text.c_str(), rect, res.TextFormat, res.DefaultTextColor);
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
		pGM->FillRectangle(rect, res.BuyOrderBrush);
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

		pGM->FillRectangle(rect, res.SellOrderBrush);
		pGM->DrawText(_Text.c_str(), text_rect, res.TextFormat, res.TextBrush);

		CBCGPPoint start_point(line_rect.left, line_rect.top + line_rect.Height() / 2.0);
		CBCGPPoint end_point(line_rect.right, line_rect.top + line_rect.Height() / 2.0);

		draw_arrow(pGM, res, start_point, end_point, 1.0, 6);

		draw_option(pGM, res);
	}

	void SmCell::draw_moving_rect(CGraphics* g)
	{
		CRect rect(_X, _Y, _X + _Width, _Y + _Height);
		g->DrawRectangle(RGB(0xff, 0x00, 0x00), rect);
	}
	void SmCell::draw_header(CGraphics* g)
	{
		CRect rect(_X, _Y, _X + _Width, _Y + _Height);
		//pGM->FillRectangle(rect, res.HeaderBackBrush);
		g->DrawFillRectangle(RGB(0xd2, 0xe0, 0xed), rect);

		CBCGPRect text_rect(_X + _LeftMargin, _Y, _X + _Width - _LeftMargin, _Y + _Height);

		if (_TextHAlign == 0) //pGM->DrawText(_Text.c_str(), text_rect, res.LeftTextFormat, res.TextBrush);
			g->DrawText(_Text.c_str(), RGB(0, 0, 0), rect, DT_LEFT | DT_VCENTER);
		else if (_TextHAlign == 2) g->DrawText(_Text.c_str(), RGB(0, 0, 0), rect, DT_CENTER | DT_VCENTER);
		else g->DrawText(_Text.c_str(), RGB(0, 0, 0), rect, DT_RIGHT | DT_VCENTER);


		g->DrawLine(RGB(0x6c, 0x6c, 0x6c), rect.left, rect.top, rect.right, rect.top);
		g->DrawLine(RGB(0x6c, 0x6c, 0x6c), rect.left, rect.top, rect.left, rect.bottom);
		g->DrawLine(RGB(0x6c, 0x6c, 0x6c), rect.right, rect.top, rect.right, rect.bottom);
		g->DrawLine(RGB(0x6c, 0x6c, 0x6c), rect.left, rect.bottom, rect.right, rect.bottom);
	}
	void SmCell::draw_check_header(CGraphics* g)
	{

	}
	
	void SmCell::draw_cell_by_type(CGraphics* g)
	{
		CRect rect(_X, _Y, _X + _Width, _Y + _Height);
		const int mark_width = 3;
		CRect right_pos_rect(_X + _Width - mark_width, _Y, _X + _Width, _Y + _Height);
		CRect left_pos_rect(_X, _Y, _X + mark_width, _Y + _Height);
		int format = DT_CENTER | DT_VCENTER;

		auto it = cellTypeMap.find(_CellType);
		if (it != cellTypeMap.end())
		{
			const auto& [fillColor, drawOutline] = it->second;
			g->DrawFillRectangle(fillColor, rect);
			switch (_CellType) {
			case SmCellType::CT_BUTTON_SELL:
			case SmCellType::CT_BUTTON_BUY:
			//case SmCellType::CT_BUTTON_NORMAL:
				g->DrawText(_Text.c_str(), RGB(255, 255, 255), rect, format);
				break;
			default:
				g->DrawText(_Text.c_str(), RGB(0, 0, 0), rect, format);
				break;
			}
			if (drawOutline)
			{
				g->DrawRectangle(RGB(0, 0, 0), rect);
			}
		}
		else
		{
			// Default behavior if _CellType is not found in the map
			g->DrawFillRectangle(RGB(255, 255, 255), rect);
			g->DrawText(_Text.c_str(), RGB(0, 0, 0), rect, format);
		}

		draw_option(g);
	}

	void SmCell::draw_order_buy(CGraphics* g)
	{
		CRect rect(_X, _Y, _X + _Width, _Y + _Height);
		const int text_width = 15;
		CRect text_rect(rect.right - text_width, rect.top, rect.right, rect.bottom);
		CRect line_rect(rect.left, rect.top, rect.right - text_width, rect.bottom);
		int format = DT_CENTER | DT_VCENTER;
		g->DrawFillRectangle(RGB(0xfc, 0xe3, 0xe4), rect);
		g->DrawText(_Text.c_str(), RGB(0, 0, 0), rect, format);

		CPoint start_point(line_rect.right, line_rect.top + line_rect.Height() / 2.0);
		CPoint end_point(line_rect.left, line_rect.top + line_rect.Height() / 2.0);

		g->draw_arrow(start_point, end_point, 1.0, 6);

		draw_option(g);
	}
	void SmCell::draw_order_sell(CGraphics* g)
	{
		CRect rect(_X, _Y, _X + _Width, _Y + _Height);
		const int text_width = 15;
		CRect text_rect(rect.left, rect.top, rect.left + text_width, rect.bottom);
		CRect line_rect(rect.left + text_width, rect.top, rect.right, rect.bottom);

		//pGM->FillRectangle(rect, res.SellOrderBrush);
		//pGM->DrawText(_Text.c_str(), text_rect, res.TextFormat, res.TextBrush);
		int format = DT_CENTER | DT_VCENTER;
		g->DrawFillRectangle(RGB(0xda, 0xe2, 0xf5), rect);
		g->DrawText(_Text.c_str(), RGB(0, 0, 0), rect, format);

		CBCGPPoint start_point(line_rect.left, line_rect.top + line_rect.Height() / 2.0);
		CBCGPPoint end_point(line_rect.right, line_rect.top + line_rect.Height() / 2.0);

		g->draw_arrow(start_point, end_point, 1.0, 6);

		draw_option(g);
	}

	void SmCell::draw_arrow
	(
		CGraphics* g,
		const CPoint& start_point,
		const CPoint& end_point,
		const double& stroke_width,
		const int& head_width
	)
	{
		g->DrawLine(RGB(0xff, 0x00, 0x00), start_point.x, start_point.y, end_point.x, end_point.y, stroke_width);

		const float head_length = head_width; // , head_width = 6;

		const float dx = static_cast<float>(end_point.x - start_point.x);
		const float dy = static_cast<float>(end_point.y - start_point.y);
		const float length = std::sqrt(dx * dx + dy * dy);

		// ux,uy is a unit vector parallel to the line.
		const float ux = dx / length;
		const float uy = dy / length;

		// vx,vy is a unit vector perpendicular to ux,uy
		const float vx = -uy;
		const float vy = ux;
		const float half_width = 0.5f * head_width;

		const CPoint arrow[3] =
		{
			end_point,
			CPoint{ (int)(end_point.x - head_length * ux + half_width * vx),
			(int)(end_point.y - head_length * uy + half_width * vy) },
			CPoint{ (int)(end_point.x - head_length * ux - half_width * vx),
			(int)(end_point.y - head_length * uy - half_width * vy) }
		};
		
		g->DrawTriangle(RGB(0xff, 0x00, 0x00), arrow);
	}

	void SmCell::draw_option(CGraphics* g)
	{
		if (show_quote_sign_ && show_position_) {
			draw_position(g);
		}
		else if (show_quote_sign_) {
			draw_quote_sign(g);
		}
		else if (show_position_) {
			draw_position(g);
		}

		if (show_moving_rect_) {
			draw_moving_rect(g);
			//CString msg;
			//msg.Format("show_moving_rect_ row[%d], col[%d]\n", _Row, _Col);
			//OutputDebugString(msg);
		}
	}

	void SmCell::draw_position(CGraphics* g)
	{
		CRect rect(_X, _Y, _X + _Width, _Y + _Height);
		const int radius = 5;
		CRect rect_buy(rect.right - radius * 2, rect.top + rect.Height() / 2.0 - radius, rect.right, rect.top + rect.Height() / 2.0 + radius);
		CRect rect_sell(rect.left, rect.top + rect.Height() / 2.0 - radius, rect.left + radius * 2, rect.top + rect.Height() / 2.0 + radius);
		if (_CellType == CT_POSITION_BUY) {
			g->DrawFillEllipse(RGB(0, 0, 0), RGB(255, 0, 0), rect_buy);
		}
		else {
			g->DrawFillEllipse(RGB(0, 0, 0), RGB(0, 0, 255), rect_sell);
		}
	}
	void SmCell::draw_quote_sign(CGraphics* g)
	{
		CRect rect(_X, _Y, _X + _Width, _Y + _Height);
		const int text_width = 10;
		CRect text_rect(rect.left, rect.top, rect.left + text_width, rect.bottom);
		int format = DT_CENTER | DT_VCENTER;
		switch (_CellType) {
		case CT_QUOTE_HIGH:
			g->DrawText("H", RGB(255, 0, 0), text_rect,  format);
			break;
		case CT_QUOTE_LOW:
			g->DrawText("L", RGB(0, 0, 255), text_rect, format);
			break;
		case CT_QUOTE_OPEN:
			g->DrawText("O", RGB(0x6c, 0x6c, 0x6c), text_rect,  format);
			break;
		case CT_QUOTE_CLOSE:
			//g->DrawText("V", RGB(0, 0, 0), text_rect, format);
			break;
		case CT_QUOTE_PRECLOSE:
			g->DrawText("C", RGB(0, 0, 0), text_rect,  format);
			break;
		}
	}
}