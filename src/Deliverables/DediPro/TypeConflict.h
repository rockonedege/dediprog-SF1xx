#pragma once

#include ".\Core\Chip\DediInludes.h"
#include "memory_id.h"

namespace memory{struct memory_id;} 

// CTypeConflict dialog

class CTypeConflict : public CDialog
{
	DECLARE_DYNAMIC(CTypeConflict)

public:
    CTypeConflict(const vector<memory::memory_id>& ids);   // standard constructor
	virtual ~CTypeConflict();

// Dialog Data
	enum { IDD = IDD_TYPE_AMBIGUITY };

public:
    const memory::memory_id& GetChipId();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
    afx_msg void OnSelectType();
    virtual BOOL OnInitDialog();
    void FillList();
    void OnCancel(){};
    void OnOK();
private:
    CListBox m_listType;

    const vector<memory::memory_id>& m_ids;
    memory::memory_id m_selected;
public:
};
