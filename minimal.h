#pragma once
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/mdi.h>
#include <wx/scrolwin.h>
#include <wx/dcbuffer.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/aui/framemanager.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>

#include "FreeImage.h"
#include <algorithm>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/mcc.hpp>
#include <opencv2/mcc/ccm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "ColMatch.h"

#include "ToolsPanel.h"


#include "myhelper.h"
#include "imagepanel.h"
#include "AboutDlg.h"



//Need to define it, because of compiler error.
class MyChild;

wxDECLARE_EVENT(EVT_IMAGE_PANEL_CHANGED, wxCommandEvent);

class MyFrame : public wxMDIParentFrame
{
public:
    MyFrame(const wxString& title);
    ~MyFrame();
    class ColMatch matchit;

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnLoadPano(wxCommandEvent& event);
    void OnLoadFootage(wxCommandEvent& event);
    void OnSize(wxSizeEvent& event);
    void CreateInitialChildren();

    const int  MYID_PANO = 300;
    const int  MYID_FOOTAGE = 301;

    void OnCascade(wxCommandEvent& event);
    void OnTileHorizontal(wxCommandEvent& event);
    void OnTileVertical(wxCommandEvent& event);
    void OnArrangeIcons(wxCommandEvent& event);
    void OnNextChild(wxCommandEvent& event);
    void OnPreviousChild(wxCommandEvent& event);
    void OnAutoCheckerFootage(wxCommandEvent& event);
    void OnAutoCheckerPano(wxCommandEvent& event);
    void OnBoxChanged(wxCommandEvent& event);
    void OnMatchPano(wxCommandEvent& event);
    void OnMatchFootage(wxCommandEvent& event);

    void OnMatchPanoInv(wxCommandEvent& event);
    void OnMatchFootageInv(wxCommandEvent& event);

    void BindSidePanel();
    void SetSidebarColors();
    void ExportFusionNode(cv::Mat inccm);
    void ExportFusionCCNode(cv::Mat inccm, double saturation);

    FIBITMAP* cam_footage;
    FIBITMAP* cam_pano;
    void LoadTheImage(wxString filename, int type);
    cv::Mat mat_footage;
    cv::Mat mat_pano;

    MyChild* mdi_pano = nullptr;
    MyChild* mdi_footage = nullptr;

    DetectedChart  chart_pano;
    DetectedChart  chart_footage;

    cv::Mat patch_pano;
    cv::Mat patch_footage;


private:
    wxAuiManager m_mgr;
    SidePanel* m_sidePanel = nullptr;



    wxDECLARE_EVENT_TABLE();
};

enum
{

    ID_Window_Cascade = wxID_HIGHEST + 1,
    ID_Window_TileH,
    ID_Window_TileV,
    ID_Window_ArrangeIcons,
    ID_Window_Next,
    ID_Window_Previous,
    Minimal_Quit = wxID_EXIT,
    Minimal_About = wxID_ABOUT
};

