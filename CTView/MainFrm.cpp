// MainFrm.cpp : CMainFrame クラスの動作の定義を行います。
//

#include "stdafx.h"

#include "CTView.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxl;
using namespace jbxwl;



/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	//ON_WM_INITMENU()
	ON_WM_INITMENU()
END_MESSAGE_MAP()



static UINT indicators[] =
{
	ID_SEPARATOR,           // ステータス ライン インジケータ
	ID_INDICATOR_KANA,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};






/////////////////////////////////////////////////////////////////////////////
// CMainFrame クラスの構築/消滅

CMainFrame::CMainFrame()
{
	pApp = NULL;
	// TODO: この位置にメンバの初期化処理コードを追加してください。
	
	// メニューの有効，無効の切り替えを可能にする
	this->m_bAutoMenuEnable = FALSE;
}




CMainFrame::~CMainFrame()
{
}






int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

/*
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // 作成に失敗
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // 作成に失敗
	}

	// TODO: ツール バーをドッキング可能にしない場合は以下の３行を削除
	//       してください。
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
*/
	return 0;
}





BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) ) return FALSE;

	// TODO: この位置で CREATESTRUCT cs を修正して、Window クラスやスタイルを
	//       修正してください。
	cs.style |= WS_CLIPCHILDREN;

	return TRUE;
}





/////////////////////////////////////////////////////////////////////////////
// CMainFrame クラスの診断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}



void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG









/////////////////////////////////////////////////////////////////////////////
// CMainFrame メッセージ ハンドラ

void  CMainFrame::OnInitMenu(CMenu* pMenu)
{
	UpdateDLLMenu(pMenu);

	// File Menu, [Save Graphic Data]
	if (pApp->msGraph_ref>=0) pMenu->EnableMenuItem(ID_FILE_SAVEFL, MF_BYCOMMAND | MF_ENABLED);
	else  pMenu->EnableMenuItem(ID_FILE_SAVEFL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	// File Menu, [Save Volume Data]
	if (pApp->msGraphVOL_ref>=0) pMenu->EnableMenuItem(ID_FILE_SAVEVOL, MF_BYCOMMAND | MF_ENABLED);
	else  pMenu->EnableMenuItem(ID_FILE_SAVEVOL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	// View Menu, [View Graphic Data]
	if (pApp->msGraph_ref>=0) pMenu->EnableMenuItem(ID_VIEW_MCT, MF_BYCOMMAND | MF_ENABLED);
	else  pMenu->EnableMenuItem(ID_VIEW_MCT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	// View Menu, [View X]
	if (pApp->msGraph_ref>=0 && pApp->msGraph.zs>1) pMenu->EnableMenuItem(ID_VIEW_MCTX, MF_BYCOMMAND | MF_ENABLED);
	else  pMenu->EnableMenuItem(ID_VIEW_MCTX, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	// View Menu, [View -Y]
	if (pApp->msGraph_ref>=0 && pApp->msGraph.zs>1) pMenu->EnableMenuItem(ID_VIEW_MCTY, MF_BYCOMMAND | MF_ENABLED);
	else  pMenu->EnableMenuItem(ID_VIEW_MCTY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	// View Menu, [View Thumbnail]
	if (pApp->msGraph_ref>=0) pMenu->EnableMenuItem(ID_VIEW_TN, MF_BYCOMMAND | MF_ENABLED);
	else  pMenu->EnableMenuItem(ID_VIEW_TN, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	// View Menu, [View Volume]
	if (pApp->msGraphVOL_ref>=0) pMenu->EnableMenuItem(ID_VIEW_VOL, MF_BYCOMMAND | MF_ENABLED);
	else  pMenu->EnableMenuItem(ID_VIEW_VOL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	// Memory Menu, [Delete Graphic Data]
	if (pApp->msGraph_ref>=0) pMenu->EnableMenuItem(ID_DEL_MDATA, MF_BYCOMMAND | MF_ENABLED);
	else  pMenu->EnableMenuItem(ID_DEL_MDATA, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	// Memory Menu, [Delete Volume Data]
	if (pApp->msGraphVOL_ref>=0) pMenu->EnableMenuItem(ID_DEL_VDATA, MF_BYCOMMAND | MF_ENABLED);
	else  pMenu->EnableMenuItem(ID_DEL_VDATA, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	// Help Menu, [Information of Graphic Data]
	//if (pApp->hasMSGraph) pMenu->EnableMenuItem(ID_INFO_GRAPH, MF_BYCOMMAND | MF_ENABLED);
	if (pApp->msGraph_ref>=0) pMenu->EnableMenuItem(ID_INFO_GRAPH, MF_BYCOMMAND | MF_ENABLED);
	else  pMenu->EnableMenuItem(ID_INFO_GRAPH, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	// Plugins Menu, No Plugins
	pMenu->EnableMenuItem(ID_DLL_DUMMY, MF_DISABLED | MF_GRAYED);

	CMDIFrameWnd::OnInitMenu(pMenu);
}






void  CMainFrame::UpdateDLLMenu(CMenu* menu)
{
	if (menu==NULL) menu = GetMenu();
	if (menu==NULL) return;

	DLLFunctionTBL* dftbl = &(pApp->dllFunctionTBL);

	if (dftbl->loaded) AppendDLLMenu(menu);
	else               RemoveDLLMenu(menu);
}






void  CMainFrame::RemoveDLLMenu(CMenu* menu)
{
	if (menu==NULL) menu = GetMenu();
	if (menu==NULL) return;

	DLLFunctionTBL* dftbl = &(pApp->dllFunctionTBL);

	CMenu* pSub = GetMenu_byID(menu, ID_DLL_PROC);
	if (pSub==NULL) return;

	int num = pSub->GetMenuItemCount();
	for (int i=0; i<num; i++){
		pSub->RemoveMenu(0, MF_BYPOSITION);
	}
	
	CString mesg;
	mesg.LoadString(IDS_STR_NO_PLUGINS);
	pSub->AppendMenu(MF_STRING, ID_DLL_DUMMY, mesg);
	//pSub->AppendMenu(MF_STRING, ID_DLL_DUMMY, "処理用DLL無し");
	DrawMenuBar();
}





void  CMainFrame::AppendDLLMenu(CMenu* menu)
{
	if (menu==NULL) menu = GetMenu();
	if (menu==NULL) return;

	DLLFunctionTBL* dftbl = &(pApp->dllFunctionTBL);

	CMenu* pSub = GetMenu_byID(menu, ID_DLL_DUMMY);
	if (pSub==NULL) pSub = GetMenu_byID(menu, ID_DLL_PROC);

	if (dftbl->vnum>0 && pSub!=NULL) {
		// ID_DLL_DUMMY を削除
		int num = pSub->GetMenuItemCount();
		for (int i=0; i<num; i++){
			pSub->RemoveMenu(0, MF_BYPOSITION);
		}

		// DLLメニューを追加
		num = Min(DLL_PROC_RANGE, dftbl->vnum);
		for (int i=0; i<num; i++){
			CString str = mbs2ts(dftbl->dllfunc[i]->title);
			BOOL active = FALSE;
			if (dftbl->dllfunc[i]->pactv_func!=NULL) active = dftbl->dllfunc[i]->pactv_func();

			pSub->AppendMenu(MF_STRING, ID_DLL_PROC+i, str);
			if (pApp->hasMSGraph || active) {
				menu->EnableMenuItem(ID_DLL_PROC+i, MF_BYCOMMAND | MF_ENABLED);
			}
			else {
				menu->EnableMenuItem(ID_DLL_PROC+i, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			}
			/*
			if (pApp->hasMSGraph) {
				pSub->AppendMenu(MF_STRING, ID_DLL_PROC+i, str);
				//pSub->EnableMenuItem(ID_DLL_PROC+i, MF_BYCOMMAND |MF_ENABLED);
			}
			else {
				pSub->AppendMenu(MF_STRING, ID_DLL_DUMMY, str);
				//pSub->EnableMenuItem(ID_DLL_PROC+i, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			}*/
		}

		DrawMenuBar();
	}
}








