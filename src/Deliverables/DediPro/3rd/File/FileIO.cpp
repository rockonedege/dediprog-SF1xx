
#include "stdafx.h"            
#include "FileIO.h"
#include "IntelHexFile.h"
#include "MotorolaFile.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include <iterator>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

CFileIO::FILE_FORMAT CFileIO::GetFileFormatFromExt(const wstring& csPath)
{
    //int pos = csPath.ReverseFind('.') ;

    //if(pos < 0)
    //    return UNKNOWN ;

    //CString fmt = csPath.Right(csPath.GetLength() - pos - 1) ;
    //fmt.MakeUpper() ;

    boost::filesystem::path p(csPath);

    wstring fmt(p.extension().wstring());
    boost::to_upper(fmt);

    if(fmt == L".BIN") 
        return BIN ;
    else if(fmt == L".HEX") 
        return HEX ;
    else if(fmt == L".S19") 
        return S19 ;
    else if(fmt == L".RAM") 
        return RAM ;
    else
        return UNKNOWN ;
}

// read file
bool CFileIO::ReadByFileExt(const wstring& csPath, std::vector<unsigned char>& buffer)
{
    switch(GetFileFormatFromExt(csPath)){
        case BIN :
            return ReadBINFile(csPath, buffer) ;
        case HEX :
            return ReadHEXFile(csPath, buffer) ;
        case S19 :
            return ReadS19File(csPath, buffer) ;
        case RAM :
            return ReadRAMFile(csPath, buffer) ;
        default :
            return false ;
    }
} 

// write file
bool CFileIO::WriteByFileExt(const std::vector<unsigned char>& buffer, const wstring& csPath)
{
    switch(GetFileFormatFromExt(csPath)){
        case BIN :
            return WriteBINFile(buffer, csPath) ;
        case HEX :
            return WriteHEXFile(buffer, csPath) ;
        case S19 :
            return WriteS19File(buffer, csPath) ;
        case RAM :
            return WriteRAMFile(buffer, csPath) ;
        default :
            return WriteBINFile(buffer, csPath) ;
    }
}

// read bin 
bool CFileIO::ReadBINFile(const wstring& csPath, std::vector<unsigned char>& buffer)
{
    //ANSI only
    std::ifstream in(csPath.c_str(), std::ios::binary) ;
    if(in == 0L)
        return false ;

    // get file buffer
    std::stringstream ss ;
    ss << in.rdbuf() ; 
    in.close() ;

    // get length
    std::string strBuff(ss.str()) ;
    buffer.resize(strBuff.length()) ;
    std::copy(strBuff.c_str(), strBuff.c_str() + strBuff.length(), buffer.begin()) ;

    return true ;

}

// write bin
bool CFileIO::WriteBINFile(const std::vector<unsigned char>& buffer, const wstring& csPath)
{
    //ANSI only
    std::ofstream out(csPath.c_str(), std::ios::binary) ;
    if(out == 0L)
        return false ;

    std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<unsigned char>(out,"")) ;

    out.close() ;

    return true ;
}

// read hex
bool CFileIO::ReadHEXFile(const wstring& csPath, std::vector<unsigned char>& buffer)
{
    //ANSI only
    char szPath[MAX_PATH] = {0};

    WideCharToMultiByte( 
        CP_ACP,
        WC_COMPOSITECHECK | WC_DEFAULTCHAR,
        csPath.c_str(),
        csPath.size(),
        szPath, 
        MAX_PATH, 
        0,
        0
    );

    return (HexFileToBin(szPath, buffer)) ;
}

//write hex
bool CFileIO::WriteHEXFile(const std::vector<unsigned char>& buffer, const wstring& csPath)
{
    //FIXME
    //ANSI only
    //ANSI only
    //char szPath[MAX_PATH] = {0};

    //WideCharToMultiByte( 
    //    CP_ACP,
    //    WC_COMPOSITECHECK | WC_DEFAULTCHAR,
    //    csPath.GetString(),
    //    csPath.GetLength(),
    //    szPath, 
    //    MAX_PATH, 
    //    0,
    //    0
    //    );

    //return (BinToHexFile(szPath, buffer)) ;
    return (BinToHexFile(csPath, buffer)) ;

}

// read s19
bool CFileIO::ReadS19File(const wstring& csPath, std::vector<unsigned char>& buffer)
{
    //ANSI only
    char szPath[MAX_PATH] = {0};

    WideCharToMultiByte( 
        CP_ACP,
        WC_COMPOSITECHECK | WC_DEFAULTCHAR,
        csPath.c_str(),
        csPath.size(),
        szPath,
        MAX_PATH, 
        0,
        0
        );

    return (S19FileToBin(szPath, buffer)) ;

}

// write s19
bool CFileIO::WriteS19File(const std::vector<unsigned char>& buffer, const wstring& csPath)
{
    //FIXME
    //ANSI only
    //ANSI only
    char szPath[MAX_PATH] = {0};

    WideCharToMultiByte( 
        CP_ACP,
        WC_COMPOSITECHECK | WC_DEFAULTCHAR,
        csPath.c_str(),
        csPath.size(),
        szPath,
        MAX_PATH, 
        0,
        0
        );

    return (BinToS19File(szPath, buffer)) ;
}

// read ram
bool CFileIO::ReadRAMFile(const wstring& csPath, std::vector<unsigned char>& buffer)
{
    //FIXME
    //ANSI only

    return true ;

}

// write ram
bool CFileIO::WriteRAMFile(const std::vector<unsigned char>& buffer, const wstring& csPath)
{
    //FIXME
    //ANSI only

    return true ;
}
