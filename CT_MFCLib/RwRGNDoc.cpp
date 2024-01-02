// RwRGNDoc.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"

#include "RwRGNDoc.h"
#include "ProgressBarDLG.h"
#include "WinTools.h"

#include "Branch.h"
#include "Tree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace jbxl;
using namespace jbxwl;



/////////////////////////////////////////////////////////////////////////////
// CRwRGNDoc

IMPLEMENT_DYNCREATE(CRwRGNDoc, CExDocument)


CRwRGNDoc::CRwRGNDoc()
{
	rgnData = NULL;
}

CRwRGNDoc::~CRwRGNDoc()
{
//	delete(rgnData);	
}

BEGIN_MESSAGE_MAP(CRwRGNDoc, CExDocument)
	//{{AFX_MSG_MAP(CRwRGNDoc)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRwRGNDoc 診断

#ifdef _DEBUG
void CRwRGNDoc::AssertValid() const
{
	CExDocument::AssertValid();
}


void CRwRGNDoc::Dump(CDumpContext& dc) const
{
	CExDocument::Dump(dc);
}
#endif //_DEBUG



/////////////////////////////////////////////////////////////////////////////
// CRwRGNDoc シリアライズ

void CRwRGNDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: この位置に保存用のコードを追加してください
	}
	else
	{
		// TODO: この位置に読み込み用のコードを追加してください
	}
}



/////////////////////////////////////////////////////////////////////////////
// CRwRGNDoc コマンド

BOOL  CRwRGNDoc::ReadDataFile(LPCTSTR fname)
{
//	char* fname = double_bs(str);
	CString fnm = get_file_name_t(fname);	// ファイル名部分
	bool  error = false;
		
	Title = fnm;
//	Buffer tmp = tstring2Buffer(fname);
	rgnData = readRgnFileWithCounter(fname);
//	free_Buffer(&tmp);

	// エラー処理
	if (rgnData==NULL) error = true;
	else if (rgnData->no<=0 || rgnData->rs==NULL) error = true;
	if (error) {
		CString message;
		//
		CString mesg, noti;
		mesg.LoadString(IDS_STR_ERR_OPEN_FILE);
		noti.LoadString(IDS_STR_ERROR);
		message = _T("CRwRGNDoc::ReadDataFile\n\n") + mesg + _T("  ") + fnm;
		MessageBox(pFrame->m_hWnd, message, noti, MB_OK);
		pFrame->doneErrorMessage = TRUE;
//		freeNull(fname);
		return FALSE;
	}
	anyData = (void*)rgnData;

//	print_debug_message("CRwRGNDoc::ReadDataFile: 領域数 = %d \n", rgnData->no);
//	freeNull(fname);
	return  TRUE;
}


//
//
int  writeRgnFileWithCounter(LPCTSTR dname, RgnData** rgnData, int rgnDataNum)
{
	// グローバルカウンタの生成
	CProgressBarDLG* counter = new CProgressBarDLG(IDD_PROGBAR, NULL, TRUE);	// ディスパッチャー有効
	char* mbstr = NULL;

	if (counter!=NULL) { 
		SetGlobalCounter(counter);
		CString mesg;
		mesg.LoadString(IDS_STR_WRTNG_FILE);

		mbstr = ts2mbs((LPCTSTR)mesg);
		counter->Start(0, mbstr);
		::free(mbstr);
	}

	mbstr = ts2mbs(dname);
	int ret = writeRgnFile(mbstr, rgnData, rgnDataNum, true);
	::free(mbstr);

	// グローバルカウンタの削除
	if (counter!=NULL) {
		counter->Stop();
		ClearGlobalCounter();
		delete counter;
	}

	return ret;
}


RgnData*  readRgnFileWithCounter(LPCTSTR fname)
{
	RgnData* pRgnData;

	// グローバルカウンタの生成
	CProgressBarDLG* counter = NULL;
	counter = new CProgressBarDLG(IDD_PROGBAR, NULL, TRUE);	// ディスパッチャー有効
	char* mbstr = NULL;
	
	if (counter!=NULL) { 
		SetGlobalCounter(counter);
		CString mesg;
		mesg.LoadString(IDS_STR_RDNG_FILE);

		mbstr = ts2mbs((LPCTSTR)mesg);
		counter->Start(0, mbstr);
		::free(mbstr);
	}
	
	mbstr = ts2mbs(fname);
	pRgnData = readRgnFile(mbstr, true);
	::free(mbstr);

	// グローバルカウンタの削除
	if (counter!=NULL) {
		counter->Stop();
		ClearGlobalCounter();
		delete counter;
	}

	return pRgnData;
}
