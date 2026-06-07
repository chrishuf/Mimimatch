#include "imagepanel.h"

//for displaying the windows containing the iamges
//Display and mouse operations.

//CHild Frame Initialisation
MyChild::MyChild(wxMDIParentFrame* parent,
    const wxString& title,
    const wxPoint& pos,
    const wxSize& size)
    : wxMDIChildFrame(parent,
        wxID_ANY,
        title,
        pos,
        size,
        wxDEFAULT_FRAME_STYLE & ~wxCLOSE_BOX)
{
    m_view = new ImagePanel(this, parent, title);

    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_view, 1, wxEXPAND);
    SetSizer(sizer);
    Layout();

    EnableCloseButton(false);
}



//wxScrolled Window Initialisation
ImagePanel::ImagePanel(wxWindow* parent, wxEvtHandler* mainframe, const wxString& label)
    : wxScrolledWindow(parent, wxID_ANY),
    m_sink(mainframe),
    m_zoom(1.0),
    m_label(label)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetScrollRate(1, 1);

    //Fill in the display image with a checkerboard if no image is loaded
    m_image = MakeCheckerboard(800, 600);
    //Reset the chart picker
    ResetQuad();
    //Setup the size of the window according to the image
    UpdateVirtualSize();

    //EVENT BINDING
    Bind(wxEVT_PAINT, &ImagePanel::OnPaint, this); //Paint event
    Bind(wxEVT_SIZE, &ImagePanel::OnSize, this); //Size event
    Bind(wxEVT_MOUSEWHEEL, &ImagePanel::OnMouseWheel, this); //Mousewheel Event

    Bind(wxEVT_LEFT_DOWN, &ImagePanel::OnLeftDown, this); //Left up and down events
    Bind(wxEVT_LEFT_UP, &ImagePanel::OnLeftUp, this);

    Bind(wxEVT_MIDDLE_DOWN, &ImagePanel::OnMiddleDown, this); //MIddle Down and up events;
    Bind(wxEVT_MIDDLE_UP, &ImagePanel::OnMiddleUp, this);

    Bind(wxEVT_MOTION, &ImagePanel::OnMouseMove, this); //mouse events
    Bind(wxEVT_MOUSE_CAPTURE_LOST, &ImagePanel::OnCaptureLost, this); //What happens if the mosue leave the window
}

//For debugging use only, loads an image using the default wxWidget function
void ImagePanel::LoadFile(const wxString& path)
{
    wxImage img;
    if (img.LoadFile(path) && img.IsOk())
    {
        m_image = img;
        m_zoom = 1.0;
        ResetQuad();
        UpdateVirtualSize();
        Scroll(0, 0);
        Refresh();
    }
}
//Notify the Main Frame if something happened.
void ImagePanel::NotifyFrame()
{
    if (!m_sink)
        return;

    wxCommandEvent evt(EVT_IMAGE_PANEL_CHANGED, GetId());
    evt.SetEventObject(this);
    evt.SetString(m_label);
    wxPostEvent(m_sink, evt);
}


//Generate the checkerboard if the image is empty;
 wxImage ImagePanel::MakeCheckerboard(int w, int h)
{
    wxImage img(w, h);
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            bool even = ((x / 24) + (y / 24)) % 2 == 0;
            img.SetRGB(x, y,
                even ? 200 : 100,
                even ? 200 : 100,
                even ? 220 : 120);
        }
    }
    return img;
}
//Resets the chart picker.
void ImagePanel::ResetQuad()
{
    const double w = static_cast<double>(m_image.GetWidth());
    const double h = static_cast<double>(m_image.GetHeight());

    m_quad[0] = { w * 0.20, h * 0.20 };
    m_quad[1] = { w * 0.80, h * 0.20 };
    m_quad[2] = { w * 0.80, h * 0.80 };
    m_quad[3] = { w * 0.20, h * 0.80 };
}

//Sets the virtual size according to the image size multiplied by the zoom level
void ImagePanel::UpdateVirtualSize()
{
    int w = std::max(1, static_cast<int>(m_image.GetWidth() * m_zoom));
    int h = std::max(1, static_cast<int>(m_image.GetHeight() * m_zoom));
    SetVirtualSize(w, h);
    SetScrollRate(1, 1);
}

//Calculates the image point from the windows position point.
DPoint ImagePanel::DeviceToImage(const wxPoint& pt) 
{
    int lx = 0;
    int ly = 0;
    //Get the absolute poistion on the virtual size
    CalcUnscrolledPosition(pt.x, pt.y, &lx, &ly);

    DPoint p;
    //divide it with the zoom level to get the position on the actual image
    p.x = static_cast<double>(lx) / m_zoom;
    p.y = static_cast<double>(ly) / m_zoom;
    return p;
}

//The location on an image to the windows postion
wxPoint ImagePanel::ImageToLogical(const DPoint& pt) 
{
    return wxPoint(
        static_cast<int>(std::lround(pt.x * m_zoom)),
        static_cast<int>(std::lround(pt.y * m_zoom))
    );
}

//clamps a position to the image boundaries.
DPoint ImagePanel::ClampToImage(DPoint p) 
{
    const double maxX = std::max(0.0, static_cast<double>(m_image.GetWidth() - 1));
    const double maxY = std::max(0.0, static_cast<double>(m_image.GetHeight() - 1));

    p.x = std::max(0.0, std::min(p.x, maxX));
    p.y = std::max(0.0, std::min(p.y, maxY));
    return p;
}

//Calculates the center of the CHart Picker for draging and returns it
DPoint ImagePanel::GetQuadCenter() 
{
    DPoint c{ 0.0, 0.0 };
    for (size_t i = 0; i < 4; ++i)
    {
        c.x += m_quad[i].x;
        c.y += m_quad[i].y;
    }
    c.x /= 4.0;
    c.y /= 4.0;
    return c;
}

//Checks if a mouse hit hits another point (Handles
bool ImagePanel::HitPoint(const DPoint& a, const DPoint& b, double tol) 
{
    return std::abs(a.x - b.x) <= tol && std::abs(a.y - b.y) <= tol;
}

//cjeclks if it hits one of the chart pickers handles or the cente and returns the value
int ImagePanel::HitAnyHandle(const DPoint& imgPt) 
{
    const double tol = 6.0 / m_zoom;

    for (int i = 0; i < 4; ++i)
    {
        if (HitPoint(imgPt, m_quad[i], tol))
            return i;
    }

    if (HitPoint(imgPt, GetQuadCenter(), tol))
        return Drag_WholeQuad;

    return Drag_None;
}

//Keeps the chart handles inside the image
void ImagePanel::KeepQuadInsideImage()
{
    double minX = m_quad[0].x;
    double maxX = m_quad[0].x;
    double minY = m_quad[0].y;
    double maxY = m_quad[0].y;

    for (size_t i = 1; i < 4; ++i)
    {
        minX = std::min(minX, m_quad[i].x);
        maxX = std::max(maxX, m_quad[i].x);
        minY = std::min(minY, m_quad[i].y);
        maxY = std::max(maxY, m_quad[i].y);
    }

    double dx = 0.0;
    double dy = 0.0;

    const double right = std::max(0.0, static_cast<double>(m_image.GetWidth() - 1));
    const double bottom = std::max(0.0, static_cast<double>(m_image.GetHeight() - 1));

    if (minX < 0.0)   dx = -minX;
    if (maxX > right) dx = right - maxX;
    if (minY < 0.0)   dy = -minY;
    if (maxY > bottom) dy = bottom - maxY;

    for (size_t i = 0; i < 4; ++i)
    {
        m_quad[i].x += dx;
        m_quad[i].y += dy;
    }

}

//if the middle of the chart is hit, it will move the whole chart picker
void ImagePanel::MoveWholeQuad(double dx, double dy)
{
    for (size_t i = 0; i < 4; ++i)
    {
        m_quad[i].x += dx;
        m_quad[i].y += dy;
    }
    KeepQuadInsideImage();
}

//draws the chart picker
void ImagePanel::DrawQuad(wxDC& dc)
{
    wxPoint pts[4];

    //Get the location of the chart on the frame
    for (int i = 0; i < 4; ++i)
        pts[i] = ImageToLogical(m_quad[i]);

    //draw the green frame of the picker
    dc.SetPen(wxPen(wxColour(0, 255, 0), 2));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawLine(pts[0], pts[1]);
    dc.DrawLine(pts[1], pts[2]);
    dc.DrawLine(pts[2], pts[3]);
    dc.DrawLine(pts[3], pts[0]);

    const int hs = 16;

    //Draw the colored handles of the chart picker according to the color
    //the macbeth charts have at that position
    dc.SetPen(wxPen(*wxBLACK, 1));
    for (int i = 0; i < 4; ++i)
    {
        if (i == 0) dc.SetBrush(wxBrush(wxColour(115, 82, 68)));
        if (i == 1) dc.SetBrush(wxBrush(wxColour(103, 189, 170)));
        if (i == 2) dc.SetBrush(wxBrush(wxColour(52, 52, 52)));
        if (i == 3) dc.SetBrush(wxBrush(wxColour(243, 243, 243)));
        dc.DrawRectangle(pts[i].x - hs / 2, pts[i].y - hs / 2, hs, hs);

    }

    //draw the center handle
    wxPoint centerPt = ImageToLogical(GetQuadCenter());
    dc.SetBrush(wxBrush(wxColour(0, 180, 255)));
    dc.DrawRectangle(centerPt.x - hs / 2, centerPt.y - hs / 2, hs, hs);
}

//stop scrollinng
void ImagePanel::StopMiddlePan()
{
    if (m_isPanningMiddle && HasCapture())
        ReleaseMouse();

    m_isPanningMiddle = false;
    SetCursor(wxNullCursor);
}

//stop dragging
void ImagePanel::StopDrag()
{
    if (m_dragMode != Drag_None && HasCapture())
        ReleaseMouse();

    m_dragMode = Drag_None;
    SetCursor(wxNullCursor);
}

//On size function
void ImagePanel::OnSize(wxSizeEvent& evt)
{
    UpdateVirtualSize();
    evt.Skip();
}

//Main paint function
void ImagePanel::OnPaint(wxPaintEvent&)
{
    wxBufferedPaintDC dc(this);
    DoPrepareDC(dc);
    dc.Clear();

    if (!m_image.IsOk())
        return;
    //Get the size of the wximage and multiply it by the zoom factor
    int w = std::max(1, static_cast<int>(m_image.GetWidth() * m_zoom));
    int h = std::max(1, static_cast<int>(m_image.GetHeight() * m_zoom));
    //generate a scaled version TODO: just scale it when te zoom factor changes
    wxImage scaled = m_image.Scale(w, h, wxIMAGE_QUALITY_BILINEAR);
    dc.DrawBitmap(wxBitmap(scaled), 0, 0, false);

    DrawQuad(dc);

    dc.SetTextForeground(*wxWHITE);
    dc.DrawText(wxString::Format("%s  [%.0f%%]", m_label, m_zoom * 100.0), 8, 8);
}

//Mousewheel event function. A bit more complicated to
//have zoom to mouse
void ImagePanel::OnMouseWheel(wxMouseEvent& evt)
{
    //Original it only worked with ctrl down, removed it, since
    //it does not make sense less intuitive
    if (!evt.ControlDown())
    {

    }

    wxPoint mousePos = evt.GetPosition();

    int startXUnits = 0;
    int startYUnits = 0;
    GetViewStart(&startXUnits, &startYUnits);

    int pxPerUnitX = 1;
    int pxPerUnitY = 1;
    GetScrollPixelsPerUnit(&pxPerUnitX, &pxPerUnitY);

    int viewX = startXUnits * pxPerUnitX;
    int viewY = startYUnits * pxPerUnitY;

    double imgX = (mousePos.x + viewX) / m_zoom;
    double imgY = (mousePos.y + viewY) / m_zoom;

    const double factor = 1.15;
    if (evt.GetWheelRotation() > 0)
        m_zoom = std::min(m_zoom * factor, 16.0);
    else
        m_zoom = std::max(m_zoom / factor, 0.05);

    UpdateVirtualSize();

    int newViewX = std::max(0, static_cast<int>(imgX * m_zoom - mousePos.x));
    int newViewY = std::max(0, static_cast<int>(imgY * m_zoom - mousePos.y));

    Scroll(newViewX / pxPerUnitX, newViewY / pxPerUnitY);
    Refresh();
}

//swith to pan behaviour once middle down event is issued
void ImagePanel::OnMiddleDown(wxMouseEvent& evt)
{
    if (m_dragMode != Drag_None)
        return;

    m_isPanningMiddle = true;
    m_panStartMouse = evt.GetPosition();
    GetViewStart(&m_panStartUnits.x, &m_panStartUnits.y);

    if (!HasCapture())
        CaptureMouse();

    SetCursor(wxCursor(wxCURSOR_HAND));
}

//stop panning
void ImagePanel::OnMiddleUp(wxMouseEvent&)
{
    StopMiddlePan();
}

//left down event checks if a handle is hit, otherwise switch
void ImagePanel::OnLeftDown(wxMouseEvent& evt)
{
    if (m_isPanningMiddle)
        return;

    //check if it hits a handle 
    DPoint imgPt = DeviceToImage(evt.GetPosition());
    int hit = HitAnyHandle(imgPt);

    //if it hits a point store it and go into drag mode
    if (hit != Drag_None)
    {
        m_dragMode = static_cast<DragMode>(hit);
        m_lastDragImagePt = imgPt;

        if (!HasCapture())
            CaptureMouse();

        SetCursor(wxCursor(wxCURSOR_SIZING));
    }
    else
    {
        evt.Skip();
    }
}

//Stop the drag mode and notify the event so the mainframe updates the
//chart picking position
void ImagePanel::OnLeftUp(wxMouseEvent&)
{
    StopDrag();
    NotifyFrame();
}

//mouse went out of framestop drag and pan mode
void ImagePanel::OnCaptureLost(wxMouseCaptureLostEvent&)
{
    m_isPanningMiddle = false;
    m_dragMode = Drag_None;
    SetCursor(wxNullCursor);
}

//Depending on the mode store and update the chart picker or scroll the window
void ImagePanel::OnMouseMove(wxMouseEvent& evt)
{
    //drag point or whole quad
    if (m_dragMode != Drag_None && evt.LeftIsDown())
    {
        DPoint imgPt = DeviceToImage(evt.GetPosition());

        if (m_dragMode == Drag_WholeQuad)
        {
            double dx = imgPt.x - m_lastDragImagePt.x;
            double dy = imgPt.y - m_lastDragImagePt.y;
            MoveWholeQuad(dx, dy);
            m_lastDragImagePt = imgPt;
        }
        else
        {
            m_quad[static_cast<int>(m_dragMode)] = ClampToImage(imgPt);
        }

        Refresh();
        return;
    }

    //middle mouse pan mode
    if (m_isPanningMiddle && evt.MiddleIsDown())
    {
        int pxPerUnitX = 1;
        int pxPerUnitY = 1;
        GetScrollPixelsPerUnit(&pxPerUnitX, &pxPerUnitY);

        wxPoint delta = evt.GetPosition() - m_panStartMouse;

        int newUnitsX = m_panStartUnits.x - delta.x / pxPerUnitX;
        int newUnitsY = m_panStartUnits.y - delta.y / pxPerUnitY;

        Scroll(std::max(0, newUnitsX), std::max(0, newUnitsY));
        return;
    }

    DPoint imgPt = DeviceToImage(evt.GetPosition());
    if (HitAnyHandle(imgPt) != Drag_None)
        SetCursor(wxCursor(wxCURSOR_SIZING));
    else
        SetCursor(wxNullCursor);

    evt.Skip();
}
