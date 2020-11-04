#pragma once

#include "Thread.h"
#include "pthread.h"
#include "dns_sd.h"

#ifdef _WIN32
#define DNSSD_API __stdcall
#elif __linux__
#include <netinet/in.h>
#define DNSSD_API
typedef struct in_addr IN_ADDR;
#endif

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "HelperFrame.h"
#include "Types.h"

class Discovery : public Thread
{
public:
    Discovery(HelperFrame *helper);

    virtual void Run();

    static void HandleEvents();

    static void DNSSD_API AddrInfoReply(DNSServiceRef sdref, const DNSServiceFlags flags, uint32_t interfaceIndex,
                                        DNSServiceErrorType errorCode, const char *hostname, const struct sockaddr *address, uint32_t ttl, void *context);
    static void DNSSD_API BrowseReply(DNSServiceRef sdref, const DNSServiceFlags flags, uint32_t ifIndex,
                                      DNSServiceErrorType errorCode, const char *replyName, const char *replyType, const char *replyDomain, void *context);
    static void DNSSD_API ResolveReply(DNSServiceRef sdref, const DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorCode,
                                       const char *fullname, const char *hosttarget, uint16_t opaqueport, uint16_t txtLen, const unsigned char *txtRecord, void *context);
};
