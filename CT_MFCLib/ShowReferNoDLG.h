#if !defined(AFX_SHOWREFERNODLG_H__3B802AA1_96FC_47C2_8DD6_64B52874B11D__INCLUDED_)
#define AFX_SHOWREFERNODLG_H__3B802AA1_96FC_47C2_8DD6_64B52874B11D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowReferNoDLG.h : ヘッダー ファイル
//


#include "resource.h"





/////////////////////////////////////////////////////////////////////////////
// CShowReferNoDLG ダイアログ

class CShowReferNoDLG : public CDialog
{
public:
	int			nData;
	int			nVol;
	int			nRgn;
	int			nMCT;
	int			nTBN;

	CStatic*	datSBox;
	CStatic*	volSBox;
	CStatic*	rgnSBox;
	CStatic*	mctSBox;
	CStatic*	tbnSBox;


// コンストラクション
public:
	CShowReferNoDLG(int data, int vol, int rgn, int mct, int tbn, CWnd* pParent = NULL);

	virtual BOOL  OnInitDialog();



// ダイアログ データ
	//{{AFX_DATA(CShowReferNoDLG)
	enum { IDD = IDD_REFNODLG };
		// メモ: ClassWizard はこの位置にデータ メンバを追加します。
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CShowReferNoDLG)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CShowReferNoDLG)
		// メモ: ClassWizard はこの位置にメンバ関数を追加します。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SHOWREFERNODLG_H__3B802AA1_96FC_47C2_8DD6_64B52874B11D__INCLUDED_)
