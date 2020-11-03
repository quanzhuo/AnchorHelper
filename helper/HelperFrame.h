#pragma once

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dataview.h"

class HelperFrame : public wxFrame
{
public:
    HelperFrame(const wxString &title);

    ~HelperFrame();

private:
    void OnAnchorFound(wxThreadEvent &event);

private:
    wxTextCtrl *m_pLogCtrl;
    wxLog *m_pOld;
    wxDataViewListCtrl *listCtrl;
    int index;
};
