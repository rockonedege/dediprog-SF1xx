#pragma once


namespace Context{ struct CDediContext; }
// CAutoSequence dialog
class CAutoSequence : public /*CDialog*/ CPropertyPage
{
	DECLARE_DYNAMIC(CAutoSequence)

public:
    enum
    {
        AUTO_PROTECT    = 0x01,
        AUTO_VERIFY     = 0x02,
        AUTO_PROGRAM    = 0x04,
        AUTO_ERASE      = 0x08,
        AUTO_BLANKCHK   = 0x10,
        AUTO_RELOAD     = 0x20,
        AUTO_PREREAD    = 0x40
    };

public:
	CAutoSequence(Context::CDediContext& context);   // standard constructor
	virtual ~CAutoSequence();

// Dialog Data
	enum { IDD = IDD_CONFIG_BATCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
private:
    void OnOption(UINT id);
    void SaveUserOptions();

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    void RetrieveSavedOptions();
    void InitListStyles();
    void FillFilesList();
    void FillSequencesList( size_t option );
    void OnToggleVerify();
    void OnToggleReloadFile();
    void OnOK();
    void AddOneSequenceRow(wstring s, UINT i = -1);
    void RemoveOneSequenceRow(UINT i/* = -1*/);
    void OnAddrChange(UINT id);

private:
    Context::CDediContext& m_context;
    CListCtrl m_lstSequences;
    CListCtrl m_lstFiles;
    BOOL m_requireVerify;
    BOOL m_requireReloadFile;
    CString m_addrFrom;

    unsigned int m_option;
};
