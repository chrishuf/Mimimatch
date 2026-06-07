#include "minimal.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "../sample.xpm"
#endif


//Myapp standard functions




class MyApp : public wxApp
{
public:
    virtual bool OnInit() wxOVERRIDE;
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    FreeImage_Initialise(FALSE);

    wxInitAllImageHandlers();

    MyFrame* frame = new MyFrame("mimimatch");
    frame->Show(true);

    // Important: create MDI children after the frame is visible
   // frame->CallAfter(&MyFrame::CreateInitialChildren);
    frame->CreateInitialChildren();


    return true;
}

//EVENT TABLLE

wxBEGIN_EVENT_TABLE(MyFrame, wxMDIParentFrame)
EVT_MENU(Minimal_Quit, MyFrame::OnQuit)
EVT_MENU(Minimal_About, MyFrame::OnAbout)
EVT_MENU(ID_Window_Cascade, MyFrame::OnCascade)
EVT_MENU(ID_Window_TileH, MyFrame::OnTileHorizontal)
EVT_MENU(ID_Window_TileV, MyFrame::OnTileVertical)
EVT_MENU(ID_Window_ArrangeIcons, MyFrame::OnArrangeIcons)
EVT_MENU(ID_Window_Next, MyFrame::OnNextChild)
EVT_MENU(ID_Window_Previous, MyFrame::OnPreviousChild)
EVT_SIZE(MyFrame::OnSize)
wxEND_EVENT_TABLE()

//Mainframe Initialisation



MyFrame::MyFrame(const wxString& title)
    : wxMDIParentFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1400, 900)), m_mgr(this)
{
    SetIcon(wxICON(sample));


    m_sidePanel = new SidePanel(this);
    m_sidePanel->SetScrollRate(5, 5);

    m_mgr.AddPane(m_sidePanel,
        wxAuiPaneInfo().Name("side").Caption("Tools").Left().BestSize(250, -1));
    m_mgr.Update();


    wxMenu* fileMenu = new wxMenu;
    wxMenu* helpMenu = new wxMenu;
    wxMenu* menuWindow = new wxMenu;

    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

    menuWindow->Append(ID_Window_Cascade, "&Cascade");
    menuWindow->Append(ID_Window_TileH, "Tile &Horizontally");
    menuWindow->Append(ID_Window_TileV, "Tile &Vertically");
    menuWindow->AppendSeparator();
    menuWindow->Append(ID_Window_ArrangeIcons, "&Arrange Icons");
    menuWindow->AppendSeparator();
    menuWindow->Append(ID_Window_Next, "&Next");
    menuWindow->Append(ID_Window_Previous, "&Previous");

    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");
    SetMenuBar(menuBar);

    SetWindowMenu(menuWindow);

    CreateStatusBar(2);
    SetStatusText("Welcome to mimimatch");

    BindSidePanel();

}

MyFrame::~MyFrame()
{
    m_mgr.UnInit();
    FreeImage_DeInitialise();
}


//Bind the sidepanel events
void MyFrame::BindSidePanel()
{

    m_sidePanel->m_btnLoadPano->Bind(wxEVT_BUTTON, &MyFrame::OnLoadPano, this);
    m_sidePanel->m_btnLoadFootage->Bind(wxEVT_BUTTON, &MyFrame::OnLoadFootage, this);
    m_sidePanel->m_btnDetectPano->Bind(wxEVT_BUTTON, &MyFrame::OnAutoCheckerPano, this);
    m_sidePanel->m_btnDetectFootage->Bind(wxEVT_BUTTON, &MyFrame::OnAutoCheckerFootage, this);

    m_sidePanel->m_btnMatchPano->Bind(wxEVT_BUTTON, &MyFrame::OnMatchPano, this);
    m_sidePanel->m_btnMatchPanoInv->Bind(wxEVT_BUTTON, &MyFrame::OnMatchPanoInv, this);

    m_sidePanel->m_btnMatchFootage->Bind(wxEVT_BUTTON, &MyFrame::OnMatchFootage, this);
    m_sidePanel->m_btnMatchFootageInv->Bind(wxEVT_BUTTON, &MyFrame::OnMatchFootageInv, this);


    Bind(EVT_IMAGE_PANEL_CHANGED, &MyFrame::OnBoxChanged, this);
}


//WIndows Arrangement Fuctions

void MyFrame::OnCascade(wxCommandEvent& WXUNUSED(event))
{
    Cascade();
}

void MyFrame::OnTileHorizontal(wxCommandEvent& WXUNUSED(event))
{
    Tile(wxHORIZONTAL);
}

void MyFrame::OnTileVertical(wxCommandEvent& WXUNUSED(event))
{
    Tile(wxVERTICAL);
}

void MyFrame::OnArrangeIcons(wxCommandEvent& WXUNUSED(event))
{
    ArrangeIcons();
}

void MyFrame::OnNextChild(wxCommandEvent& WXUNUSED(event))
{
    ActivateNext();
}

void MyFrame::OnPreviousChild(wxCommandEvent& WXUNUSED(event))
{
    ActivatePrevious();
}




void MyFrame::OnSize(wxSizeEvent& event)
{
    // Important for MDI child visibility on MSW
    if (GetClientWindow())
    {
        int w, h;
        GetClientSize(&w, &h);
        GetClientWindow()->SetSize(0, 0, w, h);
    }

    event.Skip();
}

void MyFrame::CreateInitialChildren()
{
    if (mdi_pano || mdi_footage)
        return;

    mdi_pano = new MyChild(this, "Pano Cam", wxPoint(10, 10), wxSize(650, 500));
    mdi_footage = new MyChild(this, "Footage Cam", wxPoint(80, 80), wxSize(650, 500));

    mdi_pano->Show(true);
    mdi_footage->Show(true);

    Tile(wxVERTICAL);
    mdi_pano->Raise();
    mdi_footage->Raise();
}

//When Changing the Handles inside the Windows get the location of
//the points and fill the coclor checker colors
void MyFrame::OnBoxChanged(wxCommandEvent& event)
{
    //Check from which window the event is comming from
    ImagePanel* panel = dynamic_cast<ImagePanel*>(event.GetEventObject());
    if (!panel)
        return;

    MyChild* child = dynamic_cast<MyChild*>(panel->GetParent());
    if (!child)
        return;

    //IF it is from the pano window set the box values and get the chart colors for the opencv functions
    //and the display in the sidebar, otherwise checj if its the footage window
    if (child == mdi_pano)
    {
        chart_pano.box[0] = cv::Point2f(mdi_pano->m_view->m_quad[0].x, mdi_pano->m_view->m_quad[0].y); // TL
        chart_pano.box[1] = cv::Point2f(mdi_pano->m_view->m_quad[1].x, mdi_pano->m_view->m_quad[1].y); // TR
        chart_pano.box[2] = cv::Point2f(mdi_pano->m_view->m_quad[2].x, mdi_pano->m_view->m_quad[2].y); // BR
        chart_pano.box[3] = cv::Point2f(mdi_pano->m_view->m_quad[3].x, mdi_pano->m_view->m_quad[3].y); // BL

        if (!matchit.fillChartPointsFromBox(chart_pano))
        {
            wxMessageBox("Could not calculate chart points");
            return;
        }
        patch_pano = matchit.extractPatchColors(mat_pano, chart_pano.centers, 3);
        SetSidebarColors();
        


    }
    else if (child == mdi_footage)
    {
        chart_footage.box[0] = cv::Point2f(mdi_footage->m_view->m_quad[0].x, mdi_footage->m_view->m_quad[0].y); // TL
        chart_footage.box[1] = cv::Point2f(mdi_footage->m_view->m_quad[1].x, mdi_footage->m_view->m_quad[1].y); // TR
        chart_footage.box[2] = cv::Point2f(mdi_footage->m_view->m_quad[2].x, mdi_footage->m_view->m_quad[2].y); // BR
        chart_footage.box[3] = cv::Point2f(mdi_footage->m_view->m_quad[3].x, mdi_footage->m_view->m_quad[3].y); // BL

        if (!matchit.fillChartPointsFromBox(chart_footage))
        {
            wxMessageBox("Could not calculate chart points");
            return;
        }
        patch_footage = matchit.extractPatchColors(mat_footage, chart_footage.centers, 3);
        SetSidebarColors();
    }




}
//Function to set the gamma corrected wxwindos sidebar colors.
void MyFrame::SetSidebarColors()
{
    int count = std::min<int>(patch_pano.rows, ColorCheckerPanel::PatchCount);

    for (int i = 0; i < count; ++i)
    {
        cv::Vec3d p = patch_pano.at<cv::Vec3d>(i, 0);

        int r = (int)(pow(p[0], 1.0 / 2.2) * 255.0);
        int g = (int)(pow(p[1], 1.0 / 2.2) * 255.0);
        int b = (int)(pow(p[2], 1.0 / 2.2) * 255.0);

        if (r < 0) r = 0;
        if (g < 0) g = 0;
        if (b < 0) b = 0;
        if (r > 255) r = 255;
        if (g > 255) g = 255;
        if (b > 255) b = 255;



        m_sidePanel->pan_pano->m_colors[i] = wxColour(r, g, b);
    }
    m_sidePanel->pan_pano->Refresh();

    count = std::min<int>(patch_footage.rows, ColorCheckerPanel::PatchCount);

    for (int i = 0; i < count; ++i)
    {
        cv::Vec3d p = patch_footage.at<cv::Vec3d>(i, 0);

        int r = (int)(pow(p[0], 1.0 / 2.2) * 255.0);
        int g = (int)(pow(p[1], 1.0 / 2.2) * 255.0);
        int b = (int)(pow(p[2], 1.0 / 2.2) * 255.0);

        if (r < 0) r = 0;
        if (g < 0) g = 0;
        if (b < 0) b = 0;
        if (r > 255) r = 255;
        if (g > 255) g = 255;
        if (b > 255) b = 255;


        m_sidePanel->pan_footage->m_colors[i] = wxColour(r, g, b);
    }
    m_sidePanel->pan_footage->Refresh();



}
//Callback functions to load the images
void MyFrame::OnLoadPano(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog dlg(this, "Open Pano Image (Pano)", "", "",
        "All files (*.*)|*.*",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dlg.ShowModal() == wxID_OK)
    {
        LoadTheImage(dlg.GetPath(),0);
    }
}

void MyFrame::OnLoadFootage(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog dlg(this, "Open Footage Image (Footage)", "", "",
        "All files (*.*)|*.*",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dlg.ShowModal() == wxID_OK)
    {
        LoadTheImage(dlg.GetPath(), 1);
    }
}
//Exit function for the mainframe
void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}
//OPen the about dialog
void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    AboutDlg dlg(this);
    dlg.ShowModal();
}
//Use OpenCV color chart detection for the pano image, and
//fill the values.
void MyFrame::OnAutoCheckerPano(wxCommandEvent& event)
{
    
    wxString sworki;
    if (matchit.detectChart(mat_pano, chart_pano))
    {
        mdi_pano->m_view->m_quad[0].x = chart_pano.box[0].x;
        mdi_pano->m_view->m_quad[0].y = chart_pano.box[0].y;
        mdi_pano->m_view->m_quad[1].x = chart_pano.box[1].x;
        mdi_pano->m_view->m_quad[1].y = chart_pano.box[1].y;
        mdi_pano->m_view->m_quad[2].x = chart_pano.box[2].x;
        mdi_pano->m_view->m_quad[2].y = chart_pano.box[2].y;
        mdi_pano->m_view->m_quad[3].x = chart_pano.box[3].x;
        mdi_pano->m_view->m_quad[3].y = chart_pano.box[3].y;
        if (matchit.fillChartPointsFromBox(chart_pano))
        {
            patch_pano = matchit.extractPatchColors(mat_pano, chart_pano.centers, 3);
            mdi_pano->m_view->Refresh();
            SetSidebarColors();
        }


    }

}
//same for the footage camera
void MyFrame::OnAutoCheckerFootage(wxCommandEvent& event)
{
    
    if (matchit.detectChart(mat_footage, chart_footage))
    {
        mdi_footage->m_view->m_quad[0].x = chart_footage.box[0].x;
        mdi_footage->m_view->m_quad[0].y = chart_footage.box[0].y;
        mdi_footage->m_view->m_quad[1].x = chart_footage.box[1].x;
        mdi_footage->m_view->m_quad[1].y = chart_footage.box[1].y;
        mdi_footage->m_view->m_quad[2].x = chart_footage.box[2].x;
        mdi_footage->m_view->m_quad[2].y = chart_footage.box[2].y;
        mdi_footage->m_view->m_quad[3].x = chart_footage.box[3].x;
        mdi_footage->m_view->m_quad[3].y = chart_footage.box[3].y;
        if (matchit.fillChartPointsFromBox(chart_footage))
        {
            patch_footage = matchit.extractPatchColors(mat_footage, chart_footage.centers, 3);
            mdi_footage->m_view->Refresh();
            SetSidebarColors();
        }
    }

}

//Calling the match functions for the PANO and exports the result as a Fusion node
void MyFrame::OnMatchPano(wxCommandEvent& event)
{
    cv::Mat out_ccm;
    double satScale = 1.0;
    cv::Mat satMatchedPatches;
    //Get the color space we want to do the match in
    int sel = m_sidePanel->m_colorSpace->GetSelection();
    int typ = m_sidePanel->m_checktype->GetSelection();
    //Reorder patches for datacolor users
    if (typ == 1)
    {
        patch_footage = matchit.reorderPatchesDatacolor(patch_footage);
        patch_pano = matchit.reorderPatchesDatacolor(patch_pano);
    }

    //call the opencv match function.
    if (!matchit.calculateCCMAndSaturation(patch_footage,
        patch_pano,
        out_ccm,
        satScale,
        &satMatchedPatches,
        sel))
    {

   }
    //Check if the match saturation checkbox is checked and
    //export the fusion graph
    if (m_sidePanel->m_btnMatchSat->GetValue() == 1)
    {
        ExportFusionCCNode(out_ccm, satScale);
    }
    else
    {
        ExportFusionNode(out_ccm);
    }
}

//Calling the INVERSE match functions for the PANO and exports the result as a Fusion node
void MyFrame::OnMatchPanoInv(wxCommandEvent& event)
{
    cv::Mat out_ccm;
    double satScale = 1.0;
    cv::Mat satMatchedPatches;
    //Get the color space we want to do the match in
    int sel = m_sidePanel->m_colorSpace->GetSelection();
    int typ = m_sidePanel->m_checktype->GetSelection();
    //Reorder patches for datacolor users
    if (typ == 1)
    {
        patch_footage = matchit.reorderPatchesDatacolor(patch_footage);
        patch_pano = matchit.reorderPatchesDatacolor(patch_pano);
    }

    //call the opencv match function.
    if (!matchit.calculateCCMAndSaturation(patch_footage,
        patch_pano,
        out_ccm,
        satScale,
        &satMatchedPatches,
        sel))
    {

    }

    //invert the matrix
    out_ccm = matchit.invertCCM(out_ccm);
    //get the inverse of the saturation
    if (satScale != 0)
    {
        satScale = 1.0 / satScale;
    }
    else
    {
        satScale = 1.0;
    }
    //Check if the match saturation checkbox is checked and
    //export the fusion graph
    if (m_sidePanel->m_btnMatchSat->GetValue() == 1)
    {
        ExportFusionCCNode(out_ccm, satScale);
    }
    else
    {
        ExportFusionNode(out_ccm);
    }

}

//Calling the match functions for the FOOTAGE and exports the result as a Fusion node
void MyFrame::OnMatchFootage(wxCommandEvent& event)
{
    cv::Mat out_ccm;
    double satScale = 1.0;
    cv::Mat satMatchedPatches;
    //Get the color space we want to do the match in
    int sel = m_sidePanel->m_colorSpace->GetSelection();
    int typ = m_sidePanel->m_checktype->GetSelection();
    //Reorder patches for datacolor users
    if (typ == 1)
    {
        patch_footage = matchit.reorderPatchesDatacolor(patch_footage);
        patch_pano = matchit.reorderPatchesDatacolor(patch_pano);
    }

    //call the opencv match function.
    if (!matchit.calculateCCMAndSaturation(patch_pano,
        patch_footage,
        out_ccm,
        satScale,
        &satMatchedPatches,
        sel))
    {

    }
    //Check if the match saturation checkbox is checked and
    //export the fusion graph
    if (m_sidePanel->m_btnMatchSat->GetValue() == 1)
    {
        ExportFusionCCNode(out_ccm, satScale);
    }
    else
    {
        ExportFusionNode(out_ccm);
    }
}


//Calling the INVERSE match functions for the FOOTAGE and exports the result as a Fusion node
void MyFrame::OnMatchFootageInv(wxCommandEvent& event)
{
    cv::Mat out_ccm;
    double satScale = 1.0;
    cv::Mat satMatchedPatches;
    //Get the color space we want to do the match in
    int sel = m_sidePanel->m_colorSpace->GetSelection();
    int typ = m_sidePanel->m_checktype->GetSelection();
    //Reorder patches for datacolor users
    if (typ == 1)
    {
        patch_footage = matchit.reorderPatchesDatacolor(patch_footage);
        patch_pano = matchit.reorderPatchesDatacolor(patch_pano);
    }

    //call the opencv match function.
    if (!matchit.calculateCCMAndSaturation(patch_pano,
        patch_footage,
        out_ccm,
        satScale,
        &satMatchedPatches,
        sel))
    {

    }
    //invert the matrix
    out_ccm = matchit.invertCCM(out_ccm);
    //get the inverse of the saturation
    if (satScale != 0.0)
    {
        satScale = 1.0 / satScale;
    }
    else
    {
        satScale = 1.0;
    }
    //Check if the match saturation checkbox is checked and
    //export the fusion graph
    if (m_sidePanel->m_btnMatchSat->GetValue() == 1)
    {
        ExportFusionCCNode(out_ccm, satScale);
    }
    else
    {
        ExportFusionNode(out_ccm);
    }
}

//Image load function converting it to a floating point
// image and assigns the data to a wxImage for display
// and a opencv mat for the calculation
//Notice the ConvertToFloatLinearManual function:
//This avoids a bug which is present in Freeimage when
//you convert an image which has an alpha beeing clamped
void MyFrame::LoadTheImage(wxString filename,int type)
{

    //Load the Freeimage....pretty standard
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(filename, 0);
    if (fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(filename);
    if (fif == FIF_UNKNOWN)
        return ;
    if (!FreeImage_FIFSupportsReading(fif))
        return ;
    FIBITMAP* mib = FreeImage_Load(fif, filename, 0);

    int bpp = FreeImage_GetBPP(mib);

    if(type ==0)
    {
        //Delete the existing image, must be done to avoid memory leak
        FreeImage_Unload(cam_pano);
        //Convert the image for a linear floating point image
        cam_pano = ConvertToFloatLinearManual(mib,true,false,2.2);
        //unload the original freeimage we used for laoding
        FreeImage_Unload(mib);
        //Convert it to a wxImage for displaying the content
        mdi_pano->m_view->m_image = FI_ConverttowxImage(cam_pano);
        //Convert ut to a OpenCV Mat for calculation.
        mat_pano = freeImageToMat(cam_pano);
        //Refresh Window
        mdi_pano->m_view->Refresh();
    }

    if (type == 1)
    {
        //Delete the existing image, must be done to avoid memory leak
        FreeImage_Unload(cam_footage);
        //Convert the image for a linear floating point image
        cam_footage = ConvertToFloatLinearManual(mib, true, false, 2.2);
        //unload the original freeimage we used for laoding
        FreeImage_Unload(mib);
        //Convert it to a wxImage for displaying the content
        mdi_footage->m_view->m_image = FI_ConverttowxImage(cam_footage);
        //Convert ut to a OpenCV Mat for calculation.
        mat_footage = freeImageToMat(cam_footage);
        //Refresh Window
        mdi_footage->m_view->Refresh();
    }
}

//Export the fusion nodes to a clipboard
void MyFrame::ExportFusionNode(cv::Mat inccm)
{
    //Check if clipboarding works
    if (!wxTheClipboard->Open())
    {
        return;
    }

    //Convert the calculated Matrix to something which fusion understands
    cv::Mat ccm32;
    inccm.convertTo(ccm32, CV_32F);


    //Sets the the initial export matrix.
    cv::Mat fccm = (cv::Mat_<float>(5, 5) <<
        0.00f, 0.00f, 0.00f, 0.00f, 0.00f,
        0.00f, 0.00f, 0.00f, 0.00f, 0.00f,
        0.00f, 0.00f, 0.00f, 0.00f, 0.00f,
        0.00f, 0.00f, 0.00f, 0.00f, 0.00f,
        0.00f, 0.00f, 0.00f, 0.00f, 0.00f);
    //Yes I could do it in the initialisation, but for future enhancements
    //I decide to assign the values seperately
    fccm.at<float>(3, 3) = 1.0;
    fccm.at<float>(4, 4) = 1.0;
    //copy the values from the Calculated Matrix to the export matrix.
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            fccm.at<float>(r, c) = ccm32.at<float>(r, c);
        }
    }


    //Export string plus a helper string to fill in the table values
    //TODO make it as clean as the Saturation function below
    wxString clipbrd,table;

    clipbrd.Printf("{\n        Tools = ordered() {\n            MmimmatchMatrix = CustomColorMatrix{\n");

    clipbrd.Append("                CtrlWZoom = false,\n                Inputs = { \n                    Matrix = Input { \n                        Value = MatrixTable { \n                            NumEntries = 54, \n");

    clipbrd.Append("                            Type = 10, \n                            Offset = 0, \n");
    clipbrd.Append("Minimum = -10, \n                            Maximum = 10, \n                            MinimumValue = -10, \n");
    clipbrd.Append("                            MaximumValue = 10, \n                            StartSlope = 1, \n");
    clipbrd.Append("EndSlope = 1, StartIn = 0, \n                            EndIn = 1, \n");


table.Printf("                            Table = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, %f, %f, %f, %f, %f, 0, 0, %f, %f, %f, %f, %f, 0, 0, %f, %f, %f, %f, %f, 0, 0, %f, %f, %f, %f, %f, 0, 0, %f, %f, %f, %f, %f, 0, 0, 0, 0, 0, 0, 0, 1 }\n"
    ,
    fccm.at<float>(0, 0), fccm.at<float>(0, 1), fccm.at<float>(0, 2), fccm.at<float>(0, 3), fccm.at<float>(0, 4),
    fccm.at<float>(1, 0), fccm.at<float>(1, 1), fccm.at<float>(1, 2), fccm.at<float>(1, 3), fccm.at<float>(1, 4),
    fccm.at<float>(2, 0), fccm.at<float>(2, 1), fccm.at<float>(2, 2), fccm.at<float>(2, 3), fccm.at<float>(2, 4),
    fccm.at<float>(3, 0), fccm.at<float>(3, 1), fccm.at<float>(3, 2), fccm.at<float>(3, 3), fccm.at<float>(3, 4),
    fccm.at<float>(4, 0), fccm.at<float>(4, 1), fccm.at<float>(4, 2), fccm.at<float>(4, 3), fccm.at<float>(4, 4));
clipbrd.Append(table);

clipbrd.Append("                        }, \n                    }\n                }, \n                ViewInfo = OperatorInfo { Pos = { 482.667, 16.4545 } }, \n");
    clipbrd.Append("            }\n        }, \n            ActiveTool = \"MmimmatchMatrix\"\n}");


    //Export to Clipboard
    wxTheClipboard->Clear();
    wxTheClipboard->SetData(new wxTextDataObject(clipbrd));
    wxTheClipboard->Close();



}

//Same as above, but also includes a CC COrrect node for saturation
void MyFrame::ExportFusionCCNode(cv::Mat inccm,double saturation)
{
    //Check if clipboarding works
    if (!wxTheClipboard->Open())
    {
        return;
    }
    //Convert the calculated Matrix to something which fusion understands
    cv::Mat ccm32;
    inccm.convertTo(ccm32, CV_32F);


    //Sets the the initial export matrix.
    cv::Mat fccm = (cv::Mat_<float>(5, 5) <<
        0.00f, 0.00f, 0.00f, 0.00f, 0.00f,
        0.00f, 0.00f, 0.00f, 0.00f, 0.00f,
        0.00f, 0.00f, 0.00f, 0.00f, 0.00f,
        0.00f, 0.00f, 0.00f, 0.00f, 0.00f,
        0.00f, 0.00f, 0.00f, 0.00f, 0.00f);
    //Yes I could do it in the initialisation, but for future enhancements
    //I decide to assign the values seperately
    fccm.at<float>(3, 3) = 1.0;
    fccm.at<float>(4, 4) = 1.0;
    //copy the values from the Calculated Matrix to the export matrix.
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            fccm.at<float>(r, c) = ccm32.at<float>(r, c);
        }
    }

    //Export string plus two helper string to fill in the table and saturation values
    wxString clipbrd;
    wxString table;
    wxString helper;

    clipbrd.Printf("{\n        Tools = ordered() {\n		MimimmatchCC = ColorCorrector{\n");
    clipbrd.Append("			Inputs = {\n");
    helper.Printf("				WheelSaturation1 = Input { Value = %f, },\n", saturation);
    clipbrd.Append(helper);
    clipbrd.Append("				ColorRanges = Input {\n");
    clipbrd.Append("					Value = ColorCurves {\n");
    clipbrd.Append("						Curves = {\n");
    clipbrd.Append("							{\n");
    clipbrd.Append("								Points = {\n");
    clipbrd.Append("									{ 0, 1 },\n");
    clipbrd.Append("									{ 0.4, 0.2 },\n");
    clipbrd.Append("									{ 0.6, 0 },\n");
    clipbrd.Append("									{ 1, 0 }\n");
    clipbrd.Append("								}\n");
    clipbrd.Append("							},\n");
    clipbrd.Append("							{\n");
    clipbrd.Append("								Points = {\n");
    clipbrd.Append("									{ 0, 0 },\n");
    clipbrd.Append("									{ 0.4, 0 },\n");
    clipbrd.Append("									{ 0.6, 0.2 },\n");
    clipbrd.Append("									{ 1, 1 }\n");
    clipbrd.Append("								}\n");
    clipbrd.Append("							}\n");
    clipbrd.Append("						},");
    clipbrd.Append("					},\n");
    clipbrd.Append("				},\n");
    clipbrd.Append("				HistogramIgnoreTransparent = Input { Value = 1, },\n");
    clipbrd.Append("				Input = Input {\n");
    clipbrd.Append("					SourceOp = \"MimimmatchMatrix\",\n");
    clipbrd.Append("					Source = \"Output\",\n");
    clipbrd.Append("				},\n");
    clipbrd.Append("			},\n");
    clipbrd.Append("			ViewInfo = OperatorInfo { Pos = { 927, 274.5 } },\n");
    clipbrd.Append("		},\n");
    clipbrd.Append("		MimimmatchMatrix = CustomColorMatrix {\n");
    clipbrd.Append("			CtrlWZoom = false,\n");
    clipbrd.Append("			Inputs = {\n");
    clipbrd.Append("				Matrix = Input {\n");
    clipbrd.Append("					Value = MatrixTable {\n");
    clipbrd.Append("						NumEntries = 54,\n");
    clipbrd.Append("						Type = 10,\n");
    clipbrd.Append("						Offset = 0,\n");
    clipbrd.Append("						Minimum = -10,\n");
    clipbrd.Append("						Maximum = 10,\n");
    clipbrd.Append("						MinimumValue = -10,\n");
    clipbrd.Append("						MaximumValue = 10,\n");
    clipbrd.Append("						StartSlope = 1,\n");

    clipbrd.Append("						EndSlope = 1,\n");
    clipbrd.Append("						StartIn = 0,\n");
    clipbrd.Append("						EndIn = 1,\n");






    clipbrd.Append("						Maximum = 10,\n");
    clipbrd.Append("						MinimumValue = -10,\n");
    clipbrd.Append("						MaximumValue = 10,\n");
    table.Printf("                        Table = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, %f, %f, %f, %f, %f, 0, 0, %f, %f, %f, %f, %f, 0, 0, %f, %f, %f, %f, %f, 0, 0, %f, %f, %f, %f, %f, 0, 0, %f, %f, %f, %f, %f, 0, 0, 0, 0, 0, 0, 0, 1 }\n",
        fccm.at<float>(0, 0), fccm.at<float>(0, 1), fccm.at<float>(0, 2), fccm.at<float>(0, 3), fccm.at<float>(0, 4),
        fccm.at<float>(1, 0), fccm.at<float>(1, 1), fccm.at<float>(1, 2), fccm.at<float>(1, 3), fccm.at<float>(1, 4),
        fccm.at<float>(2, 0), fccm.at<float>(2, 1), fccm.at<float>(2, 2), fccm.at<float>(2, 3), fccm.at<float>(2, 4),
        fccm.at<float>(3, 0), fccm.at<float>(3, 1), fccm.at<float>(3, 2), fccm.at<float>(3, 3), fccm.at<float>(3, 4),
        fccm.at<float>(4, 0), fccm.at<float>(4, 1), fccm.at<float>(4, 2), fccm.at<float>(4, 3), fccm.at<float>(4, 4));
    clipbrd.Append(table);
    clipbrd.Append("					},\n");
    clipbrd.Append("				},\n");
    clipbrd.Append("			},\n");
    clipbrd.Append("			ViewInfo = OperatorInfo { Pos = { 777.5, 287.5 } },\n");
    clipbrd.Append("		}\n");
    clipbrd.Append("	}\n");
    clipbrd.Append("}\n");



    //Export to Clipboard
    wxTheClipboard->Clear();
    wxTheClipboard->SetData(new wxTextDataObject(clipbrd));
    wxTheClipboard->Close();



}
