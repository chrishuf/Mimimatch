#pragma once

#include "minimal.h"



#include <wx/scrolwin.h>
#include <wx/dcbuffer.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/cursor.h>
#include <array>
#include <algorithm>
#include <cmath>

//Point defintion for the display.
struct DPoint
{
    double x;
    double y;
};

//Event for calling the Mainframe function if something changed,
wxDEFINE_EVENT(EVT_IMAGE_PANEL_CHANGED, wxCommandEvent);


//wxScrolledWindow to be displayed inside the wxChildFrame
//does mainly have the display and mouse operations
//and all functions to work and display different zoom levels.
class ImagePanel : public wxScrolledWindow
{
public:

    std::array<DPoint, 4> m_quad;
    wxImage  m_image;

    ImagePanel(wxWindow* parent, wxEvtHandler* mainframe, const wxString& label);


    void LoadFile(const wxString& path);

private:
    wxEvtHandler* m_sink = nullptr;

    enum DragMode
    {
        Drag_None = -1,
        Drag_Handle0 = 0,
        Drag_Handle1 = 1,
        Drag_Handle2 = 2,
        Drag_Handle3 = 3,
        Drag_WholeQuad = 4
    };

    double   m_zoom;
    wxString m_label;



    bool    m_isPanningMiddle = false;
    wxPoint m_panStartMouse;
    wxPoint m_panStartUnits;

    DragMode m_dragMode = Drag_None;
    DPoint   m_lastDragImagePt{ 0.0, 0.0 };


    void NotifyFrame();
    static wxImage MakeCheckerboard(int w, int h);
    void ResetQuad();
    void UpdateVirtualSize();
    DPoint DeviceToImage(const wxPoint& pt) ;
    wxPoint ImageToLogical(const DPoint& pt) ;
    DPoint ClampToImage(DPoint p) ;
    DPoint GetQuadCenter() ;
    bool HitPoint(const DPoint& a, const DPoint& b, double tol) ;
    int HitAnyHandle(const DPoint& imgPt) ;
    void KeepQuadInsideImage();
    void MoveWholeQuad(double dx, double dy);
    void DrawQuad(wxDC& dc);
    void StopMiddlePan();
    void StopDrag();
    void OnSize(wxSizeEvent& evt);
    void OnPaint(wxPaintEvent&);
    void OnMouseWheel(wxMouseEvent& evt);
    void OnMiddleDown(wxMouseEvent& evt);
    void OnMiddleUp(wxMouseEvent&);
    void OnLeftDown(wxMouseEvent& evt);
    void OnLeftUp(wxMouseEvent&);
    void OnCaptureLost(wxMouseCaptureLostEvent&);
    void OnMouseMove(wxMouseEvent& evt);
};


//Class definton of the windows.
class MyChild : public wxMDIChildFrame
{
public:
    MyChild(wxMDIParentFrame* parent,const wxString& title,const wxPoint& pos,const wxSize& size);
    ImagePanel* m_view = nullptr;
    ImagePanel* GetViewer() const { return m_view; } //Helper function to get the view

private:

};

