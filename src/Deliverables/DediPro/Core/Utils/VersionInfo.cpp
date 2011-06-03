#include "versioninfo.h"

#include <vector>
#include <string>
#include <sstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using std::vector;
using std::wstring;
using std::wstringstream;

// CVersionInfo
CVersionInfo::CVersionInfo(unsigned int major, 
        unsigned int minor, 
        unsigned int revision,
        unsigned int build)
        :m_major(major)
        ,m_minor(minor)
        ,m_revision(revision)
        ,m_build(build)
{}

CVersionInfo::CVersionInfo(std::wstring s)
        :m_major(0)
        ,m_minor(0)
        ,m_revision(0)
        ,m_build(0)
{
    vector<wstring> vS;
    wstringstream ss;
    boost::algorithm::split(vS, s, boost::algorithm::is_any_of(L"."));

    switch(vS.size())
    {
        case 4:
            ss.str(vS[3]); ss>>std::hex>>m_build; ss.clear();
        case 3:
            ss.str(vS[2]); ss>>std::hex>>m_revision; ss.clear();
        case 2:
            ss.str(vS[1]); ss>>std::hex>>m_minor; ss.clear();
        case 1:
            ss.str(vS[0]); ss>>std::hex>>m_major; ss.clear();
        default:
            ;
    }
}

bool CVersionInfo::operator< (const CVersionInfo& vi)
{
    if(m_major != vi.m_major) return m_major < vi.m_major; 
    if(m_minor != vi.m_minor) return m_minor < vi.m_minor; 
    if(m_revision != vi.m_revision) return m_revision < vi.m_revision; 
    if(m_build != vi.m_build) return m_build < vi.m_build; 


    return false;
}

bool CVersionInfo::operator> (const CVersionInfo& vi)
{
    if(m_major != vi.m_major) return m_major > vi.m_major; 
    if(m_minor != vi.m_minor) return m_minor > vi.m_minor; 
    if(m_revision != vi.m_revision) return m_revision > vi.m_revision; 
    if(m_build != vi.m_build) return m_build > vi.m_build; 

    //if(m_major > vi.m_major) return true; 
    //if(m_minor > vi.m_minor) return true; 
    //if(m_revision > vi.m_revision) return true; 
    //if(m_build > vi.m_build) return true; 

    return false;
}

bool CVersionInfo::operator== (const CVersionInfo& vi)
{
    if((m_major == vi.m_major) 
        && (m_minor == vi.m_minor)
        && (vi.m_revision == vi.m_build)) 
        return true; 

    return false;
}

unsigned int CVersionInfo::getMajor()
{
    return m_major;

}

wstring CVersionInfo::to_string() const
{
    wstringstream ss;

    ss << m_major
       << L"."
       << m_minor
       << L"."
       << m_revision
       << L"."
       << m_build;

    return ss.str();
}
