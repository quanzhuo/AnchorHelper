#include "Discovery.h"
#include "Socket.h"
#include "SocketCompat.h"
#include "Shutdown.h"
#include "HelperApp.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <sys/types.h>

#define TypeBufferSize 80
#define kDNSServiceInterfaceIndexAny 0
#define LONG_TIME 100000000

static uint32_t opinterface = kDNSServiceInterfaceIndexAny;
static DNSServiceRef client = NULL;
static DNSServiceRef client_pa = NULL;
wxDECLARE_APP(HelperApp);
struct ServiceInfo
{
    Discovery *self;
    const char *type;
    const char *name;
    const char *host;
    const char *domain;
    struct in_addr ip;
    uint16_t port;
    const char *txt;
    uint16_t txtlen;
    uint64_t id;
};

static char *gettype(char *buffer, char *typ)
{
    if (!typ || !*typ || (typ[0] == '.' && typ[1] == 0))
        typ = "_http._tcp";
    if (!strchr(typ, '.'))
    {
        snprintf(buffer, TypeBufferSize, "%s._tcp", typ);
        typ = buffer;
    }
    return (typ);
}

void Discovery::HandleEvents()
{
    int dns_sd_fd = client ? DNSServiceRefSockFD(client) : -1;
    int dns_sd_fd2 = client_pa ? DNSServiceRefSockFD(client_pa) : -1;
    int nfds = dns_sd_fd + 1;
    static volatile int stopNow = 0;
    static volatile int timeOut = LONG_TIME;
    fd_set readfds;
    struct timeval tv;
    int result;
    stopNow = 0;
    if (dns_sd_fd2 > dns_sd_fd)
        nfds = dns_sd_fd2 + 1;

    while (!stopNow)
    {
        FD_ZERO(&readfds);

        // 2. Add the fd for our client(s) to the fd_set
        if (client)
            FD_SET(dns_sd_fd, &readfds);
        if (client_pa)
            FD_SET(dns_sd_fd2, &readfds);
        // 3. Set up the timeout.
        tv.tv_sec = timeOut;
        tv.tv_usec = 0;
        result = select(nfds, &readfds, (fd_set *)NULL, (fd_set *)NULL, &tv);
        if (result > 0)
        {
            DNSServiceErrorType err = kDNSServiceErr_NoError;
            if (client && FD_ISSET(dns_sd_fd, &readfds))
                err = DNSServiceProcessResult(client);
            else if (client_pa && FD_ISSET(dns_sd_fd2, &readfds))
                err = DNSServiceProcessResult(client_pa);

            if (err)
            {
                wxLogMessage("DNSServiceProcessResult returned %d", err);
                stopNow = 1;
            }
        }
        else
        {
            wxLogMessage("select() returned %d, errno %d: %s", result, errno, strerror(errno));
            if (errno != EINTR)
                stopNow = 1;
        }
    }
}

void DNSSD_API Discovery::AddrInfoReply(DNSServiceRef sdref, const DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *hostname, const struct sockaddr *address, uint32_t ttl, void *context)
{
    char addr[256] = "";
    ServiceInfo *s = (ServiceInfo *)context;
    DNSServiceFlags check_flags = flags;
    IN_ADDR *b = 0;

    if (address && address->sa_family == AF_INET)
    {
        b = (IN_ADDR *)&((struct sockaddr_in *)address)->sin_addr;
    }
    s->ip.s_addr = htonl(b->s_addr);
    s->ip.s_addr = ntohl(s->ip.s_addr);

    wxLogMessage("Anchor %16llx found on %s", s->id, inet_ntoa(s->ip));
    helper::types::AnchorInfo anc_info{s->id, s->ip};
    wxThreadEvent event{wxEVT_THREAD, helper::ids::ANCHOR_FOUND};
    event.SetPayload(anc_info);
    wxQueueEvent(wxGetApp().GetFrame(), event.Clone());

    if (sdref)
        DNSServiceRefDeallocate(sdref);
}

void DNSSD_API Discovery::ResolveReply(DNSServiceRef sdref, const DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorCode,
                                       const char *fullname, const char *hosttarget, uint16_t opaqueport, uint16_t txtLen, const unsigned char *txtRecord, void *context)
{
    uint32_t lotid = 0, partid = 0;
    DNSServiceFlags flag = 0;
    DNSServiceErrorType err;
    char gtxt[50] = {0};
    union
    {
        uint16_t s;
        u_char b[2];
    } port = {opaqueport};
    uint16_t PortAsNumber = ((uint16_t)port.b[0]) << 8 | port.b[1];

    ServiceInfo *s = (ServiceInfo *)context;
    const char *txtlocal = (const char *)txtRecord;

    if (!errorCode && txtLen > 1)
    {
        const unsigned char *ptr = txtRecord;
        const unsigned char *max = txtRecord + txtLen;
        int index = 0;
        while (ptr < max)
        {
            const unsigned char *const end = ptr + 1 + ptr[0];
            if (end > max)
                break;
            if (++ptr < end)
                wxLogMessage(" ");

            while (ptr < end)
            {
                if (*ptr >= ' ')
                {
                    gtxt[index] = *ptr;
                }
                ptr++;
                index++;
            }
        }
    }
    s->port = PortAsNumber;
    s->txt = txtlocal;
    s->host = hosttarget;
#ifdef _WIN32
    sscanf_s(gtxt, "lotid=0x%xpartid=0x%x", &lotid, &partid);
#elif __linux__
    sscanf(gtxt, "lotid=0x%xpartid=0x%x", &lotid, &partid);
#endif
    uint64_t id = lotid;
    id <<= 32;
    id |= partid;
    s->id = id;

    flag |= kDNSServiceFlagsReturnIntermediates;
#ifdef _WIN32
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
#endif
    DNSServiceRef servRef = NULL;
    err = DNSServiceGetAddrInfo(&servRef, flag, opinterface, kDNSServiceProtocol_IPv4, hosttarget, Discovery::AddrInfoReply, s);
    DNSServiceProcessResult(servRef);

    // Be sure to deallocate the DNSServiceRef when you're finished
    if (sdref)
        DNSServiceRefDeallocate(sdref);
}

void DNSSD_API Discovery::BrowseReply(DNSServiceRef sdref, const DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorCode,
                                      const char *replyName, const char *replyType, const char *replyDomain, void *context)
{
    struct ServiceInfo *s = (ServiceInfo *)malloc(sizeof(ServiceInfo));
    memset(s, 0, sizeof(*s));
    Discovery *self = (Discovery *)context;
    s->self = self;
    DNSServiceErrorType err;

    s->name = replyName;
    s->type = replyType;
    s->domain = replyDomain;
#ifdef _WIN32
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
#endif
    wxLogMessage("FD: %d, OP: %s, NAME: %s, type: %s, domain: %s\n", DNSServiceRefSockFD(sdref), ((flags & kDNSServiceFlagsAdd) ? "Add" : "Remove"), replyName, replyType, replyDomain);
    if (flags & kDNSServiceFlagsAdd)
    {
        DNSServiceRef servRef = NULL;
        err = DNSServiceResolve(&servRef, 0, opinterface, s->name, s->type, s->domain, Discovery::ResolveReply, s);
        DNSServiceProcessResult(servRef);
    }
}

Discovery::Discovery(HelperFrame *helper)
{
}

void Discovery::Run()
{
    DNSServiceErrorType err = 0;
    char buffer[TypeBufferSize], *typ;
#ifdef _WIN32
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
#endif
    wxLogMessage("MDNS query for _dw_rtls_anchor");

    typ = gettype(buffer, "_dw_rtls_anchor");
    err = DNSServiceBrowse(&client, 0, opinterface, typ, "local", Discovery::BrowseReply, this);
    HandleEvents();

    // Be sure to deallocate the DNSServiceRef when you're finished
    if (client)
        DNSServiceRefDeallocate(client);
    if (client_pa)
        DNSServiceRefDeallocate(client_pa);
}
