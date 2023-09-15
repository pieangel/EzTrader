#pragma once
namespace DarkHorse {
	enum SmMarkType {
		MT_NONE,
		MT_BUY,
		MT_SELL,
		MT_HILO
	};
	enum SmCellType {
		CT_NORMAL,
		CT_HEADER,
		CT_CHECK_HEADER,
		CT_BUTTON_NORMAL,
		CT_BUTTON_BUY,
		CT_BUTTON_SELL,
		CT_QUOTE_CLOSE,
		CT_QUOTE_PRECLOSE,
		CT_QUOTE_OPEN,
		CT_QUOTE_HIGH,
		CT_QUOTE_LOW,
		CT_POSITION_BUY,
		CT_POSITION_SELL,
		CT_ORDER_BUY,
		CT_ORDER_SELL,
		CT_ORDER_BUY_BACK,
		CT_ORDER_SELL_BACK,
		CT_HOGA_SELL,
		CT_HOGA_BUY,
		CT_CHECK,
		CT_TICK_BUY,
		CT_TICK_SELL,
		CT_REMAIN_BUY,
		CT_REMAIN_SELL,
		CT_SELECTED,
		CT_MARK_HILO,
		CT_MARK_BUY,
		CT_MARK_SELL,
		CT_ORDER_PRESENT,
		CT_ORDER_HAS_BEEN,
		CT_BUY_HOGA1,
		CT_BUY_HOGA2,
		CT_BUY_HOGA3,
		CT_BUY_HOGA4,
		CT_BUY_HOGA5,
		CT_SELL_HOGA1,
		CT_SELL_HOGA2,
		CT_SELL_HOGA3,
		CT_SELL_HOGA4,
		CT_SELL_HOGA5
	};
}