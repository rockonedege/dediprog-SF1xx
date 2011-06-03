#define STDEVICE_ERROR_OFFSET                0x12340000

#define STDEVICE_NOERROR                    STDEVICE_ERROR_OFFSET
#define STDEVICE_MEMORY                        (STDEVICE_ERROR_OFFSET+1)
#define STDEVICE_BADPARAMETER                (STDEVICE_ERROR_OFFSET+2)

#define STDEVICE_NOTIMPLEMENTED                (STDEVICE_ERROR_OFFSET+3)
#define STDEVICE_ENUMFINISHED                (STDEVICE_ERROR_OFFSET+4)
#define STDEVICE_OPENDRIVERERROR            (STDEVICE_ERROR_OFFSET+5)

#define STDEVICE_ERRORDESCRIPTORBUILDING    (STDEVICE_ERROR_OFFSET+6)
#define STDEVICE_PIPECREATIONERROR            (STDEVICE_ERROR_OFFSET+7)
#define STDEVICE_PIPERESETERROR                (STDEVICE_ERROR_OFFSET+8)
#define STDEVICE_PIPEABORTERROR                (STDEVICE_ERROR_OFFSET+9)
#define STDEVICE_STRINGDESCRIPTORERROR        (STDEVICE_ERROR_OFFSET+0xA)

#define STDEVICE_DRIVERISCLOSED                (STDEVICE_ERROR_OFFSET+0xB)
#define STDEVICE_VENDOR_RQ_PB                (STDEVICE_ERROR_OFFSET+0xC)
#define STDEVICE_ERRORWHILEREADING            (STDEVICE_ERROR_OFFSET+0xD)
#define STDEVICE_ERRORBEFOREREADING            (STDEVICE_ERROR_OFFSET+0xE)
#define STDEVICE_ERRORWHILEWRITING            (STDEVICE_ERROR_OFFSET+0xF)
#define STDEVICE_ERRORBEFOREWRITING            (STDEVICE_ERROR_OFFSET+0x10)
#define STDEVICE_DEVICERESETERROR            (STDEVICE_ERROR_OFFSET+0x11)
#define STDEVICE_CANTUSEUNPLUGEVENT            (STDEVICE_ERROR_OFFSET+0x12)
#define STDEVICE_INCORRECTBUFFERSIZE        (STDEVICE_ERROR_OFFSET+0x13)
#define STDEVICE_DESCRIPTORNOTFOUND            (STDEVICE_ERROR_OFFSET+0x14)
#define STDEVICE_PIPESARECLOSED                (STDEVICE_ERROR_OFFSET+0x15)
#define STDEVICE_PIPESAREOPEN                (STDEVICE_ERROR_OFFSET+0x16)
