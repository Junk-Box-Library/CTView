#if !defined(AFX_MGRFRAME_H__72D30D64_A3A8_4A6F_B13A_8C15E3E3F689__INCLUDED_)
#define AFX_MGRFRAME_H__72D30D64_A3A8_4A6F_B13A_8C15E3E3F689__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




/**
CMGRFrame クラス

	CDxMGRView クラス用フレーム

*/


#include  "ContrastDLG.h"
#include  "ExFrame.h"


using namespace jbxl;
using namespace jbxwl;


using namespace jbxl;
using namespace jbxwl;




/////////////////////////////////////////////////////////////////////////////
// CMGRFrame フレーム

class CMGRFrame : public CExFrame
{
	DECLARE_DYNCREATE(CMGRFrame)
protected:
	CMGRFrame();           // 動的生成に使用されるプロテクト コンストラクタ。


// アトリビュート
public:


// オペレーション
public:
	POINT  CMGRFrame::GetMousePoint(int click);

	bool   calledMousePoint;		// GetMousePoint() の２重呼び出し禁止フラグ


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します

	//{{AFX_VIRTUAL(CMGRFrame)
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	virtual ~CMGRFrame();

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CMGRFrame)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMgrContrast();
	afx_msg void OnMgrToVOL();
	//afx_msg void OnMgrRgnGrwng();
	//afx_msg void OnMgrRcrsvRgnSrch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MGRFRAME_H__72D30D64_A3A8_4A6F_B13A_8C15E3E3F689__INCLUDED_)
