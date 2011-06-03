// DediProDoc.h : interface of the CDediProDoc class
//


#pragma once

class CDediProDoc : public CDocument
{
protected: // create from serialization only
    CDediProDoc();
    DECLARE_DYNCREATE(CDediProDoc)

// Overrides
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
    virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace = TRUE);

// Implementation
public:
    virtual ~CDediProDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()
};


