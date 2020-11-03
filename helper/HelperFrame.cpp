#include "HelperFrame.h"
#include "Discovery.h"

HelperFrame::HelperFrame(const wxString &title) : wxFrame(nullptr, wxID_ANY, title), index(1)
{
    wxPanel *panel = new wxPanel(this, wxID_ANY);
    wxSizer *rootSizer = new wxBoxSizer(wxVERTICAL);

    listCtrl = new wxDataViewListCtrl(panel, wxID_ANY);

    listCtrl->AppendTextColumn(_T("Index"), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_CENTER);
    listCtrl->AppendTextColumn(_T("Anchor ID"), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_CENTER);
    listCtrl->AppendTextColumn(_T("IP Address"), wxDATAVIEW_CELL_EDITABLE, wxCOL_WIDTH_AUTOSIZE, wxALIGN_CENTER);

    // wxVector<wxVariant> data;
    // data.push_back(wxVariant("1"));
    // data.push_back(wxVariant("82B31061144CE8B"));
    // data.push_back(wxVariant("192.168.1.1"));
    // listCtrl->AppendItem(data);

    // data.clear();
    // data.push_back(wxVariant("2"));
    // data.push_back(wxVariant("82B31061144CE8B"));
    // data.push_back(wxVariant("192.168.1.1"));
    // listCtrl->AppendItem(data);
    rootSizer->Add(listCtrl, wxSizerFlags(1).Expand().Border());

    m_pLogCtrl = new wxTextCtrl(panel, wxID_ANY, wxString(), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    m_pLogCtrl->SetMinSize(wxSize(-1, 100));
    m_pOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_pLogCtrl));
    wxLogMessage("This is the log window");
    rootSizer->Add(m_pLogCtrl, wxSizerFlags().Expand().Border());

    wxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton *btnOk = new wxButton(panel, wxID_OK);
    wxButton *btnClear = new wxButton(panel, wxID_CLEAR);
    btnSizer->AddStretchSpacer(1);
    btnSizer->Add(btnClear, wxSizerFlags().Border());
    btnSizer->AddStretchSpacer(1);
    btnSizer->Add(btnOk, wxSizerFlags().Border());
    btnSizer->AddStretchSpacer(1);
    rootSizer->Add(btnSizer, wxSizerFlags().Border().Expand());

    // panel->SetSizer(rootSizer);
    // rootSizer->SetSizeHints(this);
    panel->SetSizerAndFit(rootSizer);

    //Bind Events
    Bind(wxEVT_THREAD, &HelperFrame::OnAnchorFound, this, ids::ANCHOR_FOUND);
}

HelperFrame::~HelperFrame()
{
    delete wxLog::SetActiveTarget(m_pOld);
}

void HelperFrame::OnAnchorFound(wxThreadEvent &event)
{
    auto info = event.GetPayload<types::AnchorInfo>();
    wxVector<wxVariant> data;
    data.push_back(wxVariant(index++));
    data.push_back(wxVariant(std::to_string(info.id)));
    data.push_back(wxVariant(info.ip));
    listCtrl->AppendItem(data);
}
