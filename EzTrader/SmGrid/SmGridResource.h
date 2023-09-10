#pragma once
#include <BCGCBProInc.h>	

struct SmOrderGridResource {
	// ��� ��ư ����
	CBCGPColor HeaderButtonColor = RGB(0xff, 0xff, 0xff);
	// ��� ��� ����
	CBCGPColor HeaderBackColor = RGB(0x28, 0x28, 0x28);
	// �ŵ� ��ư ����
	//CBCGPColor SellButtonColor = RGB(0x10, 0x84, 0xF9);
	CBCGPColor SellButtonColor = RGB(0, 0, 255);
	// �ż� ��ư ����
	//CBCGPColor BuyButtonColor = RGB(0xf0, 0x33, 0x3a);
	CBCGPColor BuyButtonColor = RGB(255, 0, 0);
	// �ŵ� ȣ�� ��� ����
	CBCGPColor SellHogaBackColor = RGB(0x6c, 0x6c, 0x6c);
	// �ż� ȣ�� ��� ����
	CBCGPColor BuyHogaBackColor = RGB(0x6c, 0x6c, 0x6c);
	// �ŵ� ȣ�� �ؽ�Ʈ ����
	CBCGPColor SellHogaTextColor = RGB( 0x64, 0xf9, 0xed);
	// �ż� ȣ�� �ؽ�Ʈ ����
	CBCGPColor BuyHogaTextColor = RGB(0xff, 0xa5, 0x00);
	// ���� �ؽ�Ʈ ����
	CBCGPColor CloseTextColor = RGB(255, 255, 255);
	// ���� ��� ����
	CBCGPColor CloseBackColor = RGB(0xf0, 0x70, 0x00);
	// �Ϲ� �ؽ�Ʈ ����
	CBCGPColor TextColor = RGB(255, 255, 255);
	// �� �׸��� ����
	CBCGPColor RowGridColor = RGB(0x6c, 0x6c, 0x6c);
	// �� �׸��� ����
	CBCGPColor ColGridColor = RGB(0x6c, 0x6c, 0x6c);
	// �ü� ��� ����
	CBCGPColor QuoteBackColor = RGB(255, 0, 0);
	// �ü� �ؽ�Ʈ ����
	CBCGPColor QuoteTextColor = RGB(255, 255, 255);
	// �׸��� ��輱 ��ư ����
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


	// ��� ��ư ����
	CBCGPBrush HeaderButtonBrush = CBCGPBrush(HeaderButtonColor);
	// ��� ��� ����
	CBCGPBrush HeaderBackBrush = CBCGPBrush(HeaderBackColor);
	// �ŵ� ��ư ����
	CBCGPBrush SellButtonBrush = CBCGPBrush(SellButtonColor);
	// �ż� ��ư ����
	CBCGPBrush BuyButtonBrush = CBCGPBrush(BuyButtonColor);
	// �ŵ� ȣ�� ��� ����
	CBCGPBrush SellHogaBackBrush = CBCGPBrush(SellHogaBackColor);
	// �ż� ȣ�� ��� ����
	CBCGPBrush BuyHogaBackBrush = CBCGPBrush(BuyHogaBackColor);
	// �ŵ� ȣ�� �ؽ�Ʈ ����
	CBCGPBrush SellHogaTextBrush = CBCGPBrush(SellHogaTextColor);
	// �ż� ȣ�� �ؽ�Ʈ ����
	CBCGPBrush BuyHogaTextBrush = CBCGPBrush(BuyHogaTextColor);
	// ���� �ؽ�Ʈ ����
	CBCGPBrush CloseTextBrush = CBCGPBrush(CloseTextColor);
	// ���� ��� ����
	CBCGPBrush CloseBackBrush = CBCGPBrush(CloseBackColor);
	// �Ϲ� �ؽ�Ʈ ����
	CBCGPBrush TextBrush = CBCGPBrush(TextColor);
	// �� �׸��� ����
	CBCGPBrush RowGridBrush = CBCGPBrush(RowGridColor);
	// �� �׸��� ����
	CBCGPBrush ColGridBrush = CBCGPBrush(ColGridColor);
	// �ü� ��� ����
	CBCGPBrush QuoteBackBrush = CBCGPBrush(QuoteBackColor);
	// �ü� �ؽ�Ʈ ����
	CBCGPBrush QuoteTextBrush = CBCGPBrush(QuoteTextColor);
	// �׸��� ��輱 �귯��
	CBCGPBrush GridBorderBrush = CBCGPBrush(GridBorderColor);

	CBCGPTextFormat	TextFormat;
	CBCGPTextFormat	LeftTextFormat;
	CBCGPTextFormat	RightTextFormat;
	CBCGPTextFormat	QuoteTextFormat;
	CBCGPTextFormat	QuoteNumberFormat;
	CBCGPStrokeStyle OrderStroke;
};