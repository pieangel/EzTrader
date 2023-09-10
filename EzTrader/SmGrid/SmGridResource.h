#pragma once
#include <BCGCBProInc.h>	

struct SmOrderGridResource {
	// 헤더 버튼 색상
	CBCGPColor HeaderButtonColor = RGB(0xff, 0xff, 0xff);
	// 헤더 배경 색상
	CBCGPColor HeaderBackColor = RGB(0x28, 0x28, 0x28);
	// 매도 버튼 색상
	//CBCGPColor SellButtonColor = RGB(0x10, 0x84, 0xF9);
	CBCGPColor SellButtonColor = RGB(0, 0, 255);
	// 매수 버튼 색상
	//CBCGPColor BuyButtonColor = RGB(0xf0, 0x33, 0x3a);
	CBCGPColor BuyButtonColor = RGB(255, 0, 0);
	// 매도 호가 배경 색상
	CBCGPColor SellHogaBackColor = RGB(0x6c, 0x6c, 0x6c);
	// 매수 호가 배경 색상
	CBCGPColor BuyHogaBackColor = RGB(0x6c, 0x6c, 0x6c);
	// 매도 호가 텍스트 색상
	CBCGPColor SellHogaTextColor = RGB( 0x64, 0xf9, 0xed);
	// 매수 호가 텍스트 색상
	CBCGPColor BuyHogaTextColor = RGB(0xff, 0xa5, 0x00);
	// 종가 텍스트 색상
	CBCGPColor CloseTextColor = RGB(255, 255, 255);
	// 종가 배경 색상
	CBCGPColor CloseBackColor = RGB(0xf0, 0x70, 0x00);
	// 일반 텍스트 색상
	CBCGPColor TextColor = RGB(255, 255, 255);
	// 행 그리드 색상
	CBCGPColor RowGridColor = RGB(0x6c, 0x6c, 0x6c);
	// 열 그리드 색상
	CBCGPColor ColGridColor = RGB(0x6c, 0x6c, 0x6c);
	// 시세 배경 색상
	CBCGPColor QuoteBackColor = RGB(255, 0, 0);
	// 시세 텍스트 색상
	CBCGPColor QuoteTextColor = RGB(255, 255, 255);
	// 그리드 경계선 버튼 색상
	CBCGPColor GridBorderColor = RGB(255, 255, 255);

	CBCGPColor OrderBuyPresentColor = RGB(139, 0, 0);
	CBCGPColor OrderSellPresentColor = RGB(0, 0, 128);
	CBCGPColor OrderPresentColor = RGB(101, 67, 33);
	CBCGPColor OrderHasBeenColor = RGB(0, 100, 0);

	CBCGPColor SelectedValueColor = RGB(255, 0, 0);

	CBCGPColor GridNormalColor_black = RGB(0x46, 0x46, 0x4b);
	CBCGPColor GridNormalColor = RGB(0xff, 0xff, 0xff);
	CBCGPColor GridNormalAlterColor = RGB(0x64, 0x64, 0x64);

	CBCGPColor SelectedColor = RGB(0xf0, 0x70, 0x00);

	CBCGPColor QuoteHighColor = RGB(255, 0, 0);
	CBCGPColor QuoteLowColor = RGB(0, 0, 255);
	CBCGPColor QuoteCloseColor = RGB(0, 0, 0);
	CBCGPColor QuotePreCloseColor = RGB(0, 0, 0);
	CBCGPColor QuoteOpenColor = RGB(0, 0, 0);

	CBCGPColor BuyTextColor = RGB(0xfe, 0x2f, 0x19);
	CBCGPColor SellPositionBackColor = RGB(0, 0, 255);
	//CBCGPColor SellPositionBackColor = RGB(0x10, 0x84, 0xF9);
	//CBCGPColor BuyPositionBackColor = RGB(0xf0, 0x33, 0x3a);
	CBCGPColor BuyPositionBackColor = RGB(255, 0, 0);


	CBCGPBrush OrderBuyPresentBrush = CBCGPBrush(OrderBuyPresentColor);
	CBCGPBrush OrderSellPresentBrush = CBCGPBrush(OrderSellPresentColor);
	CBCGPBrush OrderPresentBrush = CBCGPBrush(OrderPresentColor);
	CBCGPBrush OrderHasBennBrush = CBCGPBrush(OrderHasBeenColor);

	CBCGPBrush BuyPositionBrush = CBCGPBrush(BuyPositionBackColor);
	CBCGPBrush SellPositionBrush = CBCGPBrush(SellPositionBackColor);

	CBCGPBrush BuyTextBrush = CBCGPBrush(BuyTextColor);
	CBCGPBrush QuoteHighBrush = CBCGPBrush(QuoteHighColor);
	CBCGPBrush QuoteLowBrush = CBCGPBrush(QuoteLowColor);
	CBCGPBrush QuoteCloseBrush = CBCGPBrush(QuoteCloseColor);
	CBCGPBrush QuotePreCloseBrush = CBCGPBrush(QuotePreCloseColor);
	CBCGPBrush QuoteOpenBrush = CBCGPBrush(QuoteOpenColor);

	CBCGPBrush SelectedValueBrush = CBCGPBrush(SelectedValueColor);
	CBCGPBrush SelectedBrush = CBCGPBrush(SelectedColor);

	CBCGPBrush GridNormalBrush = CBCGPBrush(GridNormalColor);
	CBCGPBrush GridNormalAlterBrush = CBCGPBrush(GridNormalAlterColor);

	CBCGPBrush QMNormalBrush = CBCGPBrush(GridNormalColor);
	CBCGPBrush QMBuyBrush = CBCGPBrush(BuyPositionBackColor, 0.2);
	CBCGPBrush QMSellBrush = CBCGPBrush(SellPositionBackColor, 0.2);
	CBCGPBrush QMHighLowBrush = CBCGPBrush(GridBorderColor, 0.1);


	// 헤더 버튼 색상
	CBCGPBrush HeaderButtonBrush = CBCGPBrush(HeaderButtonColor);
	// 헤더 배경 색상
	CBCGPBrush HeaderBackBrush = CBCGPBrush(HeaderBackColor);
	// 매도 버튼 색상
	CBCGPBrush SellButtonBrush = CBCGPBrush(SellButtonColor);
	// 매수 버튼 색상
	CBCGPBrush BuyButtonBrush = CBCGPBrush(BuyButtonColor);
	// 매도 호가 배경 색상
	CBCGPBrush SellHogaBackBrush = CBCGPBrush(SellHogaBackColor);
	// 매수 호가 배경 색상
	CBCGPBrush BuyHogaBackBrush = CBCGPBrush(BuyHogaBackColor);
	// 매도 호가 텍스트 색상
	CBCGPBrush SellHogaTextBrush = CBCGPBrush(SellHogaTextColor);
	// 매수 호가 텍스트 색상
	CBCGPBrush BuyHogaTextBrush = CBCGPBrush(BuyHogaTextColor);
	// 종가 텍스트 색상
	CBCGPBrush CloseTextBrush = CBCGPBrush(CloseTextColor);
	// 종가 배경 색상
	CBCGPBrush CloseBackBrush = CBCGPBrush(CloseBackColor);
	// 일반 텍스트 색상
	CBCGPBrush TextBrush = CBCGPBrush(TextColor);
	// 행 그리드 색상
	CBCGPBrush RowGridBrush = CBCGPBrush(RowGridColor);
	// 열 그리드 색상
	CBCGPBrush ColGridBrush = CBCGPBrush(ColGridColor);
	// 시세 배경 색상
	CBCGPBrush QuoteBackBrush = CBCGPBrush(QuoteBackColor);
	// 시세 텍스트 색상
	CBCGPBrush QuoteTextBrush = CBCGPBrush(QuoteTextColor);
	// 그리드 경계선 브러쉬
	CBCGPBrush GridBorderBrush = CBCGPBrush(GridBorderColor);

	CBCGPTextFormat	TextFormat;
	CBCGPTextFormat	LeftTextFormat;
	CBCGPTextFormat	RightTextFormat;
	CBCGPTextFormat	QuoteTextFormat;
	CBCGPTextFormat	QuoteNumberFormat;
	CBCGPStrokeStyle OrderStroke;
};