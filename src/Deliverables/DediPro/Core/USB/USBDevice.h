

#ifndef _ST_USB_DEVICE_H
#define _ST_USB_DEVICE_H

#include "./Tddk203/inc/STTubeDeviceTyp203.h"
#include <windows.h>
#include <initguid.h>

#include <vector>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>

using std::vector ;

// usb information
typedef struct _argUSBInfo{
    GUID _guid ;                    ///< guid
    unsigned short productID ;      ///< product ID
    unsigned short vendorID ;       ///< vendor ID
}USB_INFO, *PUSBINFO ;

class CUSB /*: boost::noncopyable*/
{
    // handle
    HANDLE hDev;
    // detected device
    unsigned int m_nbDeviceDetected ;
    //usb information
    USB_INFO usbInfo ;

private :

    // get device capacity
    bool GetTubeDeviceCapa() const ;
    // close device
    bool CloseDevice() ;

    // find device and open it 
    unsigned int FindUSBDevice() ;

    // open device , return number of devices found
    unsigned int open() ;

    // get connected device number
    unsigned int NumOfUSBDevice() const{
        return m_nbDeviceDetected ;
    }

    // start appli
    // part of USB driver , open usb pipes for data transfor
    // should be called after usb successfully opens pipes.
    bool StartAppli() ;

public :
    // prevent constructors
    CUSB(const USB_INFO& _usbInfo) 
        : hDev(0)
        , m_nbDeviceDetected(0)
        , usbInfo(_usbInfo)
		//, class_mutex()
		//, function_mutex()
  //      , m_lock(class_mutex)
       {
           // open device when initiating
           open() ;
       }

       ~CUSB(){
           CloseDevice() ;
       }

       bool is_open(bool bRedetect = false) 
       {
          if(bRedetect) open(); 
          
          return (m_nbDeviceDetected > 0) ;
       }

       // reset device
       bool reset() ;

public :
    // read/write/control methods
    // add two new methods
    bool OutCtrlRequest( CNTRPIPE_RQ& , const vector<unsigned char>& );
    bool InCtrlRequest(CNTRPIPE_RQ&, vector<unsigned char>&  );

    // return value based on DWORD and pSize
    bool BulkPipeRead(vector<unsigned char>& pBuff, unsigned int timeOut) const ;
    // return value based on DWORD and pSize
    bool BulkPipeWrite(const vector<unsigned char>& pBuff, unsigned int timeOut) const ;

private:
    //mutable boost::mutex class_mutex;
    //mutable boost::mutex function_mutex;
    //boost::mutex::scoped_lock m_lock;

} ;
#endif    //_ST_USB_DEVICE_H