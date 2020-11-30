#include "tools.h"
#include <string>
#include <vector>
#include <utility>
#include <sys/types.h>
#include <iostream>
#include "Typess.h"
#include "HelperApp.h"

#ifdef _WIN32
#include <windows.h>
#include <iphlpapi.h>

#pragma comment(lib, "Iphlpapi.lib")
#endif

wxDECLARE_APP(HelperApp);

bool ping(const std::string &ip)
{
    std::string cmd;

#ifdef __APPLE__
    cmd = "ping -c 1 -t 1 ";
#elif __linux__
    cmd = "ping -c 1 -W 1 ";
#else
    cmd = "ping -n 1 -w 1000 ";
#endif

    cmd.append(ip.c_str());
    int ret = system(cmd.c_str());
    if (ret == 0)
        return true;
    else
        return false;
}

bool ping_from_if(const std::string &interface, const std::string &ip)
{
    std::string cmd;
#ifdef __APPLE__
    cmd = "ping -b ";
    cmd.append(interface).append(" -c 1 -t 1 ");
#elif __linux__
    cmd = "ping -I ";
    cmd.append(interface).append(" -c 1 -W 1 ");
#else
    cmd = "ping -S ";
    cmd.append(interface).append(" -n 1 -w 1000 ");
#endif

    cmd.append(ip);
    int ret = system(cmd.c_str());
    if (ret == 0)
        return true;
    else
        return false;
}

void ip_scan(const std::string anc_ip, const unsigned n)
{
    std::string host_ip, host_nm;

// std::vector<std::pair<std::string, struct in_addr>> if_lists;
#if defined(__APPLE__) || defined(__linux__)
    struct ifaddrs *addrs, *iter;
    getifaddrs(&addrs);
    iter = addrs;
    while (iter)
    {
        if (iter->ifa_addr && iter->ifa_addr->sa_family == AF_INET)
        {
            struct sockaddr_in *sock = (struct sockaddr_in *)(iter->ifa_addr);
            struct in_addr ip = sock->sin_addr;
            std::string ip_str(inet_ntoa(ip));
            if (ip_str == "127.0.0.1")
            {
                iter = iter->ifa_next;
                continue;
            }
            sock = (struct sockaddr_in *)(iter->ifa_netmask);
            struct in_addr netmask = sock->sin_addr;
            std::string nm_str(inet_ntoa(netmask));

            std::string interface = iter->ifa_name;
            if (ping_from_if(interface, anc_ip))
            {
                host_ip = ip_str;
                host_nm = nm_str;
                wxLogMessage("host ip: %s, host netmask: %s", host_ip.c_str(), host_nm.c_str());
                break;
            }
        }
        iter = iter->ifa_next;
    }
    freeifaddrs(addrs);
#elif defined(_WIN32)
    DWORD Err, AdapterInfoSize = 0;
    PIP_ADAPTER_INFO pAdapterInfo, pAdapt;
    PIP_ADDR_STRING pAddrStr;

    if ((Err = GetAdaptersInfo(NULL, &AdapterInfoSize)) != 0)
    {
        if (Err != ERROR_BUFFER_OVERFLOW)
        {
            wxLogMessage("GetAdaptersInfo sizing failed with error %d", Err);
            return;
        }
    }

    if ((pAdapterInfo = (PIP_ADAPTER_INFO)GlobalAlloc(GPTR, AdapterInfoSize)) == NULL)
    {
        wxLogMessage("Memory allocation error");
        return;
    }

    if ((Err = GetAdaptersInfo(pAdapterInfo, &AdapterInfoSize)) != 0)
    {
        wxLogMessage("GetAdaptersInfo failed with error %d", Err);
        return;
    }

    pAdapt = pAdapterInfo;

    while (pAdapt)
    {
        pAddrStr = &(pAdapt->IpAddressList);

        bool host_ip_found = false;

        while (pAddrStr)
        {
            wxLogMessage("IP Address. . . . . . . . . : %s", pAddrStr->IpAddress.String);
            wxLogMessage("Subnet Mask . . . . . . . . : %s", pAddrStr->IpMask.String);

            std::string ip_str = pAddrStr->IpAddress.String;
            if (ip_str == "127.0.0.1")
            {
                pAddrStr = pAddrStr->Next;
                continue;
            }
            std::string nm_str = pAddrStr->IpMask.String;

            if (ping_from_if(ip_str, anc_ip))
            {
                host_ip = ip_str;
                host_nm = nm_str;
                host_ip_found = true;
                wxLogMessage("host ip: %s, host netmask: %s", host_ip.c_str(), host_nm.c_str());
                break;
            }

            pAddrStr = pAddrStr->Next;
        }

        if (host_ip_found)
        {
            break;
        }

        pAdapt = pAdapt->Next;
    }
#endif
    if (host_ip.empty())
    {
        wxLogMessage("Can not determine host ip, Please do not click “Populate ip” button continuously");
        return;
    }

    std::string *ps = new std::string[n];
    IPIter ip_iter(host_ip, host_nm);
    auto i = 0;
    while (ip_iter)
    {
        std::string ip = ip_iter.GetNext();
        if (ping(ip))
        {
            wxLogMessage("Ip %s is in use, skip it", ip.c_str());
        }
        else
        {
            ps[i++] = ip;
            if (i == n)
                break;
        }
    }

    wxThreadEvent event(wxEVT_THREAD, helper::ids::IP_SCAN_FINISHED);
    helper::types::scaned_ips info{n, ps};
    event.SetPayload(info);
    wxQueueEvent(wxGetApp().GetFrame(), event.Clone());
}

IPIter::IPIter(const std::string &ip, const std::string &nm)
{
    m_current = 1;

    int cidr = 0;
    auto first_dot = nm.find('.');
    auto second_dot = nm.find('.', first_dot + 1);
    auto last_dot = nm.find_last_of('.');
    unsigned int nm1 = std::stoul(nm.substr(0, nm.find('.')));
    unsigned int nm2 = std::stoul(nm.substr(first_dot + 1, second_dot - first_dot));
    unsigned int nm3 = std::stoul(nm.substr(second_dot + 1, last_dot - second_dot));
    unsigned int nm4 = std::stoul(nm.substr(last_dot + 1));
    unsigned nm_ = (nm1 << 24) + (nm2 << 16) + (nm3 << 8) + nm4;
    while (nm_ & 0x80000000)
    {
        ++cidr;
        nm_ <<= 1;
    }
    m_count = std::pow(2, 32 - cidr);

    uint32_t ip_int = inet_addr(ip.c_str());
    uint32_t nm_int = inet_addr(nm.c_str());
    unsigned first = ip_int & nm_int;
    field1 = first & 0xff;
    field2 = first >> 8 & 0xff;
    field3 = first >> 16 & 0xff;
    field4 = first >> 24 & 0xff;
}

const std::string IPIter::GetNext()
{
    unsigned char field4_ = m_current % 256;
    unsigned char field3_ = m_current / 256 + field3;
    unsigned char field2_ = m_current / 256 / 256 + field2;
    unsigned char field1_ = m_current / 256 / 256 / 256 + field1;

    std::string res;
    res.append(std::to_string(field1_)).append(".");
    res.append(std::to_string(field2_)).append(".");
    res.append(std::to_string(field3_)).append(".");
    res.append(std::to_string(field4_));
    ++m_current;
    return res;
}

IPIter::operator bool() const
{
    return m_current < m_count - 1;
}
