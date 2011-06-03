#ifndef _PROJECT_H
#define _PROJECT_H

#include "stdafx.h"

#include ".\3rd\File\FileIO.h"
#include "SerialFlash.h"
#include "USBDevice.h"

#include "memory_id.h"
#include "DediContext.h"

#include "IProject.h"


#include <boost/utility.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/logic/tribool.hpp>
BOOST_TRIBOOL_THIRD_STATE(recognized_not_supported)


#include "SerialFlashInfoEx.h"
#include "moduleversion.h"


using boost::scoped_ptr;
using boost::tribool;


namespace boost{ class thread; }


class CProject:boost::noncopyable, public  IProject
{
public:
    typedef enum
    {
        BLANKCHECK_WHOLE_CHIP,
        PROGRAM_CHIP_WITH_OR_WITHOUT_PADDINGS,
        UPDATE_PARTIAL_CHIP_WITH_CURRENT_BUFFER,
        ERASE_WHOLE_CHIP,
        READ_WHOLE_CHIP,
        READ_ANY_BY_PREFERENCE_CONFIGURATION,
        VERIFY,
        AUTO,

        Download2Card,

    } OPERATION_TYPE;

    typedef enum
    {
        GUI_USERS = 1,
        CONSOLE_USERS = 2,
        DLL_USERS = 3,
    } USER_GROUP;

public: 
    CProject(USER_GROUP user_group,  Context::STARTUP_MODE target_flash);
    ~CProject();
public:
    Context::CDediContext& get_context(){ return m_context;}

public:
    bool readIds(vector<memory::memory_id>& match ); 
public:
    bool readSR(unsigned char& cSR);
    bool writeSR(unsigned char cSR);
    bool setCrIfMCF();

public:
    const memory::memory_id& getChipId() const;
    double  getElapsedTime() const;
    size_t  getChipSizeInBytes();
    CUSB&   getUsbInstance();
    const programmer_board::board_info& getBoardInfo();
    wstring getImageFileInfo(DediVersion::IMAGEFILE_ATTRBUTES info);
    wstring getStatusString();   
    wstring getAllChipIdentifyingTrials() const;
    wstring getSizeOfLoadedFile() const;
    wstring getTypeDisplayName() const;
    vector<unsigned char>& getBufferForLastReadChipContents();
    vector<unsigned char>& getBufferForLoadedFile();
    pair<size_t, size_t> getLastUpLoadedRange() const;
    pair<size_t, size_t> getLastDownloadedRange() const;
    void CancelOperation();
    void setTargetFlash(Context::STARTUP_MODE mode);

public:
    bool Init() ;
    bool Init(const memory::memory_id& customId) ;
public:
    bool isFWUpdatable(std::pair<wstring,wstring>& type_version);
    bool UpdateFirmware();
    bool UpdateFirmware(const wstring& file);

public :
    void LoadFile(const wstring& path); 
    void LoadFile(const pair<wstring/* file path */, CFileIO::FILE_FORMAT/* file format*/>& fi);

    void Run(OPERATION_TYPE type);

    void UpdateWithExternalData(const vector<unsigned char>& vc);

public :
    bool is_usbWorking(bool bReDetectUSB = false);
    bool is_good(bool bReDetectUSB = false);
    tribool is_readyToDownload();
    bool is_functions_locked() const; 
    bool is_operationOnGoing() const;
    bool is_operationSuccessful() const;
    bool is_SF300();
    bool is_BoardVersionGreaterThan_1_1_6();
    bool is_BoardVersionGreaterThan_3_0_0();

    bool DownloadTFIT(vector<unsigned char>& vc);
    bool batchUpdate();
public: /// temp buffers
    pair<size_t, size_t>            m_downAddrRange ;       // download address range
    pair<size_t, size_t>            m_upAddrRange ;         // upload address range
    vector<unsigned char>           m_vprjDownloadBuffer ;

private:
    memory::memory_id GetFirstDetectionMatch();

    wstring getDataCrcString(bool bWholeFile) const;
    bool CreateChipInstance();
    bool UpdateEEPROMOnCard();
    bool FetchProgramRangeFromUserPrefernce();
    bool ValidateAddrRangeBeforeProgram();
    bool ReloadLastFileFromPreferenceProfile();
    void ResetChipId();

    bool is_BoardVersionGreaterThan(const wstring&);
    bool is_memoryChipChangedAfterLastDetection();
    bool is_FileBufferEmpty();
private:
    bool DownloadFileUsingChipErase(bool bCheckBlank);
    bool DownloadFileUsingBatchErase();
    bool batchUpdateUsingSectorErase();
    bool batchUpdateUsingChipErase();
    bool BlazeUpdate();
    bool BatchCustom();

private:
    void RefreshVccValue();
    void ResetLock();
    power::VPP_VALUE SupportedVpp();
private:
    void Log(const wstring&  line);

private:
    void LoadTFIT( std::vector< unsigned char >& vTFIT,size_t iSize, bool bSupportVPP, bool bAT45, bool bNewVersion);
    void LoadTFIT( std::vector< unsigned char >& vTFIT, bool bNewVersion);
    void UpdateOpInfo(vector<unsigned char>& v, size_t targetSize, unsigned char sequence);
    void UpdateOpInfo(vector<unsigned char>& v);

private:
    bool LoadFileWithInfo(const pair<wstring/* file path */, CFileIO::FILE_FORMAT/* file format*/>& fi);
    bool LoadFileByExtension(const wstring& path); 
 
    bool ProgramAnywhereNoUserSpecifiedPadding();
    bool ProgramAnywhereWithUserSpecifiedPadding();
 
    bool threadProgram(bool bValidateParameters);
    void ThreadLoadFileByExtension(const wstring& path); 
    void threadLoadFileWithInfo(const pair<wstring/* file path */, CFileIO::FILE_FORMAT/* file format*/>& fi); 
    bool threadReadPartialChipData(const pair<size_t/* starting addr */,size_t/* data size */>& range);
    bool threadReadWholeChipData();
    bool ThreadReadAnyByPreferenceConfiguration();
    bool threadCompareFileAndChip();
    bool threadBlankCheck();
    bool threadEraseWholeChip();
    bool threadSADownload();
    bool threadPredefinedBatchSequences();

    void ThreadUpdateWithExternalData(vector<unsigned char>& vc);
private:
    void threadRun(OPERATION_TYPE opType);
private:
    scoped_ptr<boost::thread>      m_thread;
private:
    Context::CDediContext           m_context;
    CUSB                            m_usb ;
    scoped_ptr<CSerialFlash>        m_chip ;
    scoped_ptr<programmer_board::CProgBoard>          m_board ;
    memory::memory_id               m_chip_id;
private:
    pair<wstring/* file path */, CFileIO::FILE_FORMAT/* file format*/>   m_imageFileInfo;
    wstring                         m_allChipIdentifyingTrials;
    wstring                         m_AllMatchesInString;
private:
    vector<unsigned char>           m_vBufferForLastReadData ;
    vector<unsigned char>           m_vBufferforLoadedFile ;

private:
    USER_GROUP                      m_user_group;

private:
    template <typename T>
    bool tryChipType()
    {
        if(boost::algorithm::iequals(m_chip_id.Class, T::getClassName()))
        {
            m_chip.reset(new T(m_usb, m_chip_id));
            return m_chip->is_good();
        }
        else
        {
            return false;
        }
    }
} ;

#endif    //_PROJECT_H