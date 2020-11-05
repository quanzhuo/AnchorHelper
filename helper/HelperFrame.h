#pragma once

#include <map>
#include "Types.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dataview.h"
#include "Connection.h"

class HelperFrame : public wxFrame
{
    friend void TryConnect::Run();

public:
    HelperFrame(const wxString &title);
    ~HelperFrame();
    bool ConnectToAnchor(helper::types::AnchorInfo &info);

private:
    void OnAnchorFound(wxThreadEvent &event);
    void OnClearLog(wxCommandEvent &event);
    void OnBtnOk(wxCommandEvent &event);
    void OnProgressUpdate(wxThreadEvent &event);
    wxString GetStatusText(helper::types::Status s);

private:
    wxTextCtrl *m_pLogCtrl;
    wxLog *m_pOld;
    wxDataViewListCtrl *listCtrl;
    wxTextCtrl *m_pNetmask;
    wxTextCtrl *m_pGateway;
    int index;
    // 此处的ip是通过mdns扫描出来的ip
    std::map<uint64_t, struct in_addr> m_anc_ip;
    // 此处的ip是用户自己设置的ip
    std::map<uint64_t, struct in_addr> m_anc_new_ip;
};
