// AboutDlg.cpp
#include "AboutDlg.h"

#include <wx/icon.h>
#include <wx/bitmap.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/hyperlink.h>
#include <wx/generic/statbmpg.h>


//ABout Panel nothing interesting to be seen here



AboutDlg::AboutDlg(wxWindow* parent)
    : wxDialog(parent,
        wxID_ANY,
        "About mimimatch",
        wxDefaultPosition,
        wxDefaultSize,
        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

    wxIcon appIcon("sample", wxBITMAP_TYPE_ICO_RESOURCE, 128, 128);
    if (!appIcon.IsOk())
        appIcon = wxIcon("sample");

    wxBitmap bmp;
    if (appIcon.IsOk())
        bmp = wxBitmap(appIcon);

    if (bmp.IsOk())
    {
        wxGenericStaticBitmap* iconCtrl = new wxGenericStaticBitmap(this, wxID_ANY, bmp);
        iconCtrl->SetMinSize(bmp.GetSize());
        topSizer->Add(iconCtrl, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);
    }

    wxStaticText* txt_title = new wxStaticText(this, wxID_ANY, "mmimatch");
    wxStaticText* txt_version = new wxStaticText(this, wxID_ANY, "Version 0.5");
    wxStaticText* txt_copyright = new wxStaticText(this, wxID_ANY, "(C) 2026 by chris huf");
    wxHyperlinkCtrl* txt_link = new wxHyperlinkCtrl(
        this,
        wxID_ANY,
        "www.splotchdog.com",
        "https://www.splotchdog.com"
    );

    wxStaticText* txt_uses1 = new wxStaticText(this, wxID_ANY, "Uses WXWidgets 3.14");
    wxStaticText* txt_uses2 = new wxStaticText(this, wxID_ANY, "Uses OPENCV 4 with contributions");

    topSizer->Add(txt_title, 0, wxLEFT | wxRIGHT | wxTOP | wxALIGN_CENTER_HORIZONTAL, 8);
    topSizer->Add(txt_version, 0, wxLEFT | wxRIGHT | wxTOP | wxALIGN_CENTER_HORIZONTAL, 1);
    topSizer->Add(txt_copyright, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 1);
    topSizer->Add(txt_link, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 1);
    topSizer->Add(txt_uses1, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 4);
    topSizer->Add(txt_uses2, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 4);
    topSizer->Add(CreateSeparatedButtonSizer(wxOK), 0, wxEXPAND | wxALL, 10);

    SetSizerAndFit(topSizer);
    SetSize(wxSize(300, 400));
    CentreOnParent();
}
