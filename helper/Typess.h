#pragma once

#include <string>
#include <stdint.h>
#include <cstring>
#include "Socket.h"

namespace helper
{
    namespace ids
    {
        constexpr int ANCHOR_FOUND = 100;
        constexpr int ANCHOR_REMOVED = 101;
        constexpr int PROGRESS_UPDATE = 102;
    } // namespace ids

    namespace types
    {
        enum class Status
        {
            CONNECTED,
            CONNECT_FAILED,
            SETTING_IP,
            STATIC_MARK_OK,
            STATIC_MARK_FAILED,
            SET_STATIC_IP_OK,
            SET_STATIC_IP_FAILED,
            SET_NETMASK_OK,
            SET_NETMASK_FAILED,
            SET_GATEWAY_OK,
            SET_GATEWAY_FAILED,
            DONE,
        };

        struct Anchor
        {
            uint64_t id;
            bool selected;
            struct in_addr ip;
            struct in_addr ip_set;
            Status status;
            Socket* sock;

            bool b_mark_set = false;
            bool b_ip_set = false;
            bool b_netmask_set = false;
            bool b_gateway_set = false;

            Anchor(uint64_t id_, in_addr ip_, in_addr ip_set_ = { 0 }, bool selected_ = true, Status status_ = Status::CONNECTED, Socket *sock_=nullptr) :
                id(id_), ip(ip_), ip_set(ip_set_), selected(selected_), status(status_), sock(sock_)
            {
            }

            bool SetOk()
            {
                return b_mark_set && b_ip_set && b_netmask_set && b_gateway_set;
            }

            std::string GetIPString()
            {
                return inet_ntoa(ip_set);
            }
        };

        struct cmd_staticaddr_t
        {
            uint8_t command;
            uint8_t sub_type;

            uint8_t ip1;
            uint8_t ip2;
            uint8_t ip3;
            uint8_t ip4;

            uint8_t gw1;
            uint8_t gw2;
            uint8_t gw3;
            uint8_t gw4;

            uint8_t nm1;
            uint8_t nm2;
            uint8_t nm3;
            uint8_t nm4;

            cmd_staticaddr_t(uint8_t type, in_addr& ip, in_addr& gw, in_addr& nm):command(0x92), sub_type(type),
                ip1(ip.S_un.S_un_b.s_b1), ip2(ip.S_un.S_un_b.s_b2), ip3(ip.S_un.S_un_b.s_b3), ip4(ip.S_un.S_un_b.s_b4),
                gw1(gw.S_un.S_un_b.s_b1), gw2(gw.S_un.S_un_b.s_b2), gw3(gw.S_un.S_un_b.s_b3), gw4(gw.S_un.S_un_b.s_b4),
                nm1(nm.S_un.S_un_b.s_b1), nm2(nm.S_un.S_un_b.s_b2), nm3(nm.S_un.S_un_b.s_b3), nm4(nm.S_un.S_un_b.s_b4)
            {
            }

            cmd_staticaddr_t(uint8_t type) :command(0x92), sub_type(type){}

            in_addr GetIp()
            {
                return in_addr{ ip1, ip2, ip3, ip4 };
            }

            in_addr GetGW()
            {
                return in_addr{ gw1, gw2, gw3, gw4 };
            }

            in_addr GetNM()
            {
                return in_addr{nm1, nm2, nm3, nm4};
            }

            std::string GetIPString()
            {
                return inet_ntoa(GetIp());
            }

            std::string GetGWString()
            {
                return inet_ntoa(GetGW());
            }

            std::string GetNMString()
            {
                return inet_ntoa(GetNM());
            }
        };

        struct cmd_use_staticaddr_t
        {
            uint8_t command;
            uint8_t value;
            uint8_t sub_type;
        };
    } // namespace types

    namespace cmds
    {
        constexpr int RTLS_SET_GET_STATICADDR = 0x92;
        constexpr int RTLS_USE_STATICADDR = 0x93;
    } // namespace cmds

    namespace cons
    {
        constexpr int RTLS_SET_GET_STATICADDR_LEN = sizeof(helper::types::cmd_staticaddr_t);
        constexpr int RTLS_USE_STATICADDR_LEN = sizeof(helper::types::cmd_use_staticaddr_t);

        constexpr int COL_IDX_IDX = 0;
        constexpr int COL_IDX_SELECTED = 1;
        constexpr int COL_IDX_ANC_ID = 2;
        constexpr int COL_IDX_IP_ADDR = 3;
        constexpr int COL_IDX_STATUS = 4;
    } // namespace cons
} // namespace helper
