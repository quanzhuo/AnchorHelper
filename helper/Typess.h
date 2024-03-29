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
        constexpr int IP_SCAN_FINISHED = 103;
    } // namespace ids

    namespace types
    {
        enum class Status
        {
            NOT_CONNECTED,
            CONNECTED,
            CONNECT_FAILED,
            STATIC_MARK_OK,
            STATIC_MARK_FAILED,
            DYNAMIC_MARK_OK,
            DYNAMIC_MARK_FAILED,
            SET_STATIC_IP_OK,
            SET_STATIC_IP_FAILED,
            DONE,
        };

        struct Anchor
        {
            uint64_t id;
            bool selected;
            struct in_addr ip;
            struct in_addr ip_set;
            Status status;
            Socket *sock;

            Anchor(uint64_t id_, in_addr ip_, in_addr ip_set_ = {0}, bool selected_ = true, Status status_ = Status::NOT_CONNECTED, Socket *sock_ = nullptr) : id(id_), ip(ip_), ip_set(ip_set_), selected(selected_), status(status_), sock(sock_)
            {
            }

            std::string GetIPSetString()
            {
                return inet_ntoa(ip_set);
            }

            std::string GetIPString()
            {
                return inet_ntoa(ip);
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

            cmd_staticaddr_t(uint8_t type, in_addr &ip, in_addr &gw, in_addr &nm) : command(0x92), sub_type(type)
            {
                uint32_t ip_int = *(uint32_t *)(&ip);
                ip1 = ip_int & 0xff;
                ip2 = (ip_int >> 8) & 0xff;
                ip3 = (ip_int >> 16) & 0xff;
                ip4 = (ip_int >> 24) & 0xff;

                ip_int = *(uint32_t *)(&gw);
                gw1 = ip_int & 0xff;
                gw2 = (ip_int >> 8) & 0xff;
                gw3 = (ip_int >> 16) & 0xff;
                gw4 = (ip_int >> 24) & 0xff;

                ip_int = *(uint32_t *)(&nm);
                nm1 = ip_int & 0xff;
                nm2 = (ip_int >> 8) & 0xff;
                nm3 = (ip_int >> 16) & 0xff;
                nm4 = (ip_int >> 24) & 0xff;
            }

            cmd_staticaddr_t(uint8_t type) : command(0x92), sub_type(type) {}

            in_addr GetIp()
            {
                struct in_addr temp;
                uint32_t i = ip1 + (ip2 << 8) + (ip3 << 16) + (ip4 << 24);
                memcpy(&temp, &i, 4);
                return temp;
            }

            in_addr GetGW()
            {
                struct in_addr temp;
                uint32_t i = gw1 + (gw2 << 8) + (gw3 << 16) + (gw4 << 24);
                memcpy(&temp, &i, 4);
                return temp;
            }

            in_addr GetNM()
            {
                struct in_addr temp;
                uint32_t i = nm1 + (nm2 << 8) + (nm3 << 16) + (nm4 << 24);
                memcpy(&temp, &i, 4);
                return temp;
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

        struct scaned_ips
        {
            unsigned n;
            std::string *ps;
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

        constexpr int BTN_ID_AUTO = 500;
    } // namespace cons
} // namespace helper
