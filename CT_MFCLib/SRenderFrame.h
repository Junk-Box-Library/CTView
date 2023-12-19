#if !defined(AFX_VRENDERFRAME_H__45C8B3D1_A596_4D16_B989_9806180CD39A__INCLUDED_)
#define AFX_VRENDERFRAME_H__45C8B3D1_A596_4D16_B989_9806180CD39A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "ExClass.h"
#include "resource.h"


using namespace jbxl;
using namespace jbxwl;



/////////////////////////////////////////////////////////////////////////////
// CSRenderFrame フレーム

class CSRenderFrame : public CExFrame
{
	DECLARE_DYNCREATE(CSRenderFrame)
protected:
	CSRenderFrame();           // 動的生成に使用されるプロテクト コンストラクタ。

// アトリビュート
public:

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します

	//{{AFX_VIRTUAL(CSRenderFrame)
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	virtual ~CSRenderFrame();

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CSRenderFrame)
	afx_msg void OnSRndrOk();
	afx_msg void OnSRndrCancel();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_VRENDERFRAME_H__45C8B3D1_A596_4D16_B989_9806180CD39A__INCLUDED_)
