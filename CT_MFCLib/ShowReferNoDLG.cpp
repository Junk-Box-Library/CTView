// ShowReferNoDLG.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"

#include "common++.h"
#include "tools++.h"
#include "ShowReferNoDLG.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxl;
//using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CShowReferNoDLG ダイアログ

CShowReferNoDLG::CShowReferNoDLG(int data, int vol, int rgn, int mct, int tbn, CWnd* pParent)
	: CDialog(CShowReferNoDLG::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShowReferNoDLG)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_INIT

	nData	= data;
	nVol	= vol;
	nRgn	= rgn;
	nMCT 	= mct;
	nTBN	= tbn;

	datSBox = NULL;
	volSBox = NULL;
	rgnSBox = NULL;
	mctSBox = NULL;
	tbnSBox = NULL;
}


void CShowReferNoDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShowReferNoDLG)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShowReferNoDLG, CDialog)
	//{{AFX_MSG_MAP(CShowReferNoDLG)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CShowReferNoDLG メッセージ ハンドラ

BOOL   CShowReferNoDLG::OnInitDialog()
{
	TCHAR buf[LNAME];
//	memset(buf, 0, LNAME);

	datSBox = (CStatic*)GetDlgItem(IDC_REFNO_DATA);
	volSBox	= (CStatic*)GetDlgItem(IDC_REFNO_VOL);
//	rgnSBox	= (CStatic*)GetDlgItem(IDC_REFNO_RGN);
	mctSBox	= (CStatic*)GetDlgItem(IDC_REFNO_MCT);
	tbnSBox	= (CStatic*)GetDlgItem(IDC_REFNO_TBN);

	sntprintf(buf, LNAME, _T("%d"), nData);
	datSBox->SetWindowText(buf);

	sntprintf(buf, LNAME, _T("%d"), nVol);
	volSBox->SetWindowText(buf);

	// 領域データ
//	sntprintf(buf, LNAME, _T("%d"), nRgn);
//	rgnSBox->SetWindowText(buf);

	sntprintf(buf, LNAME, _T("%d"), nMCT);
	mctSBox->SetWindowText(buf);

	sntprintf(buf, LNAME, _T("%d"), nTBN);
	tbnSBox->SetWindowText(buf);

	return TRUE;
}
