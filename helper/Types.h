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
        };

        struct cmd_staticaddr_t
        {
            uint8_t command;
            uint8_t type;
            uint8_t sub_type;
            char addr[25];
            cmd_staticaddr_t(uint8_t c, uint8_t t, uint8_t s, const std::string& ip) :
                command(c), type(t), sub_type(s)
            {
                memcpy(addr, ip.c_str(), ip.size() + 1);
            }

            cmd_staticaddr_t(uint8_t c, uint8_t t, uint8_t s, const in_addr& in) :
                command(c), type(t), sub_type(s)
            {
                char *pc = inet_ntoa(in);
                memcpy(addr, pc, std::strlen(pc) + 1);
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
