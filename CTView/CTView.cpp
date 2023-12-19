// CTView.cpp : アプリケーション用クラスの機能定義を行います．
//

#include  "stdafx.h"


#include  "tools++.h"

#include  "BasicLib.h"
#include  "MathLib.h"
#include  "GraphLib.h"
#include  "WinBaseLib.h"
//#include  "DLGBoxLib.h"

#include  "MFCBase.h"
#include  "MFCio.h"

#include  "ReadFilesDLG.h"
#include  "WriteFilesDLG.h"
#include  "WinTools.h"

#include  "Dx2DView.h"
#include  "DxMGRView.h"
#include  "DxVScrollView.h"
#include  "DxGraphDoc.h"
#include  "RwGRDoc.h"

#include  "vThumbNailDoc.h"
#include  "vThumbNailFrame.h"

#include  "CT_MFCLib.h"
#include  "CTCallBack.h"
#include  "MainFrm.h"
#include  "CTView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxl;
using namespace jbxwl;



/////////////////////////////////////////////////////////////////////////////
// CCTViewApp

BEGIN_MESSAGE_MAP(CCTViewApp, CWinApp)
	//{{AFX_MSG_MAP(CCTViewApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_INFO_GRAPH, OnInfoGraph)
	ON_COMMAND(ID_FILE_OPENVOL, OnFileOpenVOL)
	ON_COMMAND(ID_FILE_MCT, OnMCT)
	ON_COMMAND(ID_FILE_TN, OnFileTn)
	ON_COMMAND(ID_DEL_RDATA, OnDelRData)
	ON_COMMAND(ID_DEL_VDATA, OnDelVData)
	ON_COMMAND(ID_DEL_MDATA, OnDelMData)
	ON_COMMAND(ID_VIEW_VOL, OnViewOpenVOL)
	ON_COMMAND(ID_VIEW_TN, OnViewTn)
	ON_COMMAND(ID_TOOL_SET, OnToolSet)
	ON_COMMAND(ID_TOOL_SHWREFN, OnToolShwRefn)
	ON_COMMAND(ID_FILE_SAVEVOL, OnFileSaveVOL)
	ON_COMMAND(ID_FILE_SAVEFL, OnFileSaveFL)
	ON_COMMAND(ID_VIEW_MCT,  OnViewMCT)
	ON_COMMAND(ID_VIEW_MCTX, OnViewMCTX)
	ON_COMMAND(ID_VIEW_MCTY, OnViewMCTY)
	ON_COMMAND(ID_TOOL_READDLL, OnReadDLL)
	ON_COMMAND(ID_TOOL_FREEDLL, OnFreeDLL)
	ON_COMMAND(ID_TOOL_HEADER, OnToolHeader)
	ON_COMMAND(ID_FILE_OPENRGSL, OnFileOpenRGSL)
	ON_COMMAND(ID_FILE_SAVERGSL, OnFileSaveRGSL)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND_RANGE(ID_DLL_PROC, ID_DLL_PROC+DLL_PROC_RANGE-1, OnDLLFunction)
	//}}AFX_MSG_MAP
	// 標準のファイル基本ドキュメント コマンド
	//ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// 標準の印刷セットアップ コマンド
	//ON_COMMAND(ID_FILE_println_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// DLL の操作
//

//#include "WinLib.rc";

void  CCTViewApp::OnDLLFunction(UINT id) 
{
	UINT num = id - ID_DLL_PROC;

	MSGraph<sWord>* xp = dllFunctionTBL.dllfunc[num]->proc_graph((MSGraph<sWord>*)&msGraph, pMainFrame);

	if (xp!=NULL) {
		if (xp->state==JBXL_GRAPH_CANCEL) {
			CString mesg, noti;
			//"処理はキャンセルされました"
			mesg.LoadString(IDS_STR_CANCELD_PROC);
			noti.LoadString(IDS_STR_ATTNSN);
			MessageBox(m_pMainWnd->m_hWnd, mesg, noti, MB_OK);
			dllFunctionTBL.dllfunc[num]->free_graph(xp);
			xp = NULL;
		}
		else {
			if (xp->gp!=NULL) {
				CExFrame* pfrm = CreateDocFrmView(pDocTemplTMGR, this);

				if (pfrm!=NULL) {
					copy_MSGraph(*xp, pfrm->pDoc->msGraph);
					dllFunctionTBL.dllfunc[num]->free_graph(xp);
					xp = NULL;

					if (!pfrm->pDoc->msGraph.isNull()) {
						pfrm->pDoc->hasReadData = TRUE;
						pfrm->pDoc->colorMode	= pfrm->pDoc->msGraph.color;
						pfrm->pDoc->ctrlCntrst  = forceContrastMode;

						if (dllFunctionTBL.dllfunc[num]->pactv_func()) {
							pfrm->pDoc->infHead.kind = CREATE_DATA;
							pfrm->pDoc->preTitle	 = _T("");
							pfrm->pDoc->Title		 = dllFunctionTBL.dllfunc[num]->title;
							pfrm->pDoc->pstTitle	 = _T("");
						}
						else {
							CString mesg;
							//"処理結果";
							mesg.LoadString(IDS_STR_RSLT_PROC);
							pfrm->pDoc->infHead.kind = PROC_DATA;
							pfrm->pDoc->preTitle	 = mbs2ts(dllFunctionTBL.dllfunc[num]->title) + _T("： ");
							pfrm->pDoc->Title		 = mesg;
							pfrm->pDoc->pstTitle	 = _T("");
						}
						
						int ret = ExecDocFrmView(pfrm);
						ExecDocFrmViewError(m_pMainWnd->m_hWnd, ret);
					}
					// pfrm->pDoc->msGraph.isNull()==true の場合
					else {
						CString mesg, noti;
						//"メモリ確保エラー：メモリが足りません"
						mesg.LoadString(IDS_STR_ERR_MEMORY);
						noti.LoadString(IDS_STR_ERROR);
						MessageBox(m_pMainWnd->m_hWnd, mesg, noti, MB_OK);
						delete(pfrm->pDoc);
						delete(pfrm);
						pfrm = NULL;
					}
				}
				// pfrm==NULL の場合
				else {
					CString mesg, noti;
					//"表示用フレームの生成に失敗しました"
					mesg.LoadString(IDS_STR_FAIL_PLG_CRT_FRM);
					noti.LoadString(IDS_STR_ERROR);
					MessageBox(m_pMainWnd->m_hWnd, mesg, noti, MB_OK);
				}
			}
			// xp->gp==NULL の場合
			else {
				if (xp->state!=STATE_GRAPH_NOERR) {
					CString mesg, noti;
					//"処理用プラグインはデータを生成していません"
					mesg.LoadString(IDS_STR_FAIL_PLG_NODATA);
					noti.LoadString(IDS_STR_ATTNSN);
					MessageBox(m_pMainWnd->m_hWnd, mesg, noti, MB_OK);
				}
			}
		}
	}

	// xp==NULL の場合
	else {
		CString mesg, noti;
		//"処理失敗（または手動による中止）"
		mesg.LoadString(IDS_STR_FAIL_PLG_PROC);
		noti.LoadString(IDS_STR_ATTNSN);
		MessageBox(m_pMainWnd->m_hWnd, mesg, noti, MB_OK);
	}

	if (xp!=NULL) dllFunctionTBL.dllfunc[num]->free_graph(xp);

	return;
}



void  CCTViewApp::OnFreeDLL(void)
{
	pMainFrame->RemoveDLLMenu();
	dllFunctionTBL.free();
	dllFunctionTBL.dllfunc = NULL;
}



void  CCTViewApp::OnReadDLL(void)
{
	if (dllFunctionTBL.dllfunc!=NULL) OnFreeDLL();
	dllFunctionTBL.load_module(modulesDir);
	dllFunctionTBL.make_module_tbl(_T("get_info"), 4);
	pMainFrame->AppendDLLMenu();
}




/////////////////////////////////////////////////////////////////////////////
// CCTViewApp クラスの構築

CCTViewApp::CCTViewApp()
{
#ifdef _DEBUG
	DebugMode = ON;
#endif

	pMainFrame    = NULL;

	hasMSGraph    = FALSE;
	hasMSGraphVOL = FALSE;
	hasMSGraphRGN = FALSE;
	hasCmnHeadTBN = FALSE;
	hasCmnHeadMCT = FALSE;

	msGraph.setRefer(&msGraph_ref);
	msGraphVOL.setRefer(&msGraphVOL_ref);
	msGraphRGN.setRefer(&msGraphRGN_ref);
	cmnHeadTBN.setRefer(&cmnHeadTBN_ref);
	cmnHeadMCT.setRefer(&cmnHeadMCT_ref);

	infoGraphDlg = new CInfoGraphDLG;

	for (int i=0; i<RGNDATA_MAXNUM; i++) rgnData[i] = NULL;
	rgnDataNum = 0;

	tempBase = TempBase = 0;		// tempBase はメンバ変数．TempBase はグローバル変数
	init_CmnHead(&infHead);

	preTitle = _T("");
	Title	 = _T("");
	pstTitle = _T("");

	// 動作設定
	volumeColorMode		= TRUE;
	centerLineMode		= TRUE;
	forceContrastMode	= FALSE;
}



CCTViewApp::~CCTViewApp()
{
	msGraph.ffree();
	msGraphVOL.ffree();
	msGraphRGN.ffree();
	cmnHeadTBN.ffree();
	cmnHeadMCT.ffree();

	Dx9ReleaseInterface();

	for (int i=0; i<rgnDataNum; i++) {
		delete(rgnData[i]);
	}

	delete infoGraphDlg;
}




/////////////////////////////////////////////////////////////////////////////
// 唯一の CCTViewApp オブジェクト

CCTViewApp theApp;



/////////////////////////////////////////////////////////////////////////////
// CCTViewApp クラスの初期化

BOOL CCTViewApp::InitInstance()
{
	AfxEnableControlContainer();

	// 標準的な初期化処理
	// もしこれらの機能を使用せず，実行ファイルのサイズを小さく
	// したければ以下の特定の初期化ルーチンの中から不必要なもの
	// を削除してください．

	//SetThreadLocale( 0x0409);

//#define _AFXDLL

#ifdef _AFXDLL
	Enable3dControls();			// 共有 DLL の中で MFC を使用する場合にはここを呼び出してください．
#else
	Enable3dControlsStatic();	// MFC と静的にリンクしている場合にはここを呼び出してください．
#endif

	// 設定が保存される下のレジストリ キーを変更します．
	// TODO: この文字列を，会社名または所属など適切なものに
	// 変更してください．
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // 標準の INI ファイルのオプションをロードします (MRU を含む)

	// アプリケーション用のドキュメント テンプレートを登録します．ドキュメント テンプレート
	//  はドキュメント，フレーム ウィンドウとビューを結合するために機能します．
	CMultiDocTemplate* pDocTempllate;
 
	// pDocTempl2D: 未知データの 2D表示
	pDocTempllate = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDxGraphDoc),
		RUNTIME_CLASS(CExFrame), 
		RUNTIME_CLASS(CDx2DView));
	AddDocTemplate(pDocTempllate);
	pDocTempl2D = pDocTempllate;

	// pDocTemplMGR: MSGraph表示
	pDocTempllate = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CRwGRDoc),
		RUNTIME_CLASS(CMGRFrame), 
		RUNTIME_CLASS(CDxMGRView));
	AddDocTemplate(pDocTempllate);
	pDocTemplMGR = pDocTempllate;

	// pDocTemplMCT: マルチCT表示
	pDocTempllate = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CRwGRDoc),
		RUNTIME_CLASS(CMGRFrame), 
		RUNTIME_CLASS(CDxMGRView));
	AddDocTemplate(pDocTempllate);
	pDocTemplMCT = pDocTempllate;

	// pDocTemplMGR: MSGraph表示（メニューバー無し）
	pDocTempllate = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CRwGRDoc),
		RUNTIME_CLASS(CMGRSimpleFrame), 
		RUNTIME_CLASS(CDxMGRView));
	AddDocTemplate(pDocTempllate);
	pDocTemplSMGR = pDocTempllate;

	// pDocTemplTMGR: MSGraph表示（作業用）
	pDocTempllate = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CRwGRDoc),
		RUNTIME_CLASS(CMGRTempFrame), 
		RUNTIME_CLASS(CDxMGRView));
	AddDocTemplate(pDocTempllate);
	pDocTemplTMGR = pDocTempllate;

	// pDocTemplTBN: サムネイル表示
	pDocTempllate = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CvThumbNailDoc),
		RUNTIME_CLASS(CvThumbNailFrame), 
		RUNTIME_CLASS(CDxVScrollView));
	AddDocTemplate(pDocTempllate);
	pDocTemplTBN = pDocTempllate;

	// pDocTemplTVOL: ボリュームデータ表示（結合用メニューバーあり）
	pDocTempllate = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CRwGRDoc),
		RUNTIME_CLASS(CSRenderFrame), 
		RUNTIME_CLASS(CDxSRenderView));
	AddDocTemplate(pDocTempllate);
	pDocTemplTVOL = pDocTempllate;

	// pDocTemplSVOL: ボリュームデータ表示（メニューバーなし）
	pDocTempllate = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CRwGRDoc),
		RUNTIME_CLASS(CExFrame), 
		RUNTIME_CLASS(CDxSRenderView));
	AddDocTemplate(pDocTempllate);
	pDocTemplSVOL = pDocTempllate;

	// pDocTemplRGN: 領域データ表示（メニューバーなし）
	pDocTempllate = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CRwRGNDoc),
		RUNTIME_CLASS(CExFrame), 
		RUNTIME_CLASS(CDxLRenderView));
	AddDocTemplate(pDocTempllate);
	pDocTemplRGN = pDocTempllate;

	// ロケール
	setSystemLocale();

	// DLLモジュールの読み込み
	int ldr = (int)tcslen(_T(MODULES_DIR));
	int len = GetCurrentDirectory(LDIR_PATH, modulesDir);
	
	if (LDIR_PATH>=len+ldr+2) {
		modulesDir[len] = '\\';
		sntprintf((TCHAR*)&(modulesDir[len + 1]), LDIR_PATH - len - 1, ldr + 1, _T(MODULES_DIR));
	}
	else {
		sntprintf(modulesDir, ldr+1, _T(MODULES_DIR));
	}

	dllFunctionTBL.load_module(modulesDir);
	dllFunctionTBL.make_module_tbl(_T("get_info"), 4);	// 3->4へ変更

	// メイン MDI フレーム ウィンドウを作成
	pMainFrame = new CMainFrame;
	pMainFrame->pApp = this;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME)) return FALSE;
	m_pMainWnd = pMainFrame;

	// DDE，file open など標準のシェル コマンドのコマンドラインを解析します．
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// デフォルトで書類オープン無しにする．
	if (cmdInfo.m_nShellCommand==CCommandLineInfo::FileNew) {
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	}

	// コマンドラインでディスパッチ コマンドを指定します．
	if (!ProcessShellCommand(cmdInfo)) return FALSE;

	// メイン ウィンドウが初期化されたので，表示と更新を行います．
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	
	// DirectX9 の初期化
	BOOL rslt = Dx9CreateInterface(this);
	if (!rslt) {
		CString mesg, noti;
		mesg.LoadString(IDS_STR_FAIL_GET_DX9DEV);
		noti.LoadString(IDS_STR_ERROR);
		MessageBox(m_pMainWnd->m_hWnd, mesg, noti, MB_OK);
		//MessageBox(m_pMainWnd->m_hWnd, "DirectX9の初期化に失敗しました", "エラー", MB_OK);
		return FALSE;
	}
	//Dx9DividePrimitiveMode = TRUE;

	return TRUE;
}




/////////////////////////////////////////////////////////////////////////////
// アプリケーションのバージョン情報で使われる CAboutDlg ダイアログ
//
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard 仮想関数のオーバーライドを生成します．
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	//{{AFX_MSG(CAboutDlg)
		// メッセージ ハンドラはありません．
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnDropFiles(HDROP hDropInfo);
};



CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}



void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// メッセージ ハンドラはありません．
	//}}AFX_MSG_MAP
//	ON_WM_DROPFILES()
END_MESSAGE_MAP()



// アバウトダイアログを実行するためのアプリケーション コマンド
void CCTViewApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}



// 情報表示
void CCTViewApp::OnInfoGraph()
{
	infoGraphDlg->DoModal();
}




/////////////////////////////////////////////////////////////////////////////
// CCTViewApp メッセージ ハンドラ



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ファイル ＆ 表示

void  CCTViewApp::OnFileOpen() 
{
	CmnHead	  hd, dd;
	CExFrame* pfrm = NULL;

	CString mesg;
	//"データファイルを開く"
	mesg.LoadString(IDS_STR_OPEN_FILE);
	CString fname = EasyGetOpenFileName((LPCTSTR)mesg, m_pMainWnd->m_hWnd);
	if (fname.IsEmpty()) return;

	dd = SetHeaderDLG.getCmnHead();
	char* mbstr = ts2mbs(fname);
	hd = readXHead(mbstr, &dd);
	::free(mbstr);

	int kind = hd.kind & 0x00ff;
	DEBUG_ERROR("CCTViewApp: kind = %04x", kind);
	free_CmnHead(&hd);

	switch (kind) {
		case  CT_DATA:	
			DEBUG_ERROR("CCTViewApp::OnFileOpen: CT_DATA File");
			pfrm = openMCTFile(fname, TRUE);
			break;

		case  MOON_DATA:	
			DEBUG_ERROR("CCTViewApp::OnFileOpen: MOON_DATA File");
			openMCTFile(fname, TRUE);
			break;

		case  RAS_DATA:
			DEBUG_ERROR("CCTViewApp::OnFileOpen: RAS_DATA File");
			pfrm = openMGRFile(fname);
			break;

		case  JPEG_RGB_DATA:
			DEBUG_ERROR("CCTViewApp::OnFileOpen: JPEG_RGB_DATA File");
			pfrm = openMGRFile(fname);
			break;

		case  TIFF_DATA:	// 未実装
			DEBUG_ERROR("CCTViewApp::OnFileOpen: TIFF_DATA File");
			break;

		case  DICOM_DATA:
			DEBUG_ERROR("CCTViewApp::OnFileOpen: DICOM_DATA File");
			pfrm = openMCTFile(fname, TRUE);
			break;

		case  USERSET_DATA:
			DEBUG_ERROR("CCTViewApp::OnFileOpen: USERSET_DATA File");
			pfrm = openMCTFile(fname, TRUE);
			break;

		case  CT_3DM:	
			DEBUG_ERROR("CCTViewApp::OnFileOpen: CT_3DM File");
			pfrm = openMCTFile(fname);
			break;

		case  CT_3D_VOL:
			DEBUG_ERROR("CCTViewApp::OnFileOpen: CT_3D_VOL File");
			pfrm = openVOLFile(fname);
			break;

		case  CT_RGN_SL:
			DEBUG_ERROR("CCTViewApp::OnFileOpen: CT_RGN_SL File");
			pfrm = openRGNFile(fname);
			break;
		
		default:
			DEBUG_ERROR("CCTViewApp::OnFileOpen: other File kind = %04x", kind);
			pfrm = open2DFile(fname);
			break;
	}

	return;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2D画像データの読み込みと表示

//
// マルチＣＴ（ID_FILE_MCT イベント）
//
void  CCTViewApp::OnMCT() 
{
	CString mesg;
	mesg.LoadString(IDS_STR_OPN_CTFL);
	CString fname = EasyGetOpenFileName((LPCTSTR)mesg, m_pMainWnd->m_hWnd);
	//CString fname = EasyGetOpenFileName("CT画像ファイルを開く", m_pMainWnd->m_hWnd);
	if (fname.IsEmpty()) return;
	
	CExFrame* pfrm = openMCTFile(fname);
	return;
}



//
// メマルチＣＴ（ファイルオープン）
//
CExFrame*   CCTViewApp::openMCTFile(LPCTSTR fname, BOOL multi, int vmode) 
{
	if (!deleteMData(true)) return NULL;

	CExFrame* pfrm = CreateDocFrmView(pDocTemplMGR, this);
	pfrm->pDoc->msGraph        = msGraph;		// メモリ管理付
	pfrm->pDoc->cmnHead        = cmnHeadMCT;	// メモリ管理付
	pfrm->pDoc->multiSliceData = multi;
	pfrm->pDoc->ctrlCntrst	   = forceContrastMode;
	pfrm->pView->viewMode      = vmode;

	int ret = ExecDocFrmView(pfrm, fname);
	if (ret==0) {
		msGraph       = pfrm->pDoc->msGraph;
		cmnHeadMCT    = pfrm->pDoc->cmnHead;
		preTitle	  = pfrm->pDoc->preTitle;
		Title		  = pfrm->pDoc->Title;
		pstTitle	  = pfrm->pDoc->pstTitle;
		infHead		  = pfrm->pDoc->infHead;
		hasMSGraph    = TRUE;
		hasCmnHeadMCT = TRUE;
		infoGraphDlg->setVal(infHead);
		infoGraphDlg->setVal(msGraph);
	}
	else pfrm = NULL;

	ExecDocFrmViewError(m_pMainWnd->m_hWnd, ret);
	return pfrm;
}



//
// マルチＣＴ：「表示」メニューより選択．
//
void  CCTViewApp::OnViewMCT() 
{
	CExFrame* pfrm = viewMCTXYZ(VIEW_ZDIR);
	return;
}



void  CCTViewApp::OnViewMCTX() 
{
	CString mesg;
	mesg.LoadString(IDS_STR_VW_X_DIR);
	CExFrame* pfrm = viewMCTXYZ(VIEW_XDIR, (LPCTSTR)(_T(" ") + mesg));
	//CExFrame* pfrm = viewMCTXYZ(VIEW_XDIR, _T(" [X方向]"));
	return;
}



void  CCTViewApp::OnViewMCTY() 
{
	CString mesg;
	mesg.LoadString(IDS_STR_VW_RVY_DIR);
	CExFrame* pfrm = viewMCTXYZ(VIEW_YDIR, (LPCTSTR)(_T(" ") + mesg));
	//CExFrame* pfrm = viewMCTXYZ(VIEW_YDIR, _T(" [-Y方向]"));
	return;	
}



//
// マルチＣＴ表示（メニュー）
//
CExFrame*  CCTViewApp::viewMCTXYZ(int vmode, LPCTSTR title) 
{
	CExFrame* pfrm = NULL;

	if (hasMSGraph) {
		pfrm = CreateDocFrmView(pDocTemplMCT, this);
		pfrm->pDoc->msGraph     = msGraph;		// メモリ管理付
		pfrm->pDoc->hasReadData = TRUE;
		pfrm->pDoc->ctrlCntrst  = forceContrastMode; 
		pfrm->pDoc->colorMode	= msGraph.color;
		pfrm->pDoc->preTitle	= preTitle;
		pfrm->pDoc->Title		= Title;
		pfrm->pDoc->pstTitle	= title;
		pfrm->pView->viewMode   = vmode;

		int ret;
		if (hasCmnHeadMCT) {
			pfrm->pDoc->hasViewData = TRUE;
			pfrm->pDoc->cmnHead = cmnHeadMCT;	// メモリ管理付
			pfrm->pDoc->vMax = msGraph.max;
			pfrm->pDoc->vMin = msGraph.min;
			pfrm->pDoc->cMax = msGraph.max;
			pfrm->pDoc->cMin = msGraph.min;
			pfrm->pDoc->base = msGraph.base;
			ret = ExecDocFrmView(pfrm);
		}
		else {
			pfrm->pDoc->cmnHead = cmnHeadMCT;	// メモリ管理付
			ret = ExecDocFrmView(pfrm);
			if (ret==0) {
				cmnHeadMCT = pfrm->pDoc->cmnHead;
				hasCmnHeadMCT = TRUE;
			}
		}

		ExecDocFrmViewError(m_pMainWnd->m_hWnd, ret);
	}	
	else {
		CString mesg, noti;
		mesg.LoadString(IDS_STRING_NO_DISPLAY_DATA);
		noti.LoadString(IDS_STR_CNFRM);
		MessageBox(m_pMainWnd->m_hWnd, mesg, noti, MB_OK);
		//MessageBox(m_pMainWnd->m_hWnd, _T("表示できる画像データはありません"), _T("確認"), MB_OK);

		//CString fname = EasyGetOpenFileName("CT画像ファイルを開く", m_pMainWnd->m_hWnd);
		//if (fname.IsEmpty()) return pfrm;
		//openMCTFile(fname, FALSE, vmode);
	}

	return pfrm;
}



//
// Well Known データ用（ファイルオープン）
//		Sun Raster, Jpeg
//
CExFrame*  CCTViewApp::openMGRFile(LPCTSTR fname) 
{
	if (!deleteMData(true)) return NULL;

	CExFrame* pfrm = CreateDocFrmView(pDocTemplMGR, this);
	pfrm->pDoc->msGraph	   = msGraph;		// メモリ管理付
	pfrm->pDoc->cmnHead	   = cmnHeadMCT;	// メモリ管理付
	pfrm->pDoc->ctrlCntrst = forceContrastMode;

	int ret = ExecDocFrmView(pfrm, fname);
	if (ret==0) {
		msGraph       = pfrm->pDoc->msGraph;
		cmnHeadMCT    = pfrm->pDoc->cmnHead;
		preTitle	  = pfrm->pDoc->preTitle;
		Title		  = pfrm->pDoc->Title;
		pstTitle	  = pfrm->pDoc->pstTitle;
		infHead		  = pfrm->pDoc->infHead;
		hasMSGraph    = TRUE;
		hasCmnHeadMCT = TRUE;
		infoGraphDlg->setVal(infHead);
		infoGraphDlg->setVal(msGraph);
	}
	else pfrm = NULL;

	ExecDocFrmViewError(m_pMainWnd->m_hWnd, ret);
	return pfrm;
}



//
//  未知のデータ形式（ファイルオープン）
//
CExFrame*  CCTViewApp::open2DFile(LPCTSTR fname) 
{
	CExFrame* pfrm = CreateDocFrmView(pDocTempl2D, this);
	int ret = ExecDocFrmView(pfrm, fname);
	if (ret==0) {
		preTitle = pfrm->pDoc->preTitle;
		Title	 = pfrm->pDoc->Title;
		pstTitle = pfrm->pDoc->pstTitle;
		infHead	 = pfrm->pDoc->infHead;
		infoGraphDlg->setVal(infHead);
	}
	else pfrm = NULL;

	ExecDocFrmViewError(m_pMainWnd->m_hWnd, ret);
	return pfrm;
}



//
// マルチＣＴファイルデータ保存
//
void  CCTViewApp::OnFileSaveFL() 
{
	TCHAR buf[LMESG];
	CString mesg, noti;

	if (msGraph_ref<=-1) {
		//"保存できる画像データはありません"
		mesg.LoadString(IDS_STR_ERR_NO_SAVE_IMG);
		mesg.LoadString(IDS_STR_CNFRM);
		MessageBox(m_pMainWnd->m_hWnd, mesg, noti, MB_OK);
		return;
	}

	mesg.LoadString(IDS_STR_SPCFY_IMG_TMP_SVFL);
	CString fname = EasyGetSaveFileName((LPCTSTR)mesg, _T(""), m_pMainWnd->m_hWnd);
	//"画像データ保存用ファイルを仮指定する"
	if (fname.IsEmpty()) return;

	// data format
	int format = infHead.kind & 0xff;
	if (format==UN_KNOWN_DATA) {
		format = CT_DATA;
		if (msGraph.zs==3 && msGraph.color==GRAPH_COLOR_RGB) format = JPEG_RGB_DATA;
	}

	// フィル書き込み設定用ダイアログ
	CString fnm = get_file_name_t((LPCTSTR)fname);	// ファイル名部分
	CWriteFilesDLG* wfdlg = new CWriteFilesDLG((LPCTSTR)fnm, format, 0, msGraph.zs-1);
	if (wfdlg->DoModal()!=IDOK) { 
		delete (wfdlg);
		return;
	}
	int fnum = wfdlg->fromNum;
	int tnum = wfdlg->toNum;
	BOOL mlt = wfdlg->mltSave;
	fname    = wfdlg->fName;
	format	 = wfdlg->format;
	delete (wfdlg);

	// サイズ修正
	if (fnum<0)					fnum = 0;
	else if (fnum>msGraph.zs-1) fnum = msGraph.zs - 1;
	if (tnum<fnum)				tnum = fnum;
	else if (tnum>msGraph.zs-1) tnum = msGraph.zs - 1;
	int zsize = tnum - fnum + 1;

	// エラー処理
	BOOL err_occ = FALSE;
	//"フォーマットエラー．%s を 3Dモードで書き込むことはできません"
	mesg.LoadString(IDS_STR_ERR_IMG_FRMT_3D);

	if (format==MOON_DATA && zsize>1 && !mlt) {
		err_occ = TRUE;
		sntprintf(buf, LMESG, mesg, _T(MOON_DATA_STR));
	}

	else if (format==RAS_DATA && zsize>1 && !mlt) { 
		err_occ = TRUE;
		sntprintf(buf, LMESG, mesg, _T(RAS_DATA_STR));
	}

	else if (format==JPEG_MONO_DATA && zsize>1 && !mlt) { 
		err_occ = TRUE;
		sntprintf(buf, LMESG, mesg, _T(JPEG_RGB_DATA_STR));
	}

	// JPEG
	else if (format==JPEG_RGB_DATA) {
		//"フォーマットエラー．%s のカラーチャンネルの数が合いません (%d)"
		mesg.LoadString(IDS_STR_ERR_IMG_FRMT_COL);

		if (zsize==2 || zsize>4) { 
			err_occ = TRUE;
			sntprintf(buf, LMESG, mesg+_T(" (%d)"), _T(JPEG_RGB_DATA_STR), zsize);
		}
		else if (msGraph.color==GRAPH_COLOR_RGB16 || msGraph.color==GRAPH_COLOR_RGBA16 || msGraph.color==GRAPH_COLOR_ARGB16) {
			if (zsize!=1) {
				err_occ = TRUE;
				sntprintf(buf, LMESG, mesg+_T(" [16bit->32bit] (%d)"), _T(JPEG_RGB_DATA_STR), zsize);
			}
		}
		else if (msGraph.color==GRAPH_COLOR_ARGB || msGraph.color==GRAPH_COLOR_RGBA) {
			if (zsize!=4) {
				err_occ = TRUE;
				sntprintf(buf, LMESG, mesg+_T(" [delete alpha channel] (%d)"), _T(JPEG_RGB_DATA_STR), zsize);
			}
		}
	}

	if (err_occ) {
		noti.LoadString(IDS_STR_ERROR);
		MessageBox(m_pMainWnd->m_hWnd, buf, noti, MB_OK);
		return;
	}

	// 書き込み
	if (!fname.IsEmpty()) {
		int ret = writeGraphicFileWithCounter(fname, msGraph, format, mlt, fnum, tnum);
		if (ret<0) {
			//"書き込みエラー．writeCmnHeadFile() [Gio.cpp] を見てください\n\nErrorNo = %d "
			mesg.LoadString(IDS_STR_ERR_WRT_CMNFL);
			noti.LoadString(IDS_STR_ERROR);
			sntprintf(buf, LMESG, mesg, ret);
			MessageBox(m_pMainWnd->m_hWnd, buf, noti, MB_OK);
		}
	}
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 3D簡易サーフェイスレンダリング

//
// ボリュームデータ読み込み
//
void  CCTViewApp::OnFileOpenVOL() 
{
	CString mesg;
	//"ボリュームデータファイルを開く"
	mesg.LoadString(IDS_STR_OPN_VOLFL);
	CString fname = EasyGetOpenFileName((LPCTSTR)mesg, m_pMainWnd->m_hWnd);
	if (fname.IsEmpty()) return;

	CExFrame* pfrm = openVOLFile((LPCTSTR)fname);	
	return;
}



CExFrame*  CCTViewApp::openVOLFile(LPCTSTR fname, BOOL multi) 
{
	if (!deleteVData(true)) return NULL;

	CExFrame* pfrm = CreateDocFrmView(pDocTemplSVOL, this);
	pfrm->pDoc->msGraph        = msGraphVOL;	// メモリ管理付．リファレンスを渡す．
	pfrm->pDoc->multiSliceData = multi;			// マルチファイル読み込み可
	pfrm->pDoc->hasViewData    = TRUE;			// pfrm->pDoc->MakeViewData() を実行しない．
	pfrm->pDoc->ctrlCntrst	   = forceContrastMode;
	((CDxSRenderView*)(pfrm->pView))->volumeColor = volumeColorMode;

	CString mesg;
	mesg.LoadString(IDS_STR_VW_VOLDATA);
	pfrm->pDoc->preTitle = mesg + _T(": ");

	int ret = ExecDocFrmView(pfrm, fname);
	if (ret==0) {
		msGraphVOL    = pfrm->pDoc->msGraph;	// 再表示のために保存．
		preTitle	  = pfrm->pDoc->preTitle;
		Title		  = pfrm->pDoc->Title;
		pstTitle	  = pfrm->pDoc->pstTitle;
		infHead		  = pfrm->pDoc->infHead;
		hasMSGraphVOL = TRUE;
		infoGraphDlg->setVal(infHead);
		infoGraphDlg->setVal(msGraphVOL);
	}

	if (ret==0) {
		pfrm->pView->TimerStart();
	}
	else {
		pfrm = NULL;
		ExecDocFrmViewError(m_pMainWnd->m_hWnd, ret);
	}
	
	return pfrm;
}



//
// 簡易サーフェイスレンダリング：「表示」メニューより選択．
//
void  CCTViewApp::OnViewOpenVOL() 
{
	int  ret;
	CExFrame* pfrm = NULL;
	CString mesg, noti;
	
	if (hasMSGraphVOL) {
		mesg.LoadString(IDS_STR_VW_VOLDATA);
		pfrm = CreateDocFrmView(pDocTemplSVOL, this);
		pfrm->pDoc->msGraph     = msGraphVOL;
		pfrm->pDoc->hasReadData = TRUE;
		pfrm->pDoc->hasViewData = TRUE;
		pfrm->pDoc->ctrlCntrst  = forceContrastMode;
		pfrm->pDoc->preTitle	= mesg + _T(": ");
		//pfrm->pDoc->preTitle	= "ボリュームデータ表示： ";
		pfrm->pDoc->Title		= Title;
		pfrm->pDoc->pstTitle	= pstTitle;
		((CDxSRenderView*)(pfrm->pView))->volumeColor = volumeColorMode;

		ret = ExecDocFrmView(pfrm);		
		if (ret==0) pfrm->pView->TimerStart();
		else ExecDocFrmViewError(m_pMainWnd->m_hWnd, ret);
	}
	else {	
		//CString fname = EasyGetOpenFileName(_T("ボリュームデータファイルを開く"), m_pMainWnd->m_hWnd);
		//if (fname.IsEmpty()) return;
		//pfrm = openVOLFile(fname, TRUE);

		mesg.LoadString(IDS_STR_ERR_NO_DISP_VOL);
		noti.LoadString(IDS_STR_CNFRM);
		MessageBox(m_pMainWnd->m_hWnd, mesg, noti, MB_OK);
		//MessageBox(m_pMainWnd->m_hWnd, "表示できるボリュームデータはありません", "確認", MB_OK);
		return;
	}	

	return;
}



//
// 一時的ボリュームデータの表示
//		MSGraph<sWord>* ms, RgnData* prd は free() される
//
CExFrame*  CCTViewApp::showTempSVOL(MSGraph<sWord>* ms, LPCTSTR title)
{
	if (ms==NULL) return NULL;

	CExFrame* pfrm = CreateDocFrmView(pDocTemplTVOL, this);
	pfrm->pDoc->hasReadData = TRUE;
	pfrm->pDoc->hasViewData = TRUE;
	pfrm->pDoc->msGraph = *ms;

	//pfrm->pDoc->anyData = (void*)prd;	// free()用
	((CDxSRenderView*)(pfrm->pView))->volumeColor = volumeColorMode;
	CString mesg;
	mesg.LoadString(IDS_STR_VW_TMP_VIOLDATA);
	pfrm->pDoc->preTitle = mesg + _T(": ");
	//pfrm->pDoc->preTitle = "一時的ボリュームデータ表示： ";
	pfrm->pDoc->Title	 = title;
	pfrm->pDoc->pstTitle = _T("");

	int ret = ExecDocFrmView(pfrm);	
	if (ret==0) {
		pfrm->pView->TimerStart();
	}
	else {
		pfrm = NULL;
		ExecDocFrmViewError(m_pMainWnd->m_hWnd, ret);
	}

	return pfrm;
}



//
// ボリュームデータ保存
//
void  CCTViewApp::OnFileSaveVOL() 
{	
	TCHAR buf[LMESG];
	CString mesg, noti;
	noti.LoadString(IDS_STR_CNFRM);

	if (msGraphVOL_ref<=-1) {
		//"保存できるボリュームデータはありません"
		mesg.LoadString(IDS_STR_ERR_NO_SAVE_VOL);
		MessageBox(m_pMainWnd->m_hWnd, mesg, noti, MB_OK);
		return;
	}

	//"ボリュームデータ保存用ファイルの指定"
	mesg.LoadString(IDS_STR_SPCFY_VOL_SVFL);
	CString fname = EasyGetSaveFileName((LPCTSTR)mesg, _T(""), m_pMainWnd->m_hWnd);
	if (fname.IsEmpty()) return;

	int ret = MessageBox(m_pMainWnd->m_hWnd, fname, noti, MB_OKCANCEL);
	if (ret!=IDOK) return;

	CString fnm = get_file_name_t((LPCTSTR)fname);	// ファイル名部分

	DEBUG_ERROR("CCTViewApp::OnFileSaveVOL: CT_3D_VOLファイルの書き込み %dx%dx%d", msGraphVOL.xs, msGraphVOL.ys, msGraphVOL.zs);
	ret = writeGraphicFileWithCounter(fnm, msGraphVOL, CT_3D_VOL, FALSE, 0, msGraphVOL.zs-1);
	if (ret<0) {
		mesg.LoadString(IDS_STR_ERR_WRT_CMNFL);
		noti.LoadString(IDS_STR_ERROR);
		sntprintf(buf, LMESG, mesg, ret);
		//"書き込みエラー．writeCmnHeadFile() [Gio.cpp] を見よ．ErrorNo = %d "
		MessageBox(m_pMainWnd->m_hWnd, buf, noti, MB_OK);
	}

	return;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////
// CT画像のサムネイル

//
// データ読み込み
//
void  CCTViewApp::OnFileTn() 
{
	CString mesg;
	//"CT画像ファイルを開く"
	mesg.LoadString(IDS_STR_OPN_CTFL);
	CString fname = EasyGetOpenFileName((LPCTSTR)mesg, m_pMainWnd->m_hWnd);
	if (fname.IsEmpty()) return;

	CExFrame* pfrm = openTBNFile((LPCTSTR)fname);
	return;
}



CExFrame*  CCTViewApp::openTBNFile(LPCTSTR fname, BOOL multi) 
{
	if (!deleteMData(true)) return NULL;

	CExFrame* pfrm = CreateDocFrmView(pDocTemplTBN, this);
	pfrm->pDoc->msGraph        = msGraph;		// メモリ管理付
	pfrm->pDoc->cmnHead        = cmnHeadTBN;	// メモリ管理付
	pfrm->pDoc->multiSliceData = multi;
	pfrm->pDoc->ctrlCntrst     = forceContrastMode;

	int ret = ExecDocFrmView(pfrm, fname);
	if (ret==0) {
		msGraph       = pfrm->pDoc->msGraph;
		cmnHeadTBN    = pfrm->pDoc->cmnHead;
		preTitle	  = pfrm->pDoc->preTitle;
		Title		  = pfrm->pDoc->Title;
		pstTitle	  = pfrm->pDoc->pstTitle;
		infHead		  = pfrm->pDoc->infHead;
		hasMSGraph    = TRUE;
		hasCmnHeadTBN = TRUE;
		infoGraphDlg->setVal(infHead);
		infoGraphDlg->setVal(msGraph);
	}
	
	ExecDocFrmViewError(m_pMainWnd->m_hWnd, ret);
	return pfrm;
}



//
// CT画像のサムネイル：「表示」メニューより選択
//
void  CCTViewApp::OnViewTn() 
{
	int  ret;
	CExFrame* pfrm = NULL;
	CString mesg, noti;

	if (hasMSGraph) {
		mesg.LoadString(IDS_STR_VW_THUMB_IMG);
		pfrm = CreateDocFrmView(pDocTemplTBN, this);
		pfrm->pDoc->msGraph     = msGraph;
		pfrm->pDoc->hasReadData = TRUE;
		pfrm->pDoc->ctrlCntrst  = forceContrastMode;
		pfrm->pDoc->preTitle	= mesg + _T(": ");
		//pfrm->pDoc->preTitle	= "サムネイル表示： ";
		pfrm->pDoc->Title		= Title;
		pfrm->pDoc->pstTitle	= pstTitle;

		if (hasCmnHeadTBN) {
			pfrm->pDoc->cmnHead     = cmnHeadTBN;
			pfrm->pDoc->hasViewData = TRUE;
			ret = ExecDocFrmView(pfrm);
		}
		else {
			pfrm->pDoc->cmnHead = cmnHeadTBN;
			ret = ExecDocFrmView(pfrm);
			if (ret==0) {		
				cmnHeadTBN = pfrm->pDoc->cmnHead;
				hasCmnHeadTBN = TRUE;
			}
		}
		ExecDocFrmViewError(m_pMainWnd->m_hWnd, ret);
	}
	else {
		//CString fname = EasyGetOpenFileName("CT画像ファイルを開く", m_pMainWnd->m_hWnd);
		//if (fname.IsEmpty()) return;
		//openTBNFile(fname, TRUE);
		mesg.LoadString(IDS_STR_ERR_NO_IMG_THUMB);
		noti.LoadString(IDS_STR_ATTNSN);
		MessageBox(m_pMainWnd->m_hWnd, mesg, noti, MB_OK);
		//MessageBox(m_pMainWnd->m_hWnd, "サムネイルを作成するための画像データがありません", "注意", MB_OK);
	}	

	return;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 領域データ

//
// 領域データ読み込み
//
void  CCTViewApp::OnFileOpenRGSL() 
{
	CString mesg;
	//"領域データファイルを開く"
	mesg.LoadString(IDS_STR_OPN_RGNFL);
	CString fname = EasyGetOpenFileName((LPCTSTR)mesg, m_pMainWnd->m_hWnd);
	if (fname.IsEmpty()) return;

	CExFrame* pfrm = openRGNFile((LPCTSTR)fname);
	return;
}



CExFrame*  CCTViewApp::openRGNFile(LPCTSTR fname) 
{
	RgnData*  pRgnData = NULL;

	CExFrame* pfrm = NULL;
	if (centerLineMode) {
		pfrm = CreateDocFrmView(pDocTemplRGN, this);
		int ret = ExecDocFrmView(pfrm, fname);
		if (ret==0) {
			pfrm->pView->TimerStart();
			pRgnData = (RgnData*)(pfrm->pDoc->anyData);
			infHead	 = pfrm->pDoc->infHead;
		}
		else {
			pfrm = NULL;
			ExecDocFrmViewError(m_pMainWnd->m_hWnd, ret);
		}
	}
	else {
		pRgnData = readRgnFileWithCounter(fname);
	}

	//if (pRgnData!=NULL) showSVOL(NULL, pRgnData);

	infoGraphDlg->setVal(infHead);

	return pfrm;
}



/*
//
// 領域データ表示
//
void CCTViewApp::OnViewRgn() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	
}
*/



//
// 領域データ保存
//
void  CCTViewApp::OnFileSaveRGSL() 
{
	TCHAR buf[LMESG];
	CString mesg, noti;
	noti.LoadString(IDS_STR_CNFRM);

	if (rgnDataNum<=0 /*msGraphRGN_ref<=-1*/) {
		//"保存できる領域データはありません"
		mesg.LoadString(IDS_STR_ERR_NO_SAVE_RGN);
		MessageBox(m_pMainWnd->m_hWnd, mesg, noti, MB_OK);
		return;
	}

	//"領域データ保存用ファイルを指定する"
	mesg.LoadString(IDS_STR_SPCFY_RGN_SVFL);
	CString fname = EasyGetSaveFileName((LPCTSTR)mesg, _T(""), m_pMainWnd->m_hWnd);
	if (fname.IsEmpty()) return;

	int ret = MessageBox(m_pMainWnd->m_hWnd, fname, noti, MB_OKCANCEL);
	if (ret!=IDOK) return;

	ret = writeRgnFileWithCounter((LPCTSTR)fname, rgnData, rgnDataNum);
	if (ret<=0) {
		//"書き込みファイルのオープンエラー．%s "
		mesg.LoadString(IDS_STR_ERR_OPN_WRT);
		noti.LoadString(IDS_STR_ERROR);
		sntprintf(buf, LMESG, mesg, get_file_name_t(fname));
		MessageBox(m_pMainWnd->m_hWnd, buf, noti, MB_OK);
	}

	return;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////
//　メモリ

//
// 「メモリ参照数確認」
//
void  CCTViewApp::OnToolShwRefn() 
{
	CShowReferNoDLG* srfdlg = new CShowReferNoDLG(msGraph_ref, msGraphVOL_ref, rgnDataNum, cmnHeadMCT_ref, cmnHeadTBN_ref);
	if (srfdlg==NULL) return;
	srfdlg->DoModal();
	delete (srfdlg);
}



//
// 「読み込みデータ削除」
//
void  CCTViewApp::OnDelMData() 
{
	deleteMData(false);
}



//
// 「ボリュームデータ削除」
//
void  CCTViewApp::OnDelVData() 
{
	deleteVData(false);
}



//
// 「領域データ削除」
//
void  CCTViewApp::OnDelRData() 
{
	deleteRData(false);
}



bool  CCTViewApp::deleteMData(bool nocnfrm) 
{
	if (hasMSGraph) {
		CString mesg, noti;

		if (msGraph_ref>1 || cmnHeadTBN_ref>1 || cmnHeadMCT_ref>1) {
			CString mesg, noti;
			mesg.LoadString(IDS_STR_PL_CLOSE_IMG);
			noti.LoadString(IDS_STR_ATTNSN);
			MessageBox(NULL, mesg, noti, MB_OK);
			//MessageBox(NULL, "メインの画像データを使用中のウィンドウがあります\nメインデータのウィンドウを閉じてください", "注意", MB_OK);
			return false;
		}
		else {
			if (!nocnfrm) {
				mesg.LoadString(IDS_STR_DO_DEL_IMG);
				noti.LoadString(IDS_STR_DEL_DATA);
				int ret = MessageBox(NULL, mesg, noti, MB_OKCANCEL);
				//int ret = MessageBox(NULL, "メインの画像データを削除しますか？", "データ削除", MB_OKCANCEL);
				if (ret==IDOK) nocnfrm = true;
				else		   return false;
			}
			if (nocnfrm) {
				cmnHeadTBN.mfree();
				cmnHeadMCT.mfree();
				msGraph.mfree();
				init_CmnHead(&infHead);
				hasMSGraph    = FALSE;
				hasCmnHeadTBN = FALSE;
				hasCmnHeadMCT = FALSE;
			}
		}
	}
	return true;
}



bool  CCTViewApp::deleteVData(bool nocnfrm) 
{
	if (hasMSGraphVOL) {
		CString mesg, noti;

		if (msGraphVOL_ref>1) {
			mesg.LoadString(IDS_STR_PL_CLOSE_VOL);
			noti.LoadString(IDS_STR_ATTNSN);
			MessageBox(NULL, mesg, noti, MB_OK);
			//MessageBox(NULL, "ボリュームデータを使用中のウィンドウがあります\nボリュームデータのウィンドウを閉じてください", "注意", MB_OK);
			return false;
		}
		else {
			if (!nocnfrm) {
				mesg.LoadString(IDS_STR_DO_DEL_VOL);
				noti.LoadString(IDS_STR_DEL_DATA);
				int ret = MessageBox(NULL, mesg, noti, MB_OKCANCEL);
				//int ret = MessageBox(NULL, "ボリュームデータを削除しますか？", "データ削除", MB_OKCANCEL);
				if (ret==IDOK) nocnfrm = true;
				else           return false;
			}
			if (nocnfrm) {
				msGraphVOL.mfree();
				hasMSGraphVOL = FALSE;
			}
		}
	}
	return true;
}



bool  CCTViewApp::deleteRData(bool nocnfrm) 
{
	if (rgnDataNum>0) {
		if (!nocnfrm) {
			CString mesg, noti;
			mesg.LoadString(IDS_STR_DO_DEL_RGN);
			noti.LoadString(IDS_STR_DEL_DATA);
			int ret = MessageBox(NULL, mesg, noti, MB_OKCANCEL);
			//int ret = MessageBox(NULL, "領域データを削除しますか？", "データ削除", MB_OKCANCEL);
			if (ret==IDOK) nocnfrm = true;
			else           return false;
		}
		if (nocnfrm) {
			for (int i=0; i<rgnDataNum; i++) delete(rgnData[i]); 
			rgnDataNum = 0;
		}
	}
	return true;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////
//　ツール

//
// 「動作設定」
//
void  CCTViewApp::OnToolSet() 
{
	//CSetModeDLG* smdlg = new CSetModeDLG(volumeColorMode, centerLineMode, Dx9DividePrimitiveMode, forceContrastMode, tempBase);
	CSetModeDLG* smdlg = new CSetModeDLG(volumeColorMode, Dx9ReverseZMode, Dx9DividePrimitiveMode, forceContrastMode, tempBase);
	if (smdlg==NULL) return;
	if (smdlg->DoModal()!=IDOK) {
		delete (smdlg);
		return;
	}

	Dx9DividePrimitiveMode = smdlg->pmMode;
	Dx9ReverseZMode		= smdlg->rzMode;
	volumeColorMode		= smdlg->vcMode;
	centerLineMode		= smdlg->clMode;
	forceContrastMode	= smdlg->acMode;
	tempBase = TempBase = smdlg->fBase;

	delete (smdlg);
}



//
// 「ファイルヘッダ設定」
//
void CCTViewApp::OnToolHeader() 
{
	SetHeaderDLG.DoModal();
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// コールバック関数
//
//  return: NULL -- 失敗

// APP_CALLBACK_2D:		簡易画像データの簡易表示
// APP_CALLBACK_MGR:	MGR/MCT表示用
// APP_CALLBACK_REPL:	画像データのメインデータ化
// APP_CALLBACK_SVOL:	3Dボリュームデータ表示用
// APP_CALLBACK_TVOL:	結合用メニューバー有りの3Dボリューム表示
// APP_CALLBACK_VOLM:	3Dボリュームデータの結合用
//


CExFrame*  CCTViewApp::TemplateCallBack(CExFrame* pf, int mode, LPCTSTR title) 
{
	CExFrame* pfrm = NULL;

	// 簡易画像データの簡易表示
	if (mode==APP_CALLBACK_2D) {
		pfrm = CreateDocFrmView(pDocTempl2D, this);
		if (pfrm!=NULL) {
			pfrm->pDoc->hasReadData = TRUE;
			pfrm->pDoc->msGraph = callMSGraph;	// pDocが distructしたときのcallMSGraphの処理に注意!!

			int ret = ExecDocFrmView(pfrm);
			ExecDocFrmViewError(m_pMainWnd->m_hWnd, ret);
		}
	}

	// MCT表示用
	else if (mode==APP_CALLBACK_MGR) {
		pfrm = viewMCTXYZ(VIEW_ZDIR);
	}

	// 3Dボリュームデータ表示用
	else if (mode==APP_CALLBACK_SVOL) {
		OnViewOpenVOL();
		pfrm = pf;
	}

	// 画像データのメインデータ化
	else if (mode==APP_CALLBACK_REPL) {
		CString mesg, noti;
		mesg.LoadString(IDS_STR_CRT_MAIN_IMGDATA);
		noti.LoadString(IDS_STR_CNFRM);
		int ret = MessageBox(m_pMainWnd->m_hWnd, mesg, noti, MB_OKCANCEL);
		//int ret = MessageBox(m_pMainWnd->m_hWnd, "メインの画像データを破棄して，この画像をメインのデータとします", "確認", MB_OKCANCEL);

		if (ret==IDOK) {
			if (deleteMData(true)) {
				copy_MSGraph(callMSGraph, msGraph);
				if (msGraph.gp!=NULL) {
					hasMSGraph = TRUE;
					msGraph.setRefer(&msGraph_ref);

					infHead  = pf->pDoc->infHead;
					int kind = infHead.kind & 0xff;
					if (kind==CREATE_DATA) Title = pf->pDoc->Title;
					preTitle = pf->pDoc->preTitle;
					pstTitle = pf->pDoc->pstTitle;

					infoGraphDlg->setVal(infHead);
					infoGraphDlg->setVal(msGraph);
					pfrm = pf;
				}
				else {
					mesg.LoadString(IDS_STR_FAIL_COPY_IMGDATA);
					noti.LoadString(IDS_STR_ERR_MEMORY);
					MessageBox(m_pMainWnd->m_hWnd, mesg, noti+_T("?"), MB_OK);
					//MessageBox(m_pMainWnd->m_hWnd, "画像データのコピーに失敗しました", "メモリ不足？", MB_OK);
				}
			}
		}
	}

	// 結合用メニューバー有りの一時的3Dボリューム表示
	else if (mode==APP_CALLBACK_TVOL) {
		pfrm = showTempSVOL(&callMSGraph, title);
	}

	// 3Dボリュームデータの結合
	else if (mode==APP_CALLBACK_VOLM) {
		if (hasMSGraphVOL) {
			cat_MSGraph (callMSGraph, msGraphVOL);
			pfrm = pf;
		}
		else {
			copy_MSGraph(callMSGraph, msGraphVOL);
			if (msGraphVOL.gp!=NULL) {
				hasMSGraphVOL = TRUE;
				msGraphVOL.setRefer(&msGraphVOL_ref);	// (*msGraphVOL.refno) = 1;
				pfrm = pf;
			}
			else {
				CString mesg, noti;
				mesg.LoadString(IDS_STR_FAIL_COPY_IMGDATA);
				noti.LoadString(IDS_STR_ERR_MEMORY);
				MessageBox(m_pMainWnd->m_hWnd, mesg, noti+_T("?"), MB_OK);
				//MessageBox(m_pMainWnd->m_hWnd, "データのコピに失敗しました", "メモリ不足？", MB_OK);
			}
		}

	}

	return pfrm;
}




//////////////////////////////////////////////////////////////////////////////
// Junk Code

		// 再帰的領域探索
		/*
		if (callMSGraph.isNull()) {	
			RgnData* pRgnData = readRgnFileWithCounter(TEMP_RGN_FILENAME);
			if (pRgnData!=NULL) {
				if (centerLineMode) {
					pfrm = CreateDocFrmView(pDocTemplRGN, this);
					pfrm->pDoc->hasReadData = TRUE;
					pfrm->pDoc->hasViewData = TRUE;
					pfrm->pDoc->anyData = (void*)pRgnData;
					//pfrm->pView->preTitle = title + "３次元表示： ";
					int ret = ExecDocFrmView(pfrm);
					if (ret==0) pfrm->pView->TimerStart();
					else ExecDocFrmViewError(m_pMainWnd->m_hWnd, ret);
				}
			}
			showSVOL(NULL, pRgnData);
		}
		else {	// リージョングローイング
		}*/


		// 領域データの確保
		/*
		if (pf->anyData!=NULL) {
			if (rgnDataNum<RGNDATA_MAXNUM) {
				rgnData[rgnDataNum] = (RgnData*)(pf->anyData);
				for (int i=0; i<rgnData[rgnDataNum]->no; i++) rgnData[rgnDataNum]->rs[i].gn = rgnDataNum;
				rgnDataNum++;
			}
			else {
				DEBUG_ERR("TemplateCallBack(mode=%d): 最大領域データ数(%d)を超えた．", mode, RGNDATA_MAXNUM);
			}
		}*/



//
//
//
CMultiDocTemplate*  CCTViewApp::GetMultiDocTemplate(int mode)
{
	// メニューバー無しを返す（メニューバーがあるとややこしい）
	switch (mode) {
		case APP_TEMPL_2D:	 return pDocTempl2D; 
		case APP_TEMPL_SMGR: return pDocTemplSMGR;
		case APP_TEMPL_SVOL: return pDocTemplSVOL;
		//
		case APP_TEMPL_VTHUMBNAILFRAME: return pDocTemplSMGR;
		default:			 return NULL;
	}
}




