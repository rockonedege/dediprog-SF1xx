
#include "stdafx.h"
#include "USBDevice.h"
#include "./Tddk203/inc/STTubeDeviceErr203.h"
#include "./Tddk203/inc/STTubeDeviceFun203.h"
#include "./Tddk203/inc/hidsdi.h"

#include <boost/scoped_array.hpp>
#include <boost/array.hpp>

#include <setupapi.h>


bool CUSB::CloseDevice()
{
    if(hDev != NULL){
        if(STDevice_ClosePipes(hDev) != STDEVICE_NOERROR)
            return false ;

        if(STDevice_Close(hDev) != STDEVICE_NOERROR)
            return false ;

        DeleteObject(hDev) ;
        hDev = 0 ;
        m_nbDeviceDetected = 0 ;
    }

    return true ;
}

bool CUSB::reset()
{
    return (STDevice_Reset(hDev) == STDEVICE_NOERROR) ;
}

// find device , get number found
unsigned int CUSB::FindUSBDevice()
{
    m_nbDeviceDetected = 0;

    int devIndex;
    DWORD result = 0;

    HDEVINFO DeviceInfoSet;

    DeviceInfoSet = SetupDiGetClassDevs(&usbInfo._guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );
    if(INVALID_HANDLE_VALUE == DeviceInfoSet) return 0;

    DWORD needed;
    SP_DEVICE_INTERFACE_DATA ifData;
    ifData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    PSP_DEVICE_INTERFACE_DETAIL_DATA detail; 
    for (devIndex=0; SetupDiEnumDeviceInterfaces(DeviceInfoSet, NULL, &usbInfo._guid, devIndex, &ifData); ++devIndex)
    {
        /******************************************************************************
        'SetupDiGetDeviceInterfaceDetail
        'Returns: an SP_DEVICE_INTERFACE_DETAIL_DATA structure
        'containing information about a device.
        'To retrieve the information, call this function twice.
        'The first time returns the size of the structure in Needed.
        'The second time returns a pointer to the data in DeviceInfoSet.
        'Requires:
        'A DeviceInfoSet returned by SetupDiGetClassDevs and
        'an SP_DEVICE_INTERFACE_DATA structure returned by SetupDiEnumDeviceInterfaces.
        '*******************************************************************************/
        SetupDiGetDeviceInterfaceDetail( DeviceInfoSet, &ifData, NULL, 0, &needed, NULL);

        detail=(PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(needed);
        detail->cbSize=sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        /*'Call SetupDiGetDeviceInterfaceDetail again.
        'This time, pass the address of the first element of DetailDataBuffer
        'and the returned required buffer size in DetailData.*/
        SetupDiGetDeviceInterfaceDetail( DeviceInfoSet, &ifData, detail, needed, NULL, NULL);

#ifdef _UNICODE
        char pDevicePath[256] ;
        WideCharToMultiByte( CP_ACP, 0, detail->DevicePath, -1,
            pDevicePath, 256, NULL, NULL );

        result = STDevice_Open(pDevicePath, 
            &hDev, 
            NULL) ;
#else
        result = STDevice_Open(reinterpret_cast<LPSTR>(detail->DevicePath), 
            &hDev, 
            NULL) ;
#endif //_UNICODE

        free(detail) ;

        if(result != STDEVICE_NOERROR)
            continue ;

        // test capacity
        if(!GetTubeDeviceCapa())
            continue ;

        // open pipes
        if(STDevice_OpenPipes(hDev) != STDEVICE_NOERROR)
            continue ; //Error in retrieving Devive Handle with STDevice_Open

        USB_DEVICE_DESCRIPTOR UsbDeviceDesc ;
        //Clear the UsbDeviceDesc Structure
        ZeroMemory(&UsbDeviceDesc, sizeof(USB_DEVICE_DESCRIPTOR)) ;

        result = STDevice_GetDeviceDescriptor(hDev, &UsbDeviceDesc) ;
        result = STDevice_OpenPipes(hDev) ;

        //Error in filling USB_DEVICE_DESCRIPTOR structure.
        if(result != STDEVICE_NOERROR)
            continue ;

        //Find out if the device matches the one we're looking for.
        if(UsbDeviceDesc.idVendor == usbInfo.vendorID &&
            UsbDeviceDesc.idProduct == usbInfo.productID)
        {
            // find right device and open it
            if(STDevice_OpenPipes(hDev) == STDEVICE_NOERROR)
                m_nbDeviceDetected ++ ;
        }
        else
        {
            result = STDevice_ClosePipes(hDev) ;
            //If it's not the one we want, close its handle.
            result = STDevice_Close(hDev) ;
        }
    }
    //Free the memory reserved for the DeviceInfoSet returned by SetupDiGetClassDevs.
    SetupDiDestroyDeviceInfoList(DeviceInfoSet) ;

    return m_nbDeviceDetected ;
}

bool CUSB::GetTubeDeviceCapa()  const
{
    DWORD result = 0 ;
    UINT lngNbOfConfig ;
    UINT lngNbOfInterface ;
    UINT lngNbOfEndPoint ;

    // number of configurations
    result = STDevice_GetNbOfConfigurations(hDev, &lngNbOfConfig) ;

    if(result != STDEVICE_NOERROR)
        return false ;

    // configuration descriptor 
    // allocate usbconfigurationdesc first !
    USB_CONFIGURATION_DESCRIPTOR UsbConfigurationDesc ;
    result = STDevice_GetConfigurationDescriptor(hDev, 
        lngNbOfConfig - 1,
        &UsbConfigurationDesc) ;

    if(result != STDEVICE_NOERROR)
        return false ;

    result = STDevice_GetNbOfInterfaces(hDev,
        lngNbOfConfig - 1,
        &lngNbOfInterface) ;

    if(result != STDEVICE_NOERROR)
        return false ;

    USB_INTERFACE_DESCRIPTOR UsbInterfaceDesc ;
    result = STDevice_GetInterfaceDescriptor(hDev,
        lngNbOfConfig - 1,
        lngNbOfInterface - 1,
        0,
        &UsbInterfaceDesc) ;

    if(result != STDEVICE_NOERROR)
        return false ;

    result = STDevice_GetNbOfEndPoints(hDev,
        lngNbOfConfig - 1,
        lngNbOfInterface - 1,
        0,
        &lngNbOfEndPoint) ;

    if(result != STDEVICE_NOERROR)
        return false ;

    USB_ENDPOINT_DESCRIPTOR UsbEndPointDesc ;
    for(unsigned int idxEndPoint = 0; idxEndPoint < lngNbOfEndPoint; idxEndPoint ++)
    {
        result = STDevice_GetEndPointDescriptor(hDev,
            lngNbOfConfig - 1,
            lngNbOfInterface - 1,
            0,
            idxEndPoint,
            &UsbEndPointDesc) ;

        if(result != STDEVICE_NOERROR)
            return false ;
    }

    return true ;
}

// part of USB driver , open usb pipes for data transfor
// should be called after usb successfully opens pipes.
bool CUSB::StartAppli()
{
    CNTRPIPE_RQ rq ;
    vector<unsigned char> vInstruction(1) ;

    // special instruction 
    vInstruction[0] = 0 ;

    rq.Function = URB_FUNCTION_VENDOR_OTHER ;
    rq.Direction = VENDOR_DIRECTION_IN ;
    rq.Request = 0x0B ;
    rq.Value = 0x00 ;
    rq.Index = 0x00 ;
    rq.Length = 0x01 ;

    if(! OutCtrlRequest(rq, vInstruction))
        return false ;

    return true ;
}

unsigned int CUSB::open()
{
    CloseDevice();    
    reset();

    if(FindUSBDevice() == 0)
        return m_nbDeviceDetected ;

    // start appli , open pipes for communication
    if(! StartAppli())
        return 0 ;

    return m_nbDeviceDetected ;
}

// return size read 
// if fail , return -1
//long CUSB::BulkPipeRead(PBYTE pBuff, UINT sz, UINT timeOut) const
bool CUSB::BulkPipeRead(vector<unsigned char>& pBuff, unsigned int timeOut) const
{
	// boost::mutex::scoped_lock l(function_mutex);

	size_t cnRead = pBuff.size() ;
    boost::scoped_array<unsigned char> pData(new unsigned char[cnRead]) ;

    if( STDevice_Read(hDev, 
        0,
        pData.get(),
        &cnRead, timeOut) != STDEVICE_NOERROR )
        return false ;

    // copy data
    //buffer.resize(cnRead) ;
    copy(pData.get(), pData.get() + pBuff.size(), pBuff.begin()) ;

    return true ;
}

//long CUSB::BulkPipeWrite(PBYTE pBuff, UINT sz, UINT timeOut) const
bool CUSB::BulkPipeWrite(const vector<unsigned char>& pBuff, unsigned int timeOut) const
{    
	// boost::mutex::scoped_lock l(function_mutex);
    // allocate temporary buffer and copy data
    //boost::shared_array<BYTE> pData(new BYTE[buffer.size()]) ;
    // should be 512 bytes
    unsigned int nWrite = 512 ;
    boost::array<unsigned char, 512> pData;
    pData.assign(0xFF);         // fill buffer with 0xFF
    copy(pBuff.begin(), pBuff.end(), &pData[0]) ;

    if( STDevice_Write(hDev, 
        1,        //different from read(0)
        &pData[0],
        &nWrite,
        timeOut) != STDEVICE_NOERROR)
        return false ;

    // write fail
    if(nWrite != 512)
        return false ;

    return true ;
}

bool CUSB::OutCtrlRequest( CNTRPIPE_RQ& rq, const vector<unsigned char>&vData )
{
	// boost::mutex::scoped_lock l(function_mutex);

	boost::scoped_array<unsigned char> pData(new unsigned char[vData.size()]) ;
    copy(vData.begin(), vData.end(), pData.get()) ;

    if( STDevice_ControlPipeRequest(hDev, &rq, pData.get()) != STDEVICE_NOERROR)
    {
            return false ;
    }

    return true ;
}

bool CUSB::InCtrlRequest( CNTRPIPE_RQ& rq, vector<unsigned char>& vData )
{
	// boost::mutex::scoped_lock l(function_mutex);

	if(vData.size() == 0)
        return false ;

    boost::scoped_array<unsigned char> pData(new unsigned char[vData.size()]) ;

    if( STDevice_ControlPipeRequest(hDev, 
        &rq,
        pData.get()) != STDEVICE_NOERROR)
        return false ;

    copy(pData.get(), pData.get() + vData.size(), vData.begin()) ;

    // if no value returned, false 
    return vData.empty() ? false : true ;
}
