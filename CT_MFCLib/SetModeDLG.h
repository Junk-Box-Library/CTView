#if !defined(AFX_SETMODEDLG_H__4594B973_77C8_4085_873E_9EC49B2F0A9D__INCLUDED_)
#define AFX_SETMODEDLG_H__4594B973_77C8_4085_873E_9EC49B2F0A9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetModeDLG.h : ヘッダー ファイル
//

#include  "resource.h"


/////////////////////////////////////////////////////////////////////////////
// CSetModeDLG ダイアログ

class CSetModeDLG : public CDialog
{
public:
	BOOL		vcMode;
	BOOL		clMode;
	BOOL		pmMode;
	BOOL		acMode;
	BOOL		rzMode;
	int			fBase;

	CButton*	vcCBox;
	CButton*	clCBox;
	CButton*	pmCBox;
	CButton*	acCBox;
	CButton*	rzCBox;
	CEdit*		bsEBox;

// コンストラクション
public:
	// 標準のコンストラクタ
	//CSetModeDLG(BOOL vcmode=FALSE, BOOL clmode=TRUE, BOOL pmmode=TRUE, BOOL acmode=FALSE, BOOL rzmode=FALSE, int base=0, CWnd* pParent=NULL);
	CSetModeDLG(BOOL vcmode=FALSE,  BOOL rzmode=FALSE, BOOL pmmode=TRUE, BOOL acmode=FALSE, int base=0, CWnd* pParent=NULL);

	virtual BOOL  OnInitDialog();

// ダイアログ データ
	//{{AFX_DATA(CSetModeDLG)
	enum { IDD = IDD_SETDLG };
		// メモ: ClassWizard はこの位置にデータ メンバを追加します。
	//}}AFX_DATA

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CSetModeDLG)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CSetModeDLG)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnBnClickedSetCline();
	afx_msg void OnBnClickedSetVcolor();
	afx_msg void OnBnClickedSetRvrsz();
	afx_msg void OnBnClickedSetDivdraw();
	afx_msg void OnBnClickedSetAtcntrst();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SETMODEDLG_H__4594B973_77C8_4085_873E_9EC49B2F0A9D__INCLUDED_)
