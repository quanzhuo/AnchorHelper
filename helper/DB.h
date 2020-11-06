#pragma once

#include <vector>
#include <memory>
#include "Types.h"

using helper::types::Anchor;

class DB
{
public:
    std::shared_ptr<Anchor> FindAnchor(uint64_t id);
    void AddAnchor(std::shared_ptr<Anchor> pa);
    void RemoveAnchor(uint64_t id);
    in_addr& GetGateway() { return m_gateway; }
    in_addr& GetNetmask() { return m_netmask; }
    void SetGateway(const in_addr& g) { m_gateway = g; }
    void SetNetmask(const in_addr& n) { m_netmask = n; }
    std::vector<std::shared_ptr<Anchor>>::iterator Begin() { return m_ancs.begin(); }
    std::vector<std::shared_ptr<Anchor>>::iterator End() { return m_ancs.end(); }
    static DB& GetDB();



private:
    DB() :m_gateway{ 0 }, m_netmask{0} {};
    std::vector<std::shared_ptr<Anchor>> m_ancs;
    struct in_addr m_gateway;
    struct in_addr m_netmask;

    static DB db;
};