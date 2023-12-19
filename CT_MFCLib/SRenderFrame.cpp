// SRenderFrame.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"

#include "SRenderFrame.h"
#include "CTCallBack.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxl;
using namespace jbxwl;



/////////////////////////////////////////////////////////////////////////////
// CSRenderFrame

IMPLEMENT_DYNCREATE(CSRenderFrame, CExFrame)


CSRenderFrame::CSRenderFrame()
{
}



CSRenderFrame::~CSRenderFrame()
{
}


BEGIN_MESSAGE_MAP(CSRenderFrame, CExFrame)
	//{{AFX_MSG_MAP(CSRenderFrame)
	ON_COMMAND(ID_SRNDR_OK, OnSRndrOk)
	ON_COMMAND(ID_SRNDR_CANCEL, OnSRndrCancel)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// CSRenderFrame メッセージ ハンドラ

int CSRenderFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CExFrame::OnCreate(lpCreateStruct) == -1) return -1;

	// ツールバーの作成
	toolBar = new CExToolBar();
	if (!toolBar->CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | WS_CLIPCHILDREN) ||
		!toolBar->LoadToolBar(IDR_SRNDR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // 作成に失敗
	}

	// TODO: ツール バーをドッキング可能にしない場合は以下の３行を削除
	//       してください。
	toolBar->EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(toolBar);
	
	return 0;
}






void CSRenderFrame::OnSRndrOk() 
{
	anyData = pDoc->anyData;	// 領域データ
	pApp->callMSGraph = pDoc->msGraph;
	CExFrame* frm = pApp->TemplateCallBack(this, APP_CALLBACK_VOLM);

	if (frm!=NULL) {
		pApp->TemplateCallBack(this, APP_CALLBACK_SVOL);
		DestroyWindow();
	}
}






void CSRenderFrame::OnSRndrCancel() 
{
//	if (pDoc->anyData!=NULL) ((RgnData*)(pDoc->anyData))->mfree();
	
	DestroyWindow();
}




void CSRenderFrame::OnClose() 
{
//	if (pDoc->anyData!=NULL) ((RgnData*)(pDoc->anyData))->mfree();
	
	CExFrame::OnClose();
}
