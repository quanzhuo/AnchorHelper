#include "HelperFrame.h"
#include "Discovery.h"
#include "Connection.h"
#include <sstream>
#include <thread>
#include "DB.h"
#include "tools.h"

#include "wx/regex.h"

HelperFrame::HelperFrame(const wxString &title) : wxFrame(nullptr, wxID_ANY, title), index(1)
{
    wxPanel *panel = new wxPanel(this, wxID_ANY);
    wxSizer *rootSizer = new wxBoxSizer(wxVERTICAL);

    listCtrl = new wxDataViewListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES);
    listCtrl->AppendTextColumn(_T("Index"), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_CENTER);
    listCtrl->AppendToggleColumn(_T("Selected"), wxDATAVIEW_CELL_ACTIVATABLE, wxCOL_WIDTH_AUTOSIZE, wxALIGN_CENTER);
    listCtrl->AppendTextColumn(_T("Anchor ID"), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_CENTER);
    listCtrl->AppendTextColumn(_T("IP Address"), wxDATAVIEW_CELL_EDITABLE, 120, wxALIGN_CENTER);
    listCtrl->AppendTextColumn(_T("Status"), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_CENTER);
    listCtrl->SetMinSize(wxSize(580, 250));
    rootSizer->Add(listCtrl, wxSizerFlags(1).Expand().Border());

    wxGridSizer *gridSizer = new wxGridSizer(2, 5, 10);
    gridSizer->Add(new wxStaticText(panel, wxID_ANY, _T("Netmask: ")), wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL));
    m_pNetmask = new wxTextCtrl(panel, wxID_ANY);
    gridSizer->Add(m_pNetmask, wxSizerFlags(1).Align(wxGROW | wxALIGN_CENTER_VERTICAL));
    gridSizer->Add(new wxStaticText(panel, wxID_ANY, _T("Gateway: ")), wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL));
    m_pGateway = new wxTextCtrl(panel, wxID_ANY);
    gridSizer->Add(m_pGateway, wxSizerFlags(1).Align(wxGROW | wxALIGN_CENTER_VERTICAL));
    gridSizer->Add(new wxStaticText(panel, wxID_ANY, _T("Use DHCP: ")), wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL));
    m_pUseDHCP = new wxCheckBox(panel, wxID_ANY, "");
    m_pUseDHCP->Bind(wxEVT_CHECKBOX, &HelperFrame::OnChkBoxClicked, this);
    gridSizer->Add(m_pUseDHCP, wxSizerFlags().Align(wxALIGN_LEFT));
    rootSizer->Add(gridSizer, wxSizerFlags().Expand().Border());

    m_pLogCtrl = new wxTextCtrl(panel, wxID_ANY, wxString(), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    m_pLogCtrl->SetMinSize(wxSize(-1, 100));
    m_pOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_pLogCtrl));
    wxLogMessage("This is the log window");
    rootSizer->Add(m_pLogCtrl, wxSizerFlags().Expand().Border());

    wxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pBtnApply = new wxButton(panel, wxID_APPLY);
    m_pBtnApply->Bind(wxEVT_BUTTON, &HelperFrame::OnBtnApply, this);
    m_pBtnApply->SetToolTip(_T("Click to set static/dhcp"));
    wxButton *btnClear = new wxButton(panel, wxID_CLEAR);
    btnClear->Bind(wxEVT_BUTTON, &HelperFrame::OnClearLog, this);
    btnClear->SetToolTip(_T("Clear the log window"));
    m_pBtnPopulate = new wxButton(panel, helper::cons::BTN_ID_AUTO, _T("Populate IP"));
    m_pBtnPopulate->SetToolTip(_T("Populate static ip automatically!"));
    m_pBtnPopulate->Bind(wxEVT_BUTTON, &HelperFrame::OnBtnPopulate, this);

    btnSizer->AddStretchSpacer(1);
    btnSizer->Add(btnClear, wxSizerFlags().Border());
    btnSizer->AddStretchSpacer(1);
    btnSizer->Add(m_pBtnPopulate, wxSizerFlags().Border());
    btnSizer->AddStretchSpacer(1);
    btnSizer->Add(m_pBtnApply, wxSizerFlags().Border());
    btnSizer->AddStretchSpacer(1);

    rootSizer->Add(btnSizer, wxSizerFlags().Border().Expand());
    panel->SetSizer(rootSizer);
    rootSizer->SetSizeHints(this);
    CenterOnScreen();

    //Bind Thread Events to handers
    Bind(wxEVT_THREAD, &HelperFrame::OnAnchorFound, this, helper::ids::ANCHOR_FOUND);
    Bind(wxEVT_THREAD, &HelperFrame::OnAnchorRemoved, this, helper::ids::ANCHOR_REMOVED);
    Bind(wxEVT_THREAD, &HelperFrame::OnProgressUpdate, this, helper::ids::PROGRESS_UPDATE);
    Bind(wxEVT_THREAD, &HelperFrame::OnScanIPFinished, this, helper::ids::IP_SCAN_FINISHED);

    init_crc16();
}

HelperFrame::~HelperFrame()
{
    delete wxLog::SetActiveTarget(m_pOld);
}

void HelperFrame::OnAnchorFound(wxThreadEvent &event)
{
    auto pa = event.GetPayload<std::shared_ptr<helper::types::Anchor>>();
    std::ostringstream os;
    os << std::hex;
    os << pa->id;
    std::string str_id = os.str();
    bool exists = false;
    unsigned count = listCtrl->GetItemCount();
    for (unsigned i = 0; i < count; ++i)
    {
        std::string text = listCtrl->GetTextValue(i, helper::cons::COL_IDX_ANC_ID).ToStdString();
        if (text == str_id)
        {
            exists = true;
            break;
        }
    }

    if (exists)
    {
        return;
    }

    wxVector<wxVariant> data;
    data.push_back(wxVariant(std::to_string(index++)));
    data.push_back(true);
    data.push_back(wxVariant(str_id));
    data.push_back(wxVariant(inet_ntoa(pa->ip)));
    data.push_back(_T("Anchor Found"));
    listCtrl->AppendItem(data);
}

void HelperFrame::OnAnchorRemoved(wxThreadEvent &event)
{
    auto wxstr_id = event.GetString();
    wxLogMessage("OnAnchorRemoved: anchor id: %s", wxstr_id);
    unsigned count = listCtrl->GetItemCount();
    for (unsigned i = 0; i < count; ++i)
    {
        wxstr_id.LowerCase();
        wxString text = listCtrl->GetTextValue(i, helper::cons::COL_IDX_ANC_ID);
        text.LowerCase();
        if (wxstr_id == text)
        {
            listCtrl->DeleteItem(i);
            break;
        }
    }
}

void HelperFrame::OnClearLog(wxCommandEvent &event)
{
    m_pLogCtrl->Clear();
}

void HelperFrame::OnBtnApply(wxCommandEvent &event)
{
    // check if ip is valid
    if (!m_pUseDHCP->GetValue() && !VerifyInput())
        return;

    // store ip
    unsigned count = listCtrl->GetItemCount();
    for (unsigned i = 0; i < count; ++i)
    {
        std::string id_str = listCtrl->GetTextValue(i, helper::cons::COL_IDX_ANC_ID).ToStdString();
        std::string ip_str = listCtrl->GetTextValue(i, helper::cons::COL_IDX_IP_ADDR).ToStdString();
        bool selected = listCtrl->GetToggleValue(i, helper::cons::COL_IDX_SELECTED);
        std::stringstream ss;
        uint64_t id;
        ss << std::hex;
        ss.str(id_str);
        ss >> id;
        unsigned long ip_int = inet_addr(ip_str.c_str());
        struct in_addr ip;
        memcpy(&ip, &ip_int, 4);
        auto pa = DB::GetDB().FindAnchor(id);
        if (pa)
        {
            pa->ip_set = ip;
            pa->selected = selected;
        }
    }

    // store netmask and gateway
    std::string netmask = m_pNetmask->GetValue().ToStdString();
    std::string gateway = m_pGateway->GetValue().ToStdString();
    unsigned long ip_long = inet_addr(netmask.c_str());
    struct in_addr ip;
    memcpy(&ip, &ip_long, 4);
    DB::GetDB().SetNetmask(ip);
    ip_long = inet_addr(gateway.c_str());
    memcpy(&ip, &ip_long, 4);
    DB::GetDB().SetGateway(ip);
    wxLogMessage("netmask: %s, gateway: %s", netmask.c_str(), gateway.c_str());

    (new TryConnect())->Start();
    m_pBtnApply->Disable();
    m_pBtnPopulate->Disable();
}

void HelperFrame::OnBtnPopulate(wxCommandEvent &event)
{
    auto count = listCtrl->GetItemCount();
    if(!count) return;
    int seleced_count{};
    for(auto i=0; i<count; ++i)
    {
        bool selected = listCtrl->GetToggleValue(i, helper::cons::COL_IDX_SELECTED);
        if(selected) ++seleced_count;
    }
    if (!seleced_count)
    {
        wxMessageBox(_T("Please select the anchors."), _T("Warning"), wxICON_WARNING, this);
        return;
    }

    std::string id_str = listCtrl->GetTextValue(0, helper::cons::COL_IDX_ANC_ID).ToStdString();
    uint64_t id = StringToUInt64(id_str);
    std::string ip_str = DB::GetDB().FindAnchor(id)->GetIPString();
    std::thread task(ip_scan, ip_str, seleced_count);
    task.detach();
    m_pBtnPopulate->Disable();
    m_pBtnApply->Disable();
}

void HelperFrame::OnChkBoxClicked(wxCommandEvent &event)
{
    bool use_dhcp = m_pUseDHCP->GetValue();
    m_pGateway->Enable(!use_dhcp);
    m_pNetmask->Enable(!use_dhcp);
    m_pBtnPopulate->Enable(!use_dhcp);
    listCtrl->Enable(!use_dhcp);
    DB::GetDB().SetUseDHCP(use_dhcp);
}

bool HelperFrame::ConnectToAnchor(std::shared_ptr<helper::types::Anchor> pa)
{
    wxLogMessage("Connect to anchor, id: %llx, ip: %s", pa->id, inet_ntoa(pa->ip));

    bool ret = true;
    // pthread_mutex_lock(&lock);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr = pa->ip;
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
        pa->sock = sock;
        AnchorConnection *conn = new AnchorConnection(sock, pa);
        conn->Start();
    }
    catch (std::string e)
    {
        wxLogMessage("ConnectToAnchor : %s to %s", e.c_str(), inet_ntoa(pa->ip_set));
        ret = false;
    }

    // pthread_mutex_unlock(&lock);

    return ret;
}

void HelperFrame::OnProgressUpdate(wxThreadEvent &event)
{
    bool finished{true};
    auto info = event.GetPayload<std::shared_ptr<helper::types::Anchor>>();
    unsigned count = listCtrl->GetItemCount();
    for (unsigned i = 0; i < count; ++i)
    {
        std::string id_str = listCtrl->GetTextValue(i, helper::cons::COL_IDX_ANC_ID).ToStdString();
        uint64_t id = StringToUInt64(id_str);
        if (id == info->id)
        {
            wxString text = GetStatusText(info->status);
            listCtrl->SetTextValue(text, i, helper::cons::COL_IDX_STATUS);
            break;
        }
    }

    for(unsigned i=0; i<count; ++i)
    {
        bool selected = listCtrl->GetToggleValue(i, helper::cons::COL_IDX_SELECTED);
        std::string id_str = listCtrl->GetTextValue(i, helper::cons::COL_IDX_ANC_ID).ToStdString();
        uint64_t id = StringToUInt64(id_str);
        auto pa = DB::GetDB().FindAnchor(id);
        if (selected &&
                ((pa->status != helper::types::Status::DYNAMIC_MARK_FAILED) &&
                 (pa->status != helper::types::Status::DYNAMIC_MARK_OK) &&
                 (pa->status != helper::types::Status::SET_STATIC_IP_FAILED) &&
                 (pa->status != helper::types::Status::SET_STATIC_IP_OK)))
        {
            finished = false;
            break;
        }
    }

    if (finished)
    {
        m_pBtnApply->Enable();
        m_pBtnPopulate->Enable();
    }
}

void HelperFrame::OnScanIPFinished(wxThreadEvent &event)
{
    auto info = event.GetPayload<helper::types::scaned_ips>();
    unsigned info_idx = 0;
    auto rows = listCtrl->GetItemCount();
    for (unsigned i = 0u; i < rows; ++i)
    {
        bool selected = listCtrl->GetToggleValue(i, helper::cons::COL_IDX_SELECTED);
        if (selected && (info_idx < info.n))
        {
            listCtrl->SetTextValue(info.ps[info_idx], i, helper::cons::COL_IDX_IP_ADDR);
            ++info_idx;
        }
    }
    delete[] info.ps;
    m_pBtnPopulate->Enable();
    m_pBtnApply->Enable();
}

wxString HelperFrame::GetStatusText(helper::types::Status s)
{
    using namespace helper::types;
    wxString text;
    switch (s)
    {
    case Status::NOT_CONNECTED:
        text = _T("Not connected");
    case Status::CONNECTED:
        text = _T("Connected");
        break;
    case Status::CONNECT_FAILED:
        text = _T("Connect failed");
        break;
    case Status::STATIC_MARK_OK:
        text = _T("Static mark ok");
        break;
    case Status::STATIC_MARK_FAILED:
        text = _T("Static mark failed");
        break;
    case Status::DYNAMIC_MARK_OK:
        text = _T("DHCP mark ok");
        break;
    case Status::DYNAMIC_MARK_FAILED:
        text = _T("DHCP mark failed");
        break;
    case Status::SET_STATIC_IP_OK:
        text = _T("Static IP Set OK");
        break;
    case Status::SET_STATIC_IP_FAILED:
        text = _T("Static IP Set failed");
        break;
    case Status::DONE:
        text = _T("Done");
        break;
    default:
        break;
    }
    return text;
}

bool HelperFrame::IsIPValid(const wxString &ip)
{
    wxRegEx pattern("^[[:digit:]]{1,3}\.[[:digit:]]{1,3}\.[[:digit:]]{1,3}\.[[:digit:]]{1,3}$");
    bool b = pattern.IsValid();
    if (pattern.Matches(ip))
        return true;
    return false;
}

bool HelperFrame::VerifyInput()
{
    using namespace helper::cons;
    for (auto i = 0; i < listCtrl->GetItemCount(); ++i)
    {
        if (!listCtrl->GetToggleValue(i, helper::cons::COL_IDX_SELECTED))
            continue;
        wxString ip = listCtrl->GetTextValue(i, COL_IDX_IP_ADDR);
        if (!IsIPValid(ip))
        {
            ip.Append(" is not a valid ip");
            wxMessageBox(ip, _T("Warning"), wxICON_WARNING, this);
            return false;
        }
    }

    wxString netmask = m_pNetmask->GetValue();
    wxString gateway = m_pGateway->GetValue();

    if (!IsIPValid(netmask))
    {
        wxMessageBox(_T("netmask is invalid"), _T("Warning"), wxICON_WARNING, this);
        return false;
    }

    if (!IsIPValid(gateway))
    {
        wxMessageBox(_T("gateway is invalid"), _T("Warning"), wxICON_WARNING, this);
        return false;
    }

    return true;
}

uint64_t HelperFrame::StringToUInt64(const std::string &id_str)
{
    std::stringstream ss;
    uint64_t id;
    ss << std::hex;
    ss.str(id_str);
    ss >> id;
    return id;
}
