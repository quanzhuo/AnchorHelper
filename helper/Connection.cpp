#include "HelperApp.h"
#include "HelperFrame.h"
#include "Connection.h"
#include "Typess.h"
#include "DB.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

wxDECLARE_APP(HelperApp);

AnchorConnection::AnchorConnection(Socket* s, std::shared_ptr<helper::types::Anchor>& anc) : HandlerConnection(s), pa(anc)
{
    using namespace helper::cmds;
    using namespace helper::cons;
    addHandler(RTLS_USE_STATICADDR, RTLS_USE_STATICADDR_LEN, &AnchorConnection::HandleUseStaticAddr);
    addHandler(RTLS_SET_GET_STATICADDR, RTLS_SET_GET_STATICADDR_LEN, &AnchorConnection::HandleSetStaticAddr);
}

void AnchorConnection::Run()
{
    HandlerConnection<AnchorConnection>::Run();
    wxLogMessage("ANC %16llx Closing Connection", pa->id);

    // this->anc->timeConnected.tv_sec = this->anc->timeConnected.tv_usec = 0;
    // gettimeofday(&this->anc->timeDisconnected, NULL);
    // this->anc->ResetCCP();

    // if (this->anc)
    // {
    //     this->anc->socket = NULL;
    // }
    delete this; // Self destruct the instance once the connection is closed
}

size_t AnchorConnection::HandleUseStaticAddr(const char* rxBytes, size_t length)
{
    using namespace helper::types;
    using namespace helper::ids;
    cmd_use_staticaddr_t* use_static = (cmd_use_staticaddr_t*)rxBytes;

    wxThreadEvent event(wxEVT_THREAD, PROGRESS_UPDATE);


    if (DB::GetDB().IsUseDHCP())
    {
        if (use_static->value)
        {
            pa->status = Status::DYNAMIC_MARK_FAILED;
        }
        else
        {
            pa->status = Status::DYNAMIC_MARK_OK;
        }
    }
    else
    {
        if (use_static->value)
        {
            pa->status = Status::STATIC_MARK_OK;
        }
        else
        {
            pa->status = Status::STATIC_MARK_FAILED;
        }
    }

    event.SetPayload<std::shared_ptr<helper::types::Anchor>>(pa);
    wxQueueEvent(wxGetApp().GetFrame(), event.Clone());
    return length;
}

size_t AnchorConnection::HandleSetStaticAddr(const char* rxBytes, size_t length)
{
    using namespace helper::types;
    using namespace helper::ids;
    cmd_staticaddr_t* net_config = (cmd_staticaddr_t*)rxBytes;

    wxThreadEvent event(wxEVT_THREAD, PROGRESS_UPDATE);

    wxLogMessage("received static addr, ip:%s, gw: %s, nm: %s",
                 net_config->GetIPString().c_str(), net_config->GetGWString().c_str(), net_config->GetNMString().c_str());

    if (net_config->GetIPString() == pa->GetIPSetString() &&
        net_config->GetGWString() == DB::GetDB().GetGWString() &&
        net_config->GetNMString() == DB::GetDB().GetNMString())
    {
        pa->status = Status::SET_STATIC_IP_OK;
    }

    event.SetPayload(pa);
    wxQueueEvent(wxGetApp().GetFrame(), event.Clone());

    return length;
}

void TryConnect::Run()
{
    HelperFrame* frame = wxGetApp().GetFrame();

    // connect to all selected anchors that haven't connected
    auto iter = DB::GetDB().Begin();
    while (iter != DB::GetDB().End())
    {
        if (!(*iter)->selected || (*iter)->sock)
        {
            ++iter;
            continue;
        }

        if (frame->ConnectToAnchor(*iter))
        {
            (*iter)->status = helper::types::Status::CONNECTED;
        }
        else
        {
            (*iter)->status = helper::types::Status::CONNECT_FAILED;
        }

        wxThreadEvent event{ wxEVT_THREAD, helper::ids::PROGRESS_UPDATE };
        event.SetPayload(*iter);
        wxQueueEvent(frame, event.Clone());
        ++iter;
    }


    // send command
    iter = DB::GetDB().Begin();
    while (iter != DB::GetDB().End())
    {
        using namespace helper::types;

        // first check whether anchor is selected and the socket is established
        if (!(*iter)->selected || !(*iter)->sock)
        {
            ++iter;
            continue;
        }

        if (DB::GetDB().IsUseDHCP())
        {
            // set dhcp
            cmd_use_staticaddr_t cmd{ 0x93, 0, 1 };
            std::string cmd_str((char*)&cmd, sizeof cmd);
            (*iter)->sock->SendBytes(cmd_str);

            wxMilliSleep(1000);

            // Get
            cmd.sub_type = 0;
            cmd_str.assign((char*)&cmd, sizeof cmd);
            (*iter)->sock->SendBytes(cmd_str);
        }
        else
        {
            //1. Set static ip
            cmd_staticaddr_t cmd{ 1, (*iter)->ip_set, DB::GetDB().GetGateway(), DB::GetDB().GetNetmask() };
            std::string cmd_str((char*)&cmd, sizeof cmd);
            (*iter)->sock->SendBytes(cmd_str);

            // sleep a while
            wxMilliSleep(1000);

            //2. Get
            cmd_staticaddr_t cmd_get{ 0 };
            std::string cmd_str_get((char*)&cmd_get, sizeof cmd_get);
            (*iter)->sock->SendBytes(cmd_str_get);
        }

        ++iter;
    }
}
