// MainFrm.h : CMainFrame クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__FD9235C9_F31C_4FA6_86EC_7AAE90877DA0__INCLUDED_)
#define AFX_MAINFRM_H__FD9235C9_F31C_4FA6_86EC_7AAE90877DA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ExToolBar.h"
#include "MFCTool.h"


using namespace jbxl;
using namespace jbxwl;



class CCTViewApp;



class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)

public:
	CMainFrame();

// アトリビュート
public:
	CCTViewApp*  pApp;


// オペレーション
public:




// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void  RemoveDLLMenu(CMenu* menu=NULL);
	void  AppendDLLMenu(CMenu* menu=NULL);

	void  UpdateDLLMenu(CMenu* menu=NULL);



protected:  // コントロール バー用メンバ
	CStatusBar  m_wndStatusBar;
	CExToolBar  m_wndToolBar;



// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnViewToolbar();
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};





/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MAINFRM_H__FD9235C9_F31C_4FA6_86EC_7AAE90877DA0__INCLUDED_)
