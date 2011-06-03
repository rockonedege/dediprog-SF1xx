#include "StdAfx.h"
#include "FirmwareFile.h"
#include "..\..\3rd\File\FileIO.h"
#include "util.h"
#include "versioninfo.h"

#include <vector>
#include <boost\algorithm\string.hpp>

using std::vector;
using boost::algorithm::split;
using boost::algorithm::iequals;
using boost::algorithm::is_any_of;


CFirmwareFile::CFirmwareFile(BoardInfo& info) 
: m_info(info)
, m_path(path(path_util::GetCommonAppDataPath() + L"firmware"))
{}


bool CFirmwareFile::isFileCorrect(const wstring& sFolder)
{
    CFileIO f;
    vector<unsigned char> v1, v2;


    f.ReadByFileExt((sFolder + L"1.bin").c_str(), v1);
    f.ReadByFileExt((sFolder + L"2.bin").c_str(), v2);

    if(v1.empty() || v2.empty()) 
        return false;
    else
        return true;
}

wstring CFirmwareFile::getLatestVersionPathPrefix(std::pair<wstring,wstring>& type_version)    //<boardtype, version>
{

    if (is_directory(m_path))
    {
        BoardInfo uptodate;
        vector<wstring> v;
        CVersionInfo version(m_info.version);

        for (directory_iterator itr(m_path); itr!= directory_iterator(); ++itr)
        {
            split(v, itr->path().stem().wstring(), is_any_of(L"-"));
            if(v.size() > 1)
            {
                if(iequals(m_info.board_type, v.front()))
                {
                    CVersionInfo refVer(v[1]);
                    if((version < refVer ) && (version.getMajor() == refVer.getMajor()))
                    {
                        type_version = std::make_pair(v[0], v[1]);

                        wstring ws(itr->path().wstring());
                        return ws.substr(0, ws.length() - 5);    
                    }
                }
            }

        }
    }

    return L"";
}
