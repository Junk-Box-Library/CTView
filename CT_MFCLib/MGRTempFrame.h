#if !defined(AFX_MGRTEMPFRAME_H__72D30D64_A3A8_4A6F_B13A_8C15E3E3F689__INCLUDED_)
#define AFX_MGRTEMPFRAME_H__72D30D64_A3A8_4A6F_B13A_8C15E3E3F689__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
CMGRTempFrame クラス

	CDxMGRView クラス用フレーム

*/

#include  "ContrastDLG.h"
#include  "ExFrame.h"


using namespace jbxl;
using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CMGRTempFrame フレーム

class CMGRTempFrame : public CExFrame
{
	DECLARE_DYNCREATE(CMGRTempFrame)
protected:
	CMGRTempFrame();           // 動的生成に使用されるプロテクト コンストラクタ。

// アトリビュート
public:

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します

	//{{AFX_VIRTUAL(CMGRTempFrame)
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	virtual ~CMGRTempFrame();

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CMGRTempFrame)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMgrContrast();
	afx_msg void OnMgrToVOL();
	afx_msg void OnMgrBack();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MGRTEMPFRAME_H__72D30D64_A3A8_4A6F_B13A_8C15E3E3F689__INCLUDED_)
