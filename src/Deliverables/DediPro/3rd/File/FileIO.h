
// file operation 
// read file to buffer 
// supported file format : bin,hex,s19,ram ...

#ifndef _ST_FILE_H
#define _ST_FILE_H

#include <vector>
#include <string>

using std::wstring;

class CFileIO
{
public :

    enum FILE_FORMAT{
        BIN ,
        HEX,
        S19,
        RAM,
        UNKNOWN,
    } ;


    FILE_FORMAT GetFileFormatFromExt(const wstring& csPath) ;

    bool ReadBINFile(const wstring& csPath, std::vector<unsigned char>& buffer) ;
    bool ReadHEXFile(const wstring& csPath, std::vector<unsigned char>& buffer) ;
    bool ReadS19File(const wstring& csPath, std::vector<unsigned char>& buffer) ;
    bool ReadRAMFile(const wstring& csPath, std::vector<unsigned char>& buffer) ;

    bool WriteBINFile(const std::vector<unsigned char>& buffer, const wstring& csPath) ;
    bool WriteHEXFile(const std::vector<unsigned char>& buffer, const wstring& csPath) ;
    bool WriteS19File(const std::vector<unsigned char>& buffer, const wstring& csPath) ;
    bool WriteRAMFile(const std::vector<unsigned char>& buffer, const wstring& csPath) ;

    bool ReadByFileExt(const wstring& csPath, std::vector<unsigned char>& buffer) ;

    bool WriteByFileExt(const std::vector<unsigned char>& buffer,const wstring& csPath) ;

};


#endif    //_ST_FILE_H