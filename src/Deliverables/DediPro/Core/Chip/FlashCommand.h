#pragma once

#include <boost/noncopyable.hpp>
#include <utility>
#include <vector>
using std::vector;
using std::pair;

class CUSB;

class CFlashCommand:boost::noncopyable
{
public:
    CFlashCommand(CUSB& usb);
private:
    CUSB&           m_usb;  ///< one and only CUSB instance
private:
    bool TransceiveOut(const vector<unsigned char>& v, bool has_result_in);
    bool TransceiveIn(vector<unsigned char>& v);
public:
    bool SendCommand_OutOnlyInstruction(const vector<unsigned char>& v);
    bool SendCommand_OneOutOneIn(const vector<unsigned char>& vOut, vector<unsigned char>& vIn);

    bool SendCommand_SetupPacketForBulkWrite(const pair<size_t, size_t>& AddrRange, unsigned char modeWrite);
    bool SendCommand_SetupPacketForAT45DBBulkWrite(const pair<size_t, size_t>& AddrRange, unsigned char modeWrite);
    bool SendCommand_SetupPacketForBulkRead(const pair<size_t, size_t>& AddrRange, unsigned char modeRead);
};