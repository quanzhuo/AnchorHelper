#include "HelperApp.h"
#include "HelperFrame.h"
#include "Connection.h"
#include "Types.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

wxDECLARE_APP(HelperApp);

AnchorConnection::AnchorConnection(Socket *s, const helper::types::AnchorInfo &info) : HandlerConnection(s), m_info(info)
{
    using namespace helper::cmds;
    using namespace helper::cons;
    addHandler(RTLS_SET_GET_STATICADDR, RTLS_SET_GET_STATICADDR_LEN, &AnchorConnection::HandleUseStaticAddr);
    addHandler(RTLS_USE_STATICADDR, RTLS_USE_STATICADDR_LEN, &AnchorConnection::HandleSetStaticAddr);
}

void AnchorConnection::Run()
{
    HandlerConnection<AnchorConnection>::Run();
    wxLogMessage("ANC %16llx Closing Connection", m_info.id);

    // this->anc->timeConnected.tv_sec = this->anc->timeConnected.tv_usec = 0;
    // gettimeofday(&this->anc->timeDisconnected, NULL);
    // this->anc->ResetCCP();

    // if (this->anc)
    // {
    //     this->anc->socket = NULL;
    // }
    delete this; // Self destruct the instance once the connection is closed
}

size_t AnchorConnection::HandleUseStaticAddr(const char *rxBytes, size_t length)
{
    return length;
}

size_t AnchorConnection::HandleSetStaticAddr(const char *rxBytes, size_t length)
{
    return length;
}

void TryConnect::Run()
{
    HelperFrame *frame = wxGetApp().GetFrame();
    auto iter = frame->m_anc_new_ip.begin();
    while (iter != frame->m_anc_new_ip.end())
    {
        helper::types::AnchorInfo info{iter->first, iter->second};
        wxThreadEvent event{wxEVT_THREAD, helper::ids::PROGRESS_UPDATE};
        helper::types::StatusInfo status{iter->first, helper::types::Status::CONNECT_FAILED};
        if (frame->ConnectToAnchor(info))
        {
            status.status = helper::types::Status::CONNECTED;
        }
        event.SetPayload(status);
        wxQueueEvent(frame, event.Clone());
        ++iter;
    }
}
