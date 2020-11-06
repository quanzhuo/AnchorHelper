#include "DB.h"

std::shared_ptr<Anchor> DB::FindAnchor(uint64_t id)
{
    auto iter = m_ancs.begin();
    while (iter != m_ancs.end())
    {
        if ((*iter)->id == id)
            return *iter;
        ++iter;
    }
    return nullptr;
}

void DB::AddAnchor(std::shared_ptr<Anchor> pa)
{
    auto iter = m_ancs.begin();
    bool exists = false;
    while (iter != m_ancs.end())
    {
        if (pa->id == (*iter)->id)
        {
            exists = true;
            break;
        }
        ++iter;
    }
    if (!exists)
        m_ancs.push_back(pa);
}

void DB::RemoveAnchor(uint64_t id)
{
    auto iter = m_ancs.begin();
    while (iter != m_ancs.end())
    {
        if ((*iter)->id == id)
        {
            m_ancs.erase(iter);
            break;
        }
        ++iter;
    }
}

DB& DB::GetDB()
{
    return db;
}

DB DB::db{};