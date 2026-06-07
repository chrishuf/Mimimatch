#pragma once
#include"CheckrClr.h"

//Simple wxWindow class to draw the picked colors
//Using wxwindow instead of a derivate, since I encountered problems
//(Refresh issues)

ColorCheckerPanel::ColorCheckerPanel(wxWindow* parent)
    : wxWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(220, 180))
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &ColorCheckerPanel::OnPaint, this);
}

void ColorCheckerPanel::OnPaint(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(this);
    dc.SetBackground(*wxBLACK);
    dc.Clear();

    wxSize ez = GetClientSize();

    int cols = 6;
    int rows = 4;

    int pad = 2;


    int availW = ez.x - (cols + 1) * pad;
    int availH = ez.y - (rows + 1) * pad;

    int cellsize = std::min(availW / cols, availH / rows);
    int cellW = cellsize;
    int cellH = cellsize;

    dc.SetPen(*wxBLACK_PEN);

    for (int i = 0; i < (int)m_colors.size() && i < 24; ++i)
    {
        int r = i / cols;
        int c = i % cols;

        int x = pad + c * (cellW + pad);
        int y = pad + r * (cellH + pad);

        dc.SetBrush(wxBrush(m_colors[i]));
        dc.DrawRectangle(x, y, cellW, cellH);
    }
}
