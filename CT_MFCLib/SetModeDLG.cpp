// SetModeDLG.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"

#include "common++.h"
#include "tools++.h"
#include "SetModeDLG.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxl;
//using namespace jbxwl;



/////////////////////////////////////////////////////////////////////////////
// CSetModeDLG ダイアログ


//CSetModeDLG::CSetModeDLG(BOOL vcmode, BOOL clmode, BOOL pmmode, BOOL acmode, BOOL rzmode, int base, CWnd* pParent /*=NULL*/)
CSetModeDLG::CSetModeDLG(BOOL vcmode, BOOL rzmode, BOOL pmmode, BOOL acmode, int base, CWnd* pParent /*=NULL*/)
	: CDialog(CSetModeDLG::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetModeDLG)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_INIT

	vcMode = vcmode;	// ボリュームカラー
	clMode = TRUE;		// センターライン
	pmMode = pmmode;	// 分割データ転送
	acMode = acmode;	// 自動コントラスト
	rzMode = rzmode;
	fBase  = base;		// 一時的な基底値

	vcCBox = NULL;
	clCBox = NULL;
	pmCBox = NULL;
	acCBox = NULL;
	rzCBox = NULL;
	bsEBox = NULL;
}



void CSetModeDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetModeDLG)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CSetModeDLG, CDialog)
	//{{AFX_MSG_MAP(CSetModeDLG)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SET_VCOLOR,   &CSetModeDLG::OnBnClickedSetVcolor)
	//ON_BN_CLICKED(IDC_SET_CLINE,	&CSetModeDLG::OnBnClickedSetCline)
	ON_BN_CLICKED(IDC_SET_RVRSZ,	&CSetModeDLG::OnBnClickedSetRvrsz)
	ON_BN_CLICKED(IDC_SET_DIVDRAW,  &CSetModeDLG::OnBnClickedSetDivdraw)
	ON_BN_CLICKED(IDC_SET_ATCNTRST, &CSetModeDLG::OnBnClickedSetAtcntrst)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CSetModeDLG メッセージ ハンドラ


BOOL   CSetModeDLG::OnInitDialog()
{
	TCHAR buf[LMNAME];
//	memset(buf, 0, LMNAME);

	vcCBox = (CButton*)GetDlgItem(IDC_SET_VCOLOR);
//	clCBox = (CButton*)GetDlgItem(IDC_SET_CLINE);
	pmCBox = (CButton*)GetDlgItem(IDC_SET_DIVDRAW);
	acCBox = (CButton*)GetDlgItem(IDC_SET_ATCNTRST);
	rzCBox = (CButton*)GetDlgItem(IDC_SET_RVRSZ);
	bsEBox = (CEdit*)GetDlgItem(IDC_SET_FBASE);

	if (vcMode) vcCBox->SetCheck(1);
	else        vcCBox->SetCheck(0);
//	if (clMode) clCBox->SetCheck(1);
//	else        clCBox->SetCheck(0);
	if (pmMode) pmCBox->SetCheck(1);
	else        pmCBox->SetCheck(0);
	if (acMode) acCBox->SetCheck(1);
	else        acCBox->SetCheck(0);
	if (rzMode) rzCBox->SetCheck(1);
	else        rzCBox->SetCheck(0);

	sntprintf(buf, LMNAME, _T("%d"), fBase);
	bsEBox->SetWindowText(buf);

	return TRUE;
}



void CSetModeDLG::OnOK() 
{
	TCHAR buf[LMNAME];

	if (vcCBox->GetCheck()) vcMode = TRUE;
	else                    vcMode = FALSE;
//	if (clCBox->GetCheck()) clMode = TRUE;
//	else                    clMode = FALSE;
	if (pmCBox->GetCheck()) pmMode = TRUE;
	else                    pmMode = FALSE;
	if (acCBox->GetCheck()) acMode = TRUE;
	else                    acMode = FALSE;
	if (rzCBox->GetCheck()) rzMode = TRUE;
	else                    rzMode = FALSE;
	
	bzero(buf, LMNAME);
	bsEBox->GetWindowText(buf, LMNAME-1);
	fBase = ttoi(buf);

	CDialog::OnOK();
}



/*
void CSetModeDLG::OnBnClickedSetCline()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
}
*/


void CSetModeDLG::OnBnClickedSetVcolor()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
}


void CSetModeDLG::OnBnClickedSetDivdraw()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
}


void CSetModeDLG::OnBnClickedSetAtcntrst()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
}


void CSetModeDLG::OnBnClickedSetRvrsz()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
}
