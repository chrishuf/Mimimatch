#include "ToolsPanel.h"
#include <wx/msgdlg.h>


//tools sidebar panel.



//Event is called for debug purposes, in the end I used regular
//bind events in MyFrame to Capture events.

wxBEGIN_EVENT_TABLE(SidePanel, wxScrolledWindow)
//EVT_BUTTON(SidePanel::ID_BTN_LOAD_SOURCE, SidePanel::OnLoadSource)
wxEND_EVENT_TABLE()

SidePanel::SidePanel(wxWindow* parent)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxVSCROLL | wxHSCROLL)
{
    SetScrollRate(5, 5);
    BuildUI();
}


// Buildup the interface
void SidePanel::BuildUI()
{
    //colorspace popup string array
    wxArrayString csItems;
    csItems.Add("sRGB");
    csItems.Add("sRGB Linear");
    csItems.Add("AdobeRGB");
    csItems.Add("AdobeRGB Linear");
    csItems.Add("WideGamutRGB");
    csItems.Add("WideGamutRGB Linear");
    csItems.Add("ProPhotoRGB");
    csItems.Add("ProPhotoRGB Linear");
    csItems.Add("DCI_P3_RGB");
    csItems.Add("DCI_P3_RGB Linear");
    csItems.Add("AppleRGB");
    csItems.Add("AppleRGB Linear");
    csItems.Add("REC_709_RGB");
    csItems.Add("REC_709_RGB Linear");
    csItems.Add("REC_2020_RGB");
    csItems.Add("REC_2020_RGB Linear");
    csItems.Add("XYZ_D65_2");
    csItems.Add("XYZ_D65_10");
    csItems.Add("XYZ_D50_2");
    csItems.Add("XYZ_D50_10");
    csItems.Add("XYZ_A_2");
    csItems.Add("XYZ_A_10");
    csItems.Add("XYZ_D55_2");
    csItems.Add("XYZ_D55_10");
    csItems.Add("XYZ_D75_2");
    csItems.Add("XYZ_D75_10");
    csItems.Add("XYZ_E_2");
    csItems.Add("XYZ_E_10");
    csItems.Add("CIE Lab_D65_2");
    csItems.Add("CIE Lab_D65_10");
    csItems.Add("CIE Lab_D50_2");
    csItems.Add("CIE Lab_D50_10");
    csItems.Add("CIE Lab_A_2");
    csItems.Add("CIE Lab_A_10");
    csItems.Add("CIE Lab_D55_2");
    csItems.Add("CIE Lab_D55_10");
    csItems.Add("CIE Lab_D75_2");
    csItems.Add("CIE Lab_D75_10");
    csItems.Add("CIE Lab_E_2");
    csItems.Add("CIE Lab_E_10");

    //checkertype array for future implemntations
    wxArrayString checktype;

    checktype.Add("X Rite");
    checktype.Add("Datacolor");



    m_content = new wxPanel(this);
    m_contentSizer = new wxBoxSizer(wxVERTICAL);


    m_checktype = new wxChoice(
        m_content,
        ID_CHECKER_CHOICE,
        wxDefaultPosition,
        wxSize(150, -1),
        checktype
    );




    m_btnLoadPano = new wxButton(
        m_content,
        ID_BTN_LOAD_SOURCE,
        "Load Pano Shot"
    );
    m_btnLoadFootage = new wxButton(
        m_content,
        ID_BTN_LOAD_SOURCE,
        "Load Footage Shot"
    );

    m_btnDetectPano = new wxButton(
        m_content,
        ID_BTN_LOAD_SOURCE,
        "Detect Chart Pano"
    );

    m_btnDetectFootage = new wxButton(
        m_content,
        ID_BTN_LOAD_SOURCE,
        "Detect Chart Footage"
    );

    m_colorSpace= new wxChoice(
        m_content,
        ID_COLORSPACE_CHOICE,
        wxDefaultPosition,
        wxSize(150, -1),
        csItems
    );
    m_colorSpace->SetMinSize(wxSize(GetSize().x - 20, 20));

    m_btnMatchSat = new wxCheckBox(m_content, ID_BTN_LOAD_SOURCE, "Include Saturation Match");


    m_btnMatchPano = new wxButton(
        m_content,
        ID_BTN_LOAD_SOURCE,
        "Match Pano"
    );

    m_btnMatchPanoInv = new wxButton(
        m_content,
        ID_BTN_LOAD_SOURCE,
        "Invert Match Pano"
    );

    m_btnMatchFootage = new wxButton(
        m_content,
        ID_BTN_LOAD_SOURCE,
        "Match Footage"
    );

    m_btnMatchFootageInv = new wxButton(
        m_content,
        ID_BTN_LOAD_SOURCE,
        "Invert Match Footage"
    );
    pan_pano = new ColorCheckerPanel(m_content);
    pan_pano->SetMinSize(wxSize(GetSize().x-20, 180));
    pan_footage = new ColorCheckerPanel(m_content);
    pan_footage->SetMinSize(wxSize(GetSize().x - 20, 180));

    m_contentSizer->Add(new wxStaticText(m_content, wxID_ANY, "Checker Type"), 0, wxALL, 8);
    m_contentSizer->Add(m_checktype, 0, wxEXPAND | wxALL, 0);
    m_contentSizer->Add(new wxStaticText(m_content, wxID_ANY, "Tool"), 0, wxALL, 8);
    m_contentSizer->Add(m_btnLoadPano, 0, wxEXPAND | wxALL, 0);
    m_contentSizer->Add(m_btnLoadFootage, 0, wxEXPAND | wxALL, 0);
    m_contentSizer->AddSpacer(8);
    m_contentSizer->Add(m_btnDetectPano, 0, wxEXPAND | wxALL, 0);
    m_contentSizer->Add(m_btnDetectFootage, 0, wxEXPAND | wxALL, 0);
    m_contentSizer->AddSpacer(8);
    m_contentSizer->Add(new wxStaticText(m_content, wxID_ANY, "Color Space"), 0, wxALL, 8);
    m_contentSizer->AddSpacer(1);
    m_contentSizer->Add(m_colorSpace, 0, wxEXPAND | wxALL, 0);
    m_contentSizer->Add(m_btnMatchSat, 0, wxEXPAND | wxALL, 0);
    m_contentSizer->AddSpacer(8);
    m_contentSizer->Add(m_btnMatchPano, 0, wxEXPAND | wxALL, 0);
    m_contentSizer->Add(m_btnMatchPanoInv, 0, wxEXPAND | wxALL, 0);
    m_contentSizer->Add(m_btnMatchFootage, 0, wxEXPAND | wxALL, 0);
    m_contentSizer->Add(m_btnMatchFootageInv, 0, wxEXPAND | wxALL, 0);
    m_contentSizer->AddSpacer(8);
    m_contentSizer->Add(new wxStaticText(m_content, wxID_ANY, "Pano"), 0, wxALL, 8);
    m_contentSizer->Add(pan_pano, 0, wxEXPAND | wxALL, 0);
    m_contentSizer->AddSpacer(8);
    m_contentSizer->Add(new wxStaticText(m_content, wxID_ANY, "Footage"), 0, wxALL, 8);
    m_contentSizer->Add(pan_footage, 0, wxEXPAND | wxALL, 0);

    m_content->SetSizer(m_contentSizer);
    m_contentSizer->Fit(m_content);

    auto* outer = new wxBoxSizer(wxVERTICAL);
    outer->Add(m_content, 1, wxEXPAND | wxALL, 0);

    SetSizer(outer);
    FitInside();
    Layout();
    m_checktype->SetSelection(0); // XRite
    m_colorSpace->SetSelection(12); // Linear sRGB
}

//For debugging.
void SidePanel::OnLoadSource(wxCommandEvent& event)
{
    wxMessageBox("Load Source clicked",
        "SidePanel",
        wxOK | wxICON_INFORMATION,
        this);
}
