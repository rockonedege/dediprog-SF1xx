#pragma once

#include <string>

class CVersionInfo
{
public:
    CVersionInfo(std::wstring s);
    CVersionInfo(unsigned int major, 
        unsigned int minor, 
        unsigned int revision,
        unsigned int build);
public:
    std::wstring to_string() const;
    bool operator< (const CVersionInfo& vi);
    bool operator> (const CVersionInfo& vi);
    bool operator== (const CVersionInfo& vi);

    unsigned int getMajor();

private:
    unsigned int m_major;
    unsigned int m_minor;
    unsigned int m_revision;
    unsigned int m_build;
};
