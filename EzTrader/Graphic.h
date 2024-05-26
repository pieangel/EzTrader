#pragma once
#include <afxwin.h>

class CGraphics
{
protected:
    CDC m_memDC;
    CBitmap m_bitmap;
    CBitmap* m_pOldBitmap;
    CPen m_pen;
    CPen* m_pOldPen;
    CBrush m_brush;
    CBrush* m_pOldBrush;
	CFont m_font;

public:
    CGraphics(CDC* pDC, int width, int height, COLORREF clrClear = RGB(255, 255, 255))
    {
        gpCreateDoubleBuffer(pDC, width, height);

        CRect rcFill(0, 0, width, height);
        DrawFillRectangle(clrClear, rcFill);

        m_pOldPen = nullptr;
        m_pOldBrush = nullptr;

        m_font.CreatePointFont(90, _T("±¼¸²"));
    }
    CGraphics(CWnd* pWnd, COLORREF clrClear = RGB(255, 255, 255))
    {
        CClientDC dc(pWnd);
        CRect rc;
        pWnd->GetClientRect(&rc);

        gpCreateDoubleBuffer(&dc, rc.Width(), rc.Height());

        DrawFillRectangle(clrClear, rc);

        m_pOldPen = nullptr;
        m_pOldBrush = nullptr;

        m_font.CreatePointFont(90, _T("±¼¸²"));
    }
    ~CGraphics()
    {
        gpDeleteDoubleBuffer();
    }

protected:
    void gpCreateDoubleBuffer(CDC* pDC, int width, int height)
    {
        m_memDC.CreateCompatibleDC(pDC);
        m_bitmap.CreateCompatibleBitmap(pDC, width, height);
        m_pOldBitmap = m_memDC.SelectObject(&m_bitmap);
    }
    void gpDeleteDoubleBuffer()
    {
        if (m_pOldBitmap)
        {
            m_memDC.SelectObject(m_pOldBitmap);
            m_bitmap.DeleteObject();
        }
        m_memDC.DeleteDC();
    }
    void gpBeginPen(COLORREF color, int width)
    {
        m_pen.CreatePen(PS_SOLID, width, color);
        m_pOldPen = m_memDC.SelectObject(&m_pen);
    }
    void gpEndPen()
    {
        if (m_pOldPen)
        {
            m_memDC.SelectObject(m_pOldPen);
            m_pen.DeleteObject();
            m_pOldPen = nullptr;
        }
    }
    void gpBeginBrush(COLORREF color)
    {
        m_brush.CreateSolidBrush(color);
        m_pOldBrush = m_memDC.SelectObject(&m_brush);
    }
    void gpBeginHollowBrush()
    {
        m_brush.Attach((HBRUSH)::GetStockObject(HOLLOW_BRUSH));
        m_pOldBrush = m_memDC.SelectObject(&m_brush);
    }
    void gpEndBrush()
    {
        if (m_pOldBrush)
        {
            m_memDC.SelectObject(m_pOldBrush);
            m_brush.DeleteObject();
            m_pOldBrush = nullptr;
        }
    }

public:
	void ChangeFont(int pointSize, LPCTSTR faceName)
    {
		m_font.DeleteObject();
		m_font.CreatePointFont(pointSize, faceName);
	}

	void Resize(CDC* pDC, int width, int height)
	{
		gpDeleteDoubleBuffer();
		gpCreateDoubleBuffer(pDC, width, height);
	}

	void Resize(CWnd* win)
    {
		gpDeleteDoubleBuffer();
        CClientDC dc(win);
        CRect rc;
        win->GetClientRect(&rc);

        gpCreateDoubleBuffer(&dc, rc.Width(), rc.Height());

        DrawFillRectangle(RGB(255, 255, 255), rc);

        win->Invalidate();
	}

    void Render(CDC* pDC, int width, int height)
    {
        pDC->BitBlt(0, 0, width, height, &m_memDC, 0, 0, SRCCOPY);
    }
    CDC* GetDC()
    {
        return &m_memDC;
    }
    void DrawLine(COLORREF color, int x1, int y1, int x2, int y2, int width = 1)
    {
        gpBeginPen(color, width);

        m_memDC.MoveTo(x1, y1);
        m_memDC.LineTo(x2, y2);

        gpEndPen();
    }
    void DrawRectangle(COLORREF clrPen, int x1, int y1, int x2, int y2, int borderWidth = 1)
    {
        gpBeginPen(clrPen, borderWidth);
        gpBeginHollowBrush();

        m_memDC.Rectangle(x1, y1, x2, y2);

        gpEndBrush();
        gpEndPen();
    }

    void DrawRectangle(COLORREF clrPen, CRect rect, int borderWidth = 1)
    {
        gpBeginPen(clrPen, borderWidth);
        gpBeginHollowBrush();

        m_memDC.Rectangle(rect.left, rect.top, rect.right, rect.bottom);

        gpEndBrush();
        gpEndPen();
    }

    void DrawFillRectangle(COLORREF color, int x, int y, int width, int height)
    {
        CRect rc(x, y, width, height);
        m_memDC.FillSolidRect(&rc, color);
    }
    void DrawFillRectangle(COLORREF color, CRect rc)
    {
        m_memDC.FillSolidRect(&rc, color);
    }
    void DrawEllipse(COLORREF clrPen, int x1, int y1, int x2, int y2, int borderWidth = 1)
    {
        gpBeginPen(clrPen, borderWidth);
        gpBeginHollowBrush();

        m_memDC.Ellipse(x1, y1, x2, y2);

        gpEndBrush();
        gpEndPen();
    }

    void DrawEllipse(COLORREF clrPen, CRect rect, int borderWidth = 1)
    {
        gpBeginPen(clrPen, borderWidth);
        gpBeginHollowBrush();

        m_memDC.Ellipse(rect.left, rect.top, rect.right, rect.bottom);

        gpEndBrush();
        gpEndPen();
    }

    void DrawFillEllipse(COLORREF clrPen, COLORREF clrFill, int x1, int y1, int x2, int y2, int borderWidth = 1)
    {
        gpBeginPen(clrPen, borderWidth);
        gpBeginBrush(clrFill);

        m_memDC.Ellipse(x1, y1, x2, y2);

        gpEndBrush();
        gpEndPen();
    }

    void DrawFillEllipse(COLORREF clrPen, COLORREF clrFill, CRect rect, int borderWidth = 1)
    {
        gpBeginPen(clrPen, borderWidth);
        gpBeginBrush(clrFill);

        m_memDC.Ellipse(rect.left, rect.top, rect.right, rect.bottom);

        gpEndBrush();
        gpEndPen();
    }

    void DrawText(LPCTSTR text, COLORREF color, int x, int y,
        int format = DT_SINGLELINE | DT_LEFT, CFont* pFont = nullptr)
    {
        int oldMode = m_memDC.SetBkMode(TRANSPARENT);
        COLORREF oldColor = m_memDC.SetTextColor(color);
        CFont* pOldFont = nullptr;

        if (pFont)
        {
            pOldFont = m_memDC.SelectObject(pFont);
        }

        CRect rect;
        m_memDC.DrawText(text, -1, &rect, format | DT_CALCRECT);
        rect.MoveToXY(x, y);
        m_memDC.DrawText(text, -1, &rect, format);

        if (pFont && pOldFont)
        {
            m_memDC.SelectObject(pOldFont);
        }

        m_memDC.SetTextColor(oldColor);
        m_memDC.SetBkMode(oldMode);
    }

    void DrawText(LPCTSTR text, COLORREF color, CRect rect,
        int format = DT_SINGLELINE | DT_LEFT)
    {
        int oldMode = m_memDC.SetBkMode(TRANSPARENT);
        COLORREF oldColor = m_memDC.SetTextColor(color);
        CFont* pOldFont = nullptr;

        pOldFont = m_memDC.SelectObject(&m_font);

        // Ensure that the format includes centering options
        format |= DT_CENTER | DT_VCENTER;

        // This flag ensures the text is clipped at the right/bottom edge of the rectangle
        format |= DT_SINGLELINE | DT_END_ELLIPSIS;

        m_memDC.DrawText(text, -1, &rect, format);

        if (pOldFont)
        {
            m_memDC.SelectObject(pOldFont);
        }

        m_memDC.SetTextColor(oldColor);
        m_memDC.SetBkMode(oldMode);
    }

    void DrawGradientFill(COLORREF color1, COLORREF color2, int x, int y, int width, int height, bool bVertical)
    {
        int r1 = GetRValue(color1);
        int g1 = GetGValue(color1);
        int b1 = GetBValue(color1);

        int r2 = GetRValue(color2);
        int g2 = GetGValue(color2);
        int b2 = GetBValue(color2);

        if (bVertical)
        {
            for (int i = 0; i < width; ++i)
            {
                int r = r1 + (i * (r2 - r1) / width);
                int g = g1 + (i * (g2 - g1) / width);
                int b = b1 + (i * (b2 - b1) / width);
                COLORREF color = RGB(r, g, b);
                m_memDC.FillSolidRect(x + i, y, 1, height, color);
            }
        }
        else
        {
            for (int i = 0; i < height; ++i)
            {
                int r = r1 + (i * (r2 - r1) / height);
                int g = g1 + (i * (g2 - g1) / height);
                int b = b1 + (i * (b2 - b1) / height);
                COLORREF color = RGB(r, g, b);
                m_memDC.FillSolidRect(x, y + i, width, 1, color);
            }
        }
    }

    void DrawGradientFill(COLORREF color1, COLORREF color2, CRect rect, bool bVertical)
    {
        int r1 = GetRValue(color1);
        int g1 = GetGValue(color1);
        int b1 = GetBValue(color1);

        int r2 = GetRValue(color2);
        int g2 = GetGValue(color2);
        int b2 = GetBValue(color2);

        if (bVertical)
        {
            for (int i = 0; i < rect.Width(); ++i)
            {
                int r = r1 + (i * (r2 - r1) / rect.Width());
                int g = g1 + (i * (g2 - g1) / rect.Width());
                int b = b1 + (i * (b2 - b1) / rect.Width());
                COLORREF color = RGB(r, g, b);
                m_memDC.FillSolidRect(rect.left + i, rect.top, 1, rect.Height(), color);
            }
        }
        else
        {
            for (int i = 0; i < rect.Height(); ++i)
            {
                int r = r1 + (i * (r2 - r1) / rect.Height());
                int g = g1 + (i * (g2 - g1) / rect.Height());
                int b = b1 + (i * (b2 - b1) / rect.Height());
                COLORREF color = RGB(r, g, b);
                m_memDC.FillSolidRect(rect.left, rect.top + i, rect.Width(), 1, color);
            }
        }
    }

    void DrawTriangle(COLORREF clrPen, const CPoint points[3], int borderWidth = 1)
    {
        gpBeginPen(clrPen, borderWidth);
        gpBeginHollowBrush();

        m_memDC.Polygon(points, 3);

        gpEndBrush();
        gpEndPen();
    }

    void draw_arrow
    (
        const CPoint& start_point,
        const CPoint& end_point,
        const double& stroke_width,
        const int& head_width
    )
    {
        DrawLine(RGB(0xff, 0x00, 0x00), start_point.x, start_point.y, end_point.x, end_point.y, (int)stroke_width);

        const float head_length = (float)head_width; // , head_width = 6;

        const float dx = static_cast<float>(end_point.x - start_point.x);
        const float dy = static_cast<float>(end_point.y - start_point.y);
        const float length = (sqrt)(dx * dx + dy * dy);

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

        DrawTriangle(RGB(0xff, 0x00, 0x00), arrow);
    }
};
