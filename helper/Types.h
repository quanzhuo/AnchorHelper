#pragma once

#include <string>
#include <stdint.h>
#include "Socket.h"

namespace helper
{
    namespace ids
    {
        constexpr int ANCHOR_FOUND = 100;
        constexpr int PROGRESS_UPDATE = 101;
    } // namespace ids

    namespace types
    {
        struct AnchorInfo
        {
            uint64_t id;
            struct in_addr ip;
        };

        enum class Status
        {
            CONNECTED,
            CONNECT_FAILED,
            SETTING_IP,
            DONE,
        };
        struct StatusInfo
        {
            uint64_t id;
            Status status;
        };

        struct cmd_staticaddr_t
        {
            uint8_t command;
            uint8_t type;
            uint8_t sub_type;
            char addr[25];
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
    } // namespace cons
} // namespace helper
