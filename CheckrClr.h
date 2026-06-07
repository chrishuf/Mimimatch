#pragma once
#include <array>
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include <wx/window.h>
#include <wx/colour.h>
#include <wx/dcbuffer.h>
#include <wx/event.h>



class ColorCheckerPanel : public wxWindow
{
public:
    static constexpr int PatchCount = 24;

    ColorCheckerPanel(wxWindow* parent);

    std::array<wxColour, PatchCount> m_colors{};

private:
    void OnPaint(wxPaintEvent& event);

private:

};
