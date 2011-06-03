// DediProDoc.cpp : implementation of the CDediProDoc class
//

#include "stdafx.h"
#include "DediProDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDediProDoc

IMPLEMENT_DYNCREATE(CDediProDoc, CDocument)

BEGIN_MESSAGE_MAP(CDediProDoc, CDocument)
END_MESSAGE_MAP()


// CDediProDoc construction/destruction

CDediProDoc::CDediProDoc()
{
    // TODO: add one-time construction code here
}

CDediProDoc::~CDediProDoc()
{
}

BOOL CDediProDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)
    return TRUE;
}


// CDediProDoc serialization

void CDediProDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // TODO: add storing code here
    }
    else
    {
        // TODO: add loading code here
    }
}

BOOL CDediProDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace/* = TRUE*/)
{
    return true ;
}

// CDediProDoc diagnostics

#ifdef _DEBUG
void CDediProDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CDediProDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG
