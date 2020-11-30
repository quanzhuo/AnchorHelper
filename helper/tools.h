#include <string>
#include <vector>
#include <utility>
#include <sys/types.h>
#include <iostream>
#include <cmath>
#include <cstdint>

#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#elif defined(__linux__) || defined(__APPLE__)
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

bool ping(const std::string &ip);

/**
 * ping 时指定接口，即从指定的网络接口ping ip
 * @param interface: 从哪一个接口卡ping，对于 Windows 来说是源ip地址，对mac和linux来说是接口名称
 * @param ip: 要 ping的ip地址
 * @return true ping成功，false ping 失败
*/
bool ping_from_if(const std::string &interface, const std::string &ip);

/**
 * 根据基站ip，扫描本机网络，得到n个未分配的IP地址
 * @param anc_ip：基站的IP
 * @param n: 需要的IP的个数
*/
void ip_scan(const std::string anc_ip, const unsigned n);

class IPIter
{
public:
    IPIter(const std::string &ip, const std::string &nm);
    const std::string GetNext();
    operator bool() const;

private:
    unsigned m_count;
    unsigned m_current;

    //"10.206.25.230" --> "field1.field2.field3.field4"
    unsigned char field1;
    unsigned char field2;
    unsigned char field3;
    unsigned char field4;
};