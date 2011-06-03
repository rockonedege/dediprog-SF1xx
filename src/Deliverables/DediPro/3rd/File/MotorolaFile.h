

#ifndef _MOTOROLA_FILE_H
#define _MOTOROLA_FILE_H

#include <vector>
#include <string>
using std::string ;
using std::vector ;

bool S19FileToBin(const string& filePath, vector<unsigned char>& vData) ;
bool BinToS19File(const string& filePath, const vector<unsigned char>& vData) ;

#endif    //_MOTOROLA_FILE_H