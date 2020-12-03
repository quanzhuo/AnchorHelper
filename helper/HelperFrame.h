#pragma once

#include <map>
#include "Typess.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dataview.h"
#include "Connection.h"

using helper::types::Anchor;

class HelperFrame : public wxFrame
{
    friend void TryConnect::Run();

public:
    HelperFrame(const wxString &title);
    ~HelperFrame();
    bool ConnectToAnchor(std::shared_ptr<Anchor> pa);

private:
    void OnAnchorFound(wxThreadEvent &event);
    void OnAnchorRemoved(wxThreadEvent &event);
    void OnClearLog(wxCommandEvent &event);
    void OnBtnApply(wxCommandEvent &event);
    void OnBtnPopulate(wxCommandEvent &event);
    void OnChkBoxClicked(wxCommandEvent& event);
    void OnProgressUpdate(wxThreadEvent &event);
    void OnScanIPFinished(wxThreadEvent &event);
    wxString GetStatusText(helper::types::Status s);
    bool IsIPValid(const wxString& ip);
    bool VerifyInput();
    uint64_t StringToUInt64(const std::string &id_str);

private:
    wxTextCtrl *m_pLogCtrl;
    wxLog *m_pOld;
    wxDataViewListCtrl *listCtrl;
    wxTextCtrl *m_pNetmask;
    wxTextCtrl *m_pGateway;
    wxCheckBox* m_pUseDHCP;
    wxButton *m_pBtnPopulate;
    wxButton *m_pBtnApply;
    int index;
};
