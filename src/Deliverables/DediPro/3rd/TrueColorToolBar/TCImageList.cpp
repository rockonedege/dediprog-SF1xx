// TCImageList.cpp: implementation of the CTCImageList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TCImageList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTCImageList::CTCImageList()
{

}

CTCImageList::~CTCImageList()
{

}

/* Creates image list from bitmap
	for each image: width = height = bitmap's height
					transparent color = white
*/
BOOL CTCImageList::CreateTC(UINT nID)
{
	// determine bitmap's size
	CBitmap res;
	if(!res.LoadBitmap(nID))
		return FALSE;

	BITMAP bmp;
	res.GetBitmap(&bmp);

	// heigh and width are equal for each image, thus the total width of the bitmap
	// must be a multiple of its height
	ASSERT(bmp.bmWidth%bmp.bmHeight == 0);

	return CreateTC(nID, bmp.bmHeight, bmp.bmHeight, RGB(255,255,255));
}

/* Creates image list from bitmap
	for each image: width = height = cx
					transparent color = white
*/
BOOL CTCImageList::CreateTC(UINT nID, int cx)
{
	return CreateTC(nID, cx, cx, RGB(255,255,255));
}

/* Creates image list from bitmap
	for each image: width = height = cx
					transparent color = colMask
*/
BOOL CTCImageList::CreateTC(UINT nID, int cx, COLORREF colMask)
{
	return CreateTC(nID, cx, cx, colMask);
}

/* Creates image list from bitmap
	for each image: width = cx
					height = cy
					transparent color = white
*/
BOOL CTCImageList::CreateTC(UINT nID, int cx, int cy)
{
	return CreateTC(nID, cx, cy, RGB(255,255,255));
}

/* Creates image list from bitmap
	for each image: width = height = bitmap's height
					transparent color = colMask
*/
BOOL CTCImageList::CreateTC(UINT nID, COLORREF colMask)
{
	CBitmap res;
	if(!res.LoadBitmap(nID))
		return FALSE;
	
	BITMAP bmp;
	res.GetBitmap(&bmp);

	ASSERT(bmp.bmWidth%bmp.bmHeight == 0);
	
	return CreateTC(nID, bmp.bmHeight, bmp.bmHeight, colMask);
}

/* Creates image list from bitmap
	for each image: width = cx
					height = cy
					transparent color = colMask
*/
BOOL CTCImageList::CreateTC(UINT nID, int cx, int cy, COLORREF colMask)
{
	if(!CImageList::Create(cx, cy,ILC_MASK | ILC_COLOR32,0,0))
		return FALSE;

	CBitmap bmp;
	if(!bmp.LoadBitmap(nID))
		return FALSE;

	Add(&bmp, colMask);
	
	return TRUE;
}
