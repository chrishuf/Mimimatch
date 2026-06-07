#pragma once

#include <wx/scrolwin.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/event.h>
#include"CheckrClr.h"




class SidePanel : public wxScrolledWindow
{
public:
    enum
    {
        ID_BTN_LOAD_SOURCE = wxID_HIGHEST + 500,
        ID_COLORSPACE_CHOICE = wxID_HIGHEST + 501,
        ID_CHECKER_CHOICE = wxID_HIGHEST + 502
    };

    explicit SidePanel(wxWindow* parent);

    void BuildUI();
    void OnLoadSource(wxCommandEvent& event);

    wxButton* m_btnLoadPano = nullptr;
    wxButton* m_btnLoadFootage = nullptr;

    wxButton* m_btnDetectPano = nullptr;
    wxButton* m_btnDetectFootage = nullptr;

    wxCheckBox* m_btnMatchSat = nullptr;

    wxButton* m_btnMatchPano = nullptr;
    wxButton* m_btnMatchFootage = nullptr;

    wxButton* m_btnMatchPanoInv = nullptr;
    wxButton* m_btnMatchFootageInv = nullptr;

    wxChoice* m_colorSpace = nullptr;
    wxChoice* m_checktype = nullptr;
    ColorCheckerPanel* pan_pano;
    ColorCheckerPanel* pan_footage;

private:
    wxPanel* m_content = nullptr;
    wxBoxSizer* m_contentSizer = nullptr;



    wxDECLARE_EVENT_TABLE();
};
