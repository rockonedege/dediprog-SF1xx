#define _BIND_TO_CURRENT_VCLIBS_VERSION 1 

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <boost\array.hpp>


using std::stringstream;
using std::ofstream;
using std::string;

void GenFile(size_t size, char c)
{
    stringstream fileName;
    if(size < (1<<20))
    {
        fileName << (size>>10) <<"Kbit.bin";
    }
    else
    {
        if(size & ((1<<20) - 1))
        {
            fileName << (size>>20) <<"Mbit+.bin";
        }
        else
        {
            fileName << (size>>20) << "Mbit.bin";
        }
    }

    ofstream of(fileName.str().c_str(), std::ios::binary);

    size>>=3;

    string s0("\x1\x2\x3\x4\x5\x6\x7\x8\x9\xa\xb\xc\xd\xe\xf\x11\x12\x13\x14\x15\x16\x17\x18\x19\x20");
    size -= 0x20;
    string s1(size>>1, c);
    string s2(size>>1, c^0xFF);

    of.write(s0.c_str(), 0x20);
    of.write(s1.c_str(), size>>1);
    of.write(s2.c_str(), size>>1);
}

int main()
{
    boost::array<size_t,9> a = {1<<19, 1<<20, 1<<21, 1<<22, 1<<23, 1<<24, 1<<25, 1<<26, 1<<27};

    for(unsigned int i = 0; i < a.size(); i++)
    {
        GenFile(a[i], '\xAA');
        GenFile(a[i] + a[i]/256*8, '\xAA');
    }
	return 0;
}
