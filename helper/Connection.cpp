#include "HelperApp.h"
#include "HelperFrame.h"
#include "Connection.h"
#include "Types.h"
#include "DB.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

wxDECLARE_APP(HelperApp);

AnchorConnection::AnchorConnection(Socket *s, std::shared_ptr<helper::types::Anchor> &anc) : HandlerConnection(s), pa(anc)
{
    using namespace helper::cmds;
    using namespace helper::cons;
    addHandler(RTLS_SET_GET_STATICADDR, RTLS_SET_GET_STATICADDR_LEN, &AnchorConnection::HandleUseStaticAddr);
    addHandler(RTLS_USE_STATICADDR, RTLS_USE_STATICADDR_LEN, &AnchorConnection::HandleSetStaticAddr);
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

size_t AnchorConnection::HandleUseStaticAddr(const char *rxBytes, size_t length)
{
    using namespace helper::types;
    using namespace helper::ids;
    cmd_use_staticaddr_t* use_static = (cmd_use_staticaddr_t*)rxBytes;

    wxThreadEvent event(wxEVT_THREAD, PROGRESS_UPDATE);

    if (use_static->value)
        pa->status = Status::STATIC_MARK_OK;
    else
        pa->status = Status::STATIC_MARK_FAILED;

    event.SetPayload(pa);

    wxQueueEvent(wxGetApp().GetFrame(), event.Clone());

    return length;
}

size_t AnchorConnection::HandleSetStaticAddr(const char *rxBytes, size_t length)
{
    using namespace helper::types;
    using namespace helper::ids;
    cmd_staticaddr_t* net_config = (cmd_staticaddr_t*)rxBytes;

    wxThreadEvent event(wxEVT_THREAD, PROGRESS_UPDATE);

    if (net_config->type == 1) //ip
    {
        char* ip_str = inet_ntoa(pa->ip_set);
        if (!std::strcmp(ip_str, net_config->addr))
        {
            pa->status = Status::SET_STATIC_IP_OK;
        }
        else
        {
            pa->status = Status::SET_STATIC_IP_FAILED;
        }
    }
    else if (net_config->type == 2) //netmask
    {
        char* netmask_str = inet_ntoa((DB::GetDB().GetNetmask()));
        if (!std::strcmp(netmask_str, net_config->addr))
        {
            pa->status = Status::SET_NETMASK_OK;
        }
        else
        {
            pa->status = Status::SET_NETMASK_FAILED;
        }
    }
    else if (net_config->type == 3) //gateway
    {
        char* gateway_str = inet_ntoa((DB::GetDB().GetGateway()));
        if (!std::strcmp(gateway_str, net_config->addr))
        {
            pa->status = Status::SET_GATEWAY_OK;
        }
        else
        {
            pa->status = Status::SET_GATEWAY_FAILED;
        }
    }

    event.SetPayload(pa);

    wxQueueEvent(wxGetApp().GetFrame(), event.Clone());

    return length;
}

void TryConnect::Run()
{
    HelperFrame* frame = wxGetApp().GetFrame();

    auto iter = DB::GetDB().Begin();
    while (iter != DB::GetDB().End())
    {
        if ((*iter)->sock)
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

    iter = DB::GetDB().Begin();
    while (iter != DB::GetDB().End())
    {
        //1. set static ip mark
        using namespace helper::types;
        using namespace helper::cmds;
        cmd_use_staticaddr_t use_static{ RTLS_USE_STATICADDR, 1, 1 };
        std::string str((char*)&use_static, sizeof use_static);
        (*iter)->sock->SendBytes(str);

        //2. set netmask

        cmd_staticaddr_t set_netmask{ RTLS_SET_GET_STATICADDR, 2, 1, DB::GetDB().GetNetmask() };
        str.assign((char*)&set_netmask, sizeof set_netmask);
        (*iter)->sock->SendBytes(str);

        //3. set gateway
        cmd_staticaddr_t set_gateway{ RTLS_SET_GET_STATICADDR, 3, 1, DB::GetDB().GetGateway() };
        str.assign((char*)&set_gateway, sizeof set_gateway);
        (*iter)->sock->SendBytes(str);

        //4. set ip
        cmd_staticaddr_t set_static_ip{ RTLS_SET_GET_STATICADDR, 1, 1, (*iter)->ip_set };
        str.assign((char*)&set_static_ip, sizeof set_static_ip);
        (*iter)->sock->SendBytes(str);

        // 5. get static ip mark
        use_static.sub_type = 0;
        (*iter)->sock->SendBytes(str);

        //6. get netmask
        set_netmask.sub_type = 0;
        (*iter)->sock->SendBytes(str);

        //7. get gateway
        set_gateway.sub_type = 0;
        (*iter)->sock->SendBytes(str);

        //8. get ip
        set_static_ip.sub_type = 0;
        (*iter)->sock->SendBytes(str);

        ++iter;
    }
}
