#pragma once


namespace Context{struct CDediContext;}

// CMiscSettings dialog

class CMiscSettings : public CPropertyPage
{
	DECLARE_DYNAMIC(CMiscSettings)

public:
	CMiscSettings(Context::CDediContext& context);
	virtual ~CMiscSettings();

// Dialog Data
	enum { IDD = IDD_CONFIG_MISC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnSelectVcc(UINT id);

    void initVcc();
    void initVpp();

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
private:
    Context::CDediContext& m_context;
public:
};
