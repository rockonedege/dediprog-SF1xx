// TSI
#include "stdafx.h"
#include "TS25Lxx.h"
#include "..\USB\USBDevice.h"

CTS25Lxxx::CTS25Lxxx(CUSB& usb, const memory::memory_id& id)
    : CM25PExx(usb, id)
{

}
 
