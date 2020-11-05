#include "HelperFrame.h"
#include "Discovery.h"
#include "Connection.h"
#include <sstream>

HelperFrame::HelperFrame(const wxString &title) : wxFrame(nullptr, wxID_ANY, title), index(1)
{
    wxPanel *panel = new wxPanel(this, wxID_ANY);
    wxSizer *rootSizer = new wxBoxSizer(wxVERTICAL);

    listCtrl = new wxDataViewListCtrl(panel, wxID_ANY);

    listCtrl->AppendTextColumn(_T("Index"), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_CENTER);
    listCtrl->AppendTextColumn(_T("Anchor ID"), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_CENTER);
    listCtrl->AppendTextColumn(_T("IP Address"), wxDATAVIEW_CELL_EDITABLE, wxCOL_WIDTH_AUTOSIZE, wxALIGN_CENTER);
    listCtrl->AppendTextColumn(_T("Status"), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_CENTER);
    rootSizer->Add(listCtrl, wxSizerFlags(1).Expand().Border());

    wxGridSizer *gridSizer = new wxGridSizer(2, 5, 10);
    gridSizer->Add(new wxStaticText(panel, wxID_ANY, _T("Netmask: ")), wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL));
    m_pNetmask = new wxTextCtrl(panel, wxID_ANY);
    gridSizer->Add(m_pNetmask, wxSizerFlags(1).Align(wxGROW | wxALIGN_CENTER_VERTICAL));
    gridSizer->Add(new wxStaticText(panel, wxID_ANY, _T("Gateway: ")), wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL));
    m_pGateway = new wxTextCtrl(panel, wxID_ANY);
    gridSizer->Add(m_pGateway, wxSizerFlags(1).Align(wxGROW | wxALIGN_CENTER_VERTICAL));
    rootSizer->Add(gridSizer, wxSizerFlags().Expand().Border());

    m_pLogCtrl = new wxTextCtrl(panel, wxID_ANY, wxString(), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    m_pLogCtrl->SetMinSize(wxSize(-1, 100));
    m_pOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_pLogCtrl));
    wxLogMessage("This is the log window");
    rootSizer->Add(m_pLogCtrl, wxSizerFlags().Expand().Border());

    wxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton *btnOk = new wxButton(panel, wxID_OK);
    btnOk->Bind(wxEVT_BUTTON, &HelperFrame::OnBtnOk, this);
    wxButton *btnClear = new wxButton(panel, wxID_CLEAR);
    btnClear->Bind(wxEVT_BUTTON, &HelperFrame::OnClearLog, this);
    btnSizer->AddStretchSpacer(1);
    btnSizer->Add(btnClear, wxSizerFlags().Border());
    btnSizer->AddStretchSpacer(1);
    btnSizer->Add(btnOk, wxSizerFlags().Border());
    btnSizer->AddStretchSpacer(1);

    rootSizer->Add(btnSizer, wxSizerFlags().Border().Expand());
    panel->SetSizer(rootSizer);
    rootSizer->SetSizeHints(this);

    //Bind Thread Events to handers
    Bind(wxEVT_THREAD, &HelperFrame::OnAnchorFound, this, helper::ids::ANCHOR_FOUND);
    Bind(wxEVT_THREAD, &HelperFrame::OnProgressUpdate, this, helper::ids::PROGRESS_UPDATE);
}

HelperFrame::~HelperFrame()
{
    delete wxLog::SetActiveTarget(m_pOld);
}

void HelperFrame::OnAnchorFound(wxThreadEvent &event)
{
    auto info = event.GetPayload<helper::types::AnchorInfo>();
    m_anc_ip[info.id] = info.ip;
    wxVector<wxVariant> data;
    data.push_back(wxVariant(index++));
    data.push_back(wxVariant(std::to_string(info.id)));
    data.push_back(wxVariant(inet_ntoa(info.ip)));
    listCtrl->AppendItem(data);
}

void HelperFrame::OnClearLog(wxCommandEvent &event)
{
    m_pLogCtrl->Clear();
}

void HelperFrame::OnBtnOk(wxCommandEvent &event)
{
    //1. 扫描出来用户设置的所有基站ip
    unsigned count = listCtrl->GetItemCount();
    for (unsigned i = 0; i < count; ++i)
    {
        std::string id_str = listCtrl->GetTextValue(i, 1).ToStdString();
        std::string ip_str = listCtrl->GetTextValue(i, 2).ToStdString();
        std::stringstream ss;
        uint64_t id;
        ss.str(id_str);
        ss >> id;
        unsigned long ip_int = inet_addr(ip_str.c_str());
        struct in_addr ip;
        memcpy(&ip, &ip_int, 4);
        m_anc_new_ip[id] = ip;
    }

    (new TryConnect())->Start();
}

bool HelperFrame::ConnectToAnchor(helper::types::AnchorInfo &info)
{
    wxLogMessage("Connect to anchor, id: %16llx, ip: %s", info.id, inet_ntoa(info.ip));

    bool ret = true;
    // pthread_mutex_lock(&lock);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr = info.ip;
    addr.sin_port = htons(3000);

    try
    {
        Socket *sock;
        try
        {
            sock = new SocketClient(addr);
        }
        catch (std::string e)
        {
            throw((std::string)("Socket error"));
        }

        AnchorConnection *conn = new AnchorConnection(sock, info);
        conn->Start();
    }
    catch (std::string e)
    {
        wxLogMessage("ConnectToAnchor : %s to %s", e.c_str(), inet_ntoa(info.ip));
        ret = false;
    }

    // pthread_mutex_unlock(&lock);

    return ret;
}

void HelperFrame::OnProgressUpdate(wxThreadEvent &event)
{
    auto info = event.GetPayload<helper::types::StatusInfo>();
    unsigned count = listCtrl->GetItemCount();
    for (unsigned i = 0; i < count; ++i)
    {
        std::string id_str = listCtrl->GetTextValue(i, 1).ToStdString();
        std::stringstream ss;
        uint64_t id;
        ss.str(id_str);
        ss >> id;
        if (id == info.id)
        {
            wxString text = GetStatusText(info.status);
            listCtrl->SetTextValue(text, i, 3);
            break;
        }
    }
}

wxString HelperFrame::GetStatusText(helper::types::Status s)
{
    using namespace helper::types;
    wxString text;
    switch (s)
    {
    case Status::CONNECTED:
        text = _T("Connected");
        break;
    case Status::CONNECT_FAILED:
        text = _T("Connect Failed");
        break;
    case Status::DONE:
        text = _T("Done");
        break;
    case Status::SETTING_IP:
        text = _T("Setting IP");
        break;
    default:
        break;
    }
    return text;
}
