// MGRFrame.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"

#include "GraphLib.h"
#include "ExClass.h"
#include "WinTools.h"

#include "MGRFrame.h"
#include "ContrastDLG.h"
#include "CTCallBack.h"

//#include "CT.h"
//#include "RegionGDLG.h"
//#include "ProgressTextDLG.h"

//#include "RecursiveRSDLG.h"
//#include "SearchStart.h"
//#include "DxSMPLDisp.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace jbxl;
using namespace jbxwl;


/////////////////////////////////////////////////////////////////////////////
// CMGRFrame

IMPLEMENT_DYNCREATE(CMGRFrame, CExFrame)

CMGRFrame::CMGRFrame()
{
	calledMousePoint = false;
}


CMGRFrame::~CMGRFrame()
{

}


BEGIN_MESSAGE_MAP(CMGRFrame, CExFrame)
	//{{AFX_MSG_MAP(CMGRFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_MGR_CONTRAST, OnMgrContrast)
	ON_COMMAND(ID_MGR_TOVOL, OnMgrToVOL)
	//ON_COMMAND(ID_MGR_RG,  OnMgrRgnGrwng)
	//ON_COMMAND(ID_MGR_RRS, OnMgrRcrsvRgnSrch)
	//}}AFX_MSG_MAP
//	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CMGRFrame メッセージ ハンドラ
//
int CMGRFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CExFrame::OnCreate(lpCreateStruct) == -1) return -1;

	// ツールバーの作成
	toolBar = new CExToolBar();
	if (!toolBar->CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP 
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | WS_CLIPCHILDREN) ||
		!toolBar->LoadToolBar(IDR_MGR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // 作成に失敗
	}

	// TODO: ツール バーをドッキング可能にしない場合は以下の３行を削除してください。     
	toolBar->EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(toolBar);
	
	return 0;
}


//
// コントラスト設定
//
void CMGRFrame::OnMgrContrast() 
{
	CContrastDLG* cntDLG = new CContrastDLG(pView);
	if (cntDLG!=NULL) {
		cntDLG->DoModal();	//ShowWindow(SW_SHOW);	// ->DoModal();
		delete(cntDLG);
	}
}


//
// 下位の関数のポインタを獲得して実行
//
void CMGRFrame::OnMgrToVOL() 
{
	pApp->callMSGraph.init();
	copy_MSGraph(pDoc->msGraph, pApp->callMSGraph);
	if (!pApp->callMSGraph.isNull()) {
		anyData = NULL;		// 領域データは無し．
		DEBUG_WARN("CMGRFrame::OnMgrToVOL(): TemplateCallBack: START");
		CExFrame* frm = pApp->TemplateCallBack(this, APP_CALLBACK_TVOL, pDoc->Title);
		//if (frm==NULL)  pApp->callMSGraph.free();	// 3D表示失敗
		DEBUG_WARN("CMGRFrame::OnMgrToVOL(): TemplateCallBack: END");
	}
	else {//IDS_STR_ERR_MEMORY
		CString mesg, noti;
		mesg.LoadString(IDS_STR_FAIL_COPY_FLDATA);
		noti.LoadString(IDS_STR_ERR_MEMORY);
		MessageBox(_T("CMGRFrame::OnMgrToVOL\n\n")+mesg, noti+_T("?"), MB_OK);
		//IDS_STR_FAIL_COPY_FLDATA
	}	
}


//
// 操作中にウィンドウのクローズボタンをクリックすると，セグメンテーションエラーを起こす．
//
POINT  CMGRFrame::GetMousePoint(int click) 
{
	int   i, j, mousebtn, val;
	int   xx, yy, zz, base;
	POINT mousepos, oldmspos;
	TCHAR buf[LNAME];


	base = pDoc->base + TempBase;
	pView->GetClientRect(&pView->clientRect);

	oldmspos.x = oldmspos.y = -1;
	//uByte* ptr = (uByte*)&(CmnHeadPoint(pView->cmnHead, 0, 0, pView->vSBpos));
	uByte* ptr = pView->viewData.grptr;

	do {
		mousepos   = pView->GetMousePos();
		mousepos.x = (int)(mousepos.x*(pView->viewData.xsize-1)/(float)pView->clientRect.right  + 0.5);
		mousepos.y = (int)(mousepos.y*(pView->viewData.ysize-1)/(float)pView->clientRect.bottom + 0.5);
		
		// 縦線の消去と表示
		if (mousepos.x!=oldmspos.x) {
			if (oldmspos.x>=0 && oldmspos.x<pView->viewData.xsize) {
				for (j=0; j<pView->viewData.ysize; j++) {
					ptr[oldmspos.x + j*pView->viewData.xsize] ^= 0xff;
				}
			}
			if (mousepos.x>=0 && mousepos.x<pView->viewData.xsize) {
				for (j=0; j<pView->viewData.ysize; j++) {
					ptr[mousepos.x + j*pView->viewData.xsize] ^= 0xff;
				}
			}
		}
		
		// 横線の消去と表示
		if (mousepos.y!=oldmspos.y) {
			if (oldmspos.y>=0 && oldmspos.y<pView->viewData.ysize) {
				for (i=0; i<pView->viewData.xsize; i++) {
					ptr[i + oldmspos.y*pView->viewData.xsize] ^= 0xff;
				}
			}
			if (mousepos.y>=0 && mousepos.y<pView->viewData.ysize) {
				for (i=0; i<pView->viewData.xsize; i++) {
					ptr[i + mousepos.y*pView->viewData.xsize] ^= 0xff;
				}
			}
		}
	
		// 画面の書き換え
		if (mousepos.x!=oldmspos.x || mousepos.y!=oldmspos.y) {
			if ((mousepos.x>=0 && mousepos.x<pView->viewData.xsize) &&
				(mousepos.y>=0 && mousepos.y<pView->viewData.ysize)) {

				if (pView->viewMode==0) {
					xx = mousepos.x;
					yy = mousepos.y;
					zz = pView->vSBpos;
				}
				else if (pView->viewMode==1) {
					xx = pView->vSBpos;
					yy = mousepos.x;
					zz = mousepos.y;
				}
				else if (pView->viewMode==2) {
					xx = mousepos.x;
					yy = pView->vSBpos;
					zz = mousepos.y;
				}
				val = pDoc->msGraph.point(xx, yy, zz);
			}
			else  xx = yy = zz = val = 0;
			sntprintf(buf, LNAME, _T("    ( %3d,  %3d,  %3d ) : %8d"), xx, yy, zz, val - base);
			
			pView->SetNewSurface();
			SetTitle(Title + buf);
			pDoc->SetTitle(Title + buf);
			SetWindowText(Title + buf);
			pView->OnDraw(NULL);
		}
		oldmspos = mousepos;
		mousebtn = pView->GetMouseButton();
		
//		DisPatcher(1, WM_NCLBUTTONDOWN);
		DisPatcher();
		if (isNull(pView)) return mousepos;		// 異常終了

//	} while (mousebtn!=click || mousepos.x<0 || mousepos.y<0);
	} while (mousebtn<=0 || mousepos.x<0 || mousepos.y<0);


	// 線の消去
	if (oldmspos.x>=0 && oldmspos.x<pView->viewData.xsize) {
		for (j=0; j<pView->viewData.ysize; j++) {
			ptr[oldmspos.x + j*pView->viewData.xsize] ^= 0xff;
		}
	}
	if (oldmspos.y>=0 && oldmspos.y<pView->viewData.ysize) {
		for (i=0; i<pView->viewData.xsize; i++) {
			ptr[i + oldmspos.y*pView->viewData.xsize] ^= 0xff;
		}
	}

	pView->SetNewSurface();

	SetTitle(Title);
	pDoc->SetTitle(Title);
	SetWindowText(Title);
	pView->OnDraw(NULL);
	
	if (mousebtn!=click) mousepos.x = mousepos.y = -1;
	return  mousepos;
}


/////////////////////////////////////////////////////////////////////////////////////
// メモ：メッセージ処理サンプル
//

/*
	MSG msg;
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
*/

/*
MSG msg;
while (PeekMessage(&msg,0,0,0,PM_REMOVE)){
	if (IsDialogMessage(&msg)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
*/

/*
if( ( (MSG *)lParam )->message == WM_MOUSEWHEEL )
{
	TRACE0( "Hit!(Hook)\n" );
	( (MSG *)lParam )->message = WM_NULL;	//ここで書き換えちゃう。
}
*/

/*	
	MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
		TranslateMessage(&msg);
		if (msg.message==WM_MOUSEWHEEL) DEBUG_ERRORor("WHEEL");
		if (msg.message==WM_CLOSE) {
			DEBUG_ERRORor("DESTROY");
			msg.message = WM_NULL;
		}
		DispatchMessage(&msg);
	}
*/

/*
 LPMSG＝MSG FAR * （Win16)；MSG * （Win32) 
 MSG msg; ⇒ LPMSG lpmag = &msg;
 
 typedef struct tagMSG {  
	HWND hwnd;  
	UINT message;  
	WPARAM wParam;  
	LPARAM lParam;  
	DWORD time;  
	POINT pt;  
 } MSG;   
   
 hwnd：メッセージを受け取るウィンドウ プロシージャを持つウィンドウを識別します。 
 message：メッセージ番号を示します。 
 wParam：メッセージの付加情報を示します。意味は message メンバの値によって特定されます。 
 lParam：メッセージの付加情報を示します。意味は message メンバの値によって特定されます。 
 time：メッセージがポストされた時間を示します。 
 pt：メッセージがポストされたときのカーソル位置をスクリーン座標で示します。 

９節へ戻る
メッセージの種類
 #define WM_NULL 0x0000  
 #define WM_CREATE 0x0001  
 #define WM_DESTROY 0x0002  
 #define WM_MOVE 0x0003  
 #define WM_SIZE 0x0005  
 #define WM_ACTIVATE 0x0006  
 #define WM_SETFOCUS 0x0007  
 #define WM_KILLFOCUS 0x0008  
 #define WM_ENABLE 0x000A  
 #define WM_SETREDRAW 0x000B  
 #define WM_SETTEXT 0x000C  
 #define WM_GETTEXT 0x000D  
 #define WM_GETTEXTLENGTH 0x000E  
 #define WM_PAINT 0x000F  
 #define WM_CLOSE 0x0010  
 #define WM_QUERYENDSESSION 0x0011  
 #define WM_QUIT 0x0012  
 #define WM_QUERYOPEN 0x0013  
 #define WM_ERASEBKGND 0x0014  
 #define WM_SYSCOLORCHANGE 0x0015  
 #define WM_ENDSESSION 0x0016  
 #define WM_SHOWWINDOW 0x0018  
 #define WM_WININICHANGE 0x001A  
 
 Windows95/98以上  
	#define WM_SETTINGCHANGE WM_WININICHANGE  
 
 #define WM_DEVMODECHANGE 0x001B  
 #define WM_ACTIVATEAPP 0x001C  
 #define WM_FONTCHANGE 0x001D  
 #define WM_TIMECHANGE 0x001E  
 #define WM_CANCELMODE 0x001F  
 #define WM_SETCURSOR 0x0020  
 #define WM_MOUSEACTIVATE 0x0021  
 #define WM_CHILDACTIVATE 0x0022  
 #define WM_QUEUESYNC 0x0023  
 #define WM_GETMINMAXINFO 0x0024  
 #define WM_PAINTICON 0x0026  
 #define WM_ICONERASEBKGND 0x0027  
 #define WM_NEXTDLGCTL 0x0028  
 #define WM_SPOOLERSTATUS 0x002A  
 #define WM_DRAWITEM 0x002B  
 #define WM_MEASUREITEM 0x002C  
 #define WM_DELETEITEM 0x002D  
 #define WM_VKEYTOITEM 0x002E  
 #define WM_CHARTOITEM 0x002F  
 #define WM_SETFONT 0x0030  
 #define WM_GETFONT 0x0031  
 #define WM_SETHOTKEY 0x0032  
 #define WM_GETHOTKEY 0x0033  
 #define WM_QUERYDRAGICON 0x0037  
 #define WM_COMPAREITEM 0x0039  

 Windows2000  
	#define WM_GETOBJECT 0x003D 
 
 #define WM_COMPACTING 0x0041 
 #define WM_COMMNOTIFY 0x0044 (機能は使えません) 
 #define WM_WINDOWPOSCHANGING 0x0046 
 #define WM_WINDOWPOSCHANGED 0x0047 
 #define WM_POWER 0x0048 
 #define WM_COPYDATA 0x004A 
 #define WM_CANCELJOURNAL 0x004B 

 Windows95/98/NT4.0 
	#define WM_NOTIFY 0x004E 
	#define WM_INPUTLANGCHANGEREQUEST 0x0050 
	#define WM_INPUTLANGCHANGE 0x0051 
	#define WM_TCARD 0x0052 
	#define WM_HELP 0x0053 
	#define WM_USERCHANGED 0x0054 
	#define WM_NOTIFYFORMAT 0x0055 
	#define WM_CONTEXTMENU 0x007B 
	#define WM_STYLECHANGING 0x007C 
	#define WM_STYLECHANGED 0x007D 
	#define WM_DISPLAYCHANGE 0x007E 
	#define WM_GETICON 0x007F 
	#define WM_SETICON 0x0080 
 
 #define WM_NCCREATE 0x0081 
 #define WM_NCDESTROY 0x0082 
 #define WM_NCCALCSIZE 0x0083 
 #define WM_NCHITTEST 0x0084 
 #define WM_NCPAINT 0x0085 
 #define WM_NCACTIVATE 0x0086 
 #define WM_GETDLGCODE 0x0087 
 #define WM_SYNCPAINT 0x0088 
 #define WM_NCMOUSEMOVE 0x00A0 
 #define WM_NCLBUTTONDOWN 0x00A1 
 #define WM_NCLBUTTONUP 0x00A2 
 #define WM_NCLBUTTONDBLCLK 0x00A3 
 #define WM_NCRBUTTONDOWN 0x00A4 
 #define WM_NCRBUTTONUP 0x00A5 
 #define WM_NCRBUTTONDBLCLK 0x00A6 
 #define WM_NCMBUTTONDOWN 0x00A7 
 #define WM_NCMBUTTONUP 0x00A8 
 #define WM_NCMBUTTONDBLCLK 0x00A9 
 #define WM_KEYFIRST 0x0100 
 #define WM_KEYDOWN 0x0100 
 #define WM_KEYUP 0x0101 
 #define WM_CHAR 0x0102 
 #define WM_DEADCHAR 0x0103 
 #define WM_SYSKEYDOWN 0x0104 
 #define WM_SYSKEYUP 0x0105 
 #define WM_SYSCHAR 0x0106 
 #define WM_SYSDEADCHAR 0x0107 
 #define WM_KEYLAST 0x0108 

 Windows95/98以上 
	#define WM_IME_STARTCOMPOSITION 0x010D 
	#define WM_IME_ENDCOMPOSITION 0x010E 
	#define WM_IME_COMPOSITION 0x010F 
	#define WM_IME_KEYLAST 0x010F 
 
 #define WM_INITDIALOG 0x0110 
 #define WM_COMMAND 0x0111 
 #define WM_SYSCOMMAND 0x0112 
 #define WM_TIMER 0x0113 
 #define WM_HSCROLL 0x0114 
 #define WM_VSCROLL 0x0115 
 #define WM_INITMENU 0x0116 
 #define WM_INITMENUPOPUP 0x0117 
 #define WM_MENUSELECT 0x011F 
 #define WM_MENUCHAR 0x0120 
 #define WM_ENTERIDLE 0x0121
 
 Windows2000 
	#define WM_MENURBUTTONUP 0x0122 
	#define WM_MENUDRAG 0x0123 
	#define WM_MENUGETOBJECT 0x0124 
	#define WM_UNINITMENUPOPUP 0x0125 
	#define WM_MENUCOMMAND 0x0126 
 
 #define WM_CTLCOLORMSGBOX 0x0132 
 #define WM_CTLCOLOREDIT 0x0133 
 #define WM_CTLCOLORLISTBOX 0x0134 
 #define WM_CTLCOLORBTN 0x0135 
 #define WM_CTLCOLORDLG 0x0136 
 #define WM_CTLCOLORSCROLLBAR 0x0137 
 #define WM_CTLCOLORSTATIC 0x0138 
 #define WM_MOUSEFIRST 0x0200 
 #define WM_MOUSEMOVE 0x0200 
 #define WM_LBUTTONDOWN 0x0201 
 #define WM_LBUTTONUP 0x0202 
 #define WM_LBUTTONDBLCLK 0x0203 
 #define WM_RBUTTONDOWN 0x0204 
 #define WM_RBUTTONUP 0x0205 
 #define WM_RBUTTONDBLCLK 0x0206 
 #define WM_MBUTTONDOWN 0x0207 
 #define WM_MBUTTONUP 0x0208 
 #define WM_MBUTTONDBLCLK 0x0209
 
 Windows95/98/NT4.0以上 
	#define WM_MOUSEWHEEL 0x020A 
	#define WM_MOUSELAST 0x020A 
 
 Windows3.1 
	#define WM_MOUSELAST 0x0209 
 
 WindowsNT4.0以上 
	#define WHEEL_DELTA 120 
	#define WHEEL_PAGESCROLL (UINT_MAX) 
 
 #define WM_PARENTNOTIFY 0x0210 
 #define WM_ENTERMENULOOP 0x0211 
 #define WM_EXITMENULOOP 0x0212 
 
 Windows95/98以上 　  
	#define WM_NEXTMENU 0x0213 
	#define WM_SIZING 0x0214 
	#define WM_CAPTURECHANGED 0x0215 
	#define WM_MOVING 0x0216 
	#define WM_POWERBROADCAST 0x0218 
	#define WM_DEVICECHANGE 0x0219 
 
 #define WM_MDICREATE 0x0220 
 #define WM_MDIDESTROY 0x0221 
 #define WM_MDIACTIVATE 0x0222 
 #define WM_MDIRESTORE 0x0223 
 #define WM_MDINEXT 0x0224 
 #define WM_MDIMAXIMIZE 0x0225 
 #define WM_MDITILE 0x0226 
 #define WM_MDICASCADE 0x0227 
 #define WM_MDIICONARRANGE 0x0228 
 #define WM_MDIGETACTIVE 0x0229 
 #define WM_MDISETMENU 0x0230 
 #define WM_ENTERSIZEMOVE 0x0231 
 #define WM_EXITSIZEMOVE 0x0232 
 #define WM_DROPFILES 0x0233 
 #define WM_MDIREFRESHMENU 0x0234 
 
 Windows95/98以上 
	#define WM_IME_SETCONTEXT 0x0281 
 	#define WM_IME_NOTIFY 0x0282 
	#define WM_IME_CONTROL 0x0283 
	#define WM_IME_COMPOSITIONFULL 0x0284 
	#define WM_IME_SELECT 0x0285 
	#define WM_IME_CHAR 0x0286 
 
 Windows2000 
	#define WM_IME_REQUEST 0x0288 
 
 Windows95/98以上 
	#define WM_IME_KEYDOWN 0x0290 
	#define WM_IME_KEYUP 0x0291 
 
 WindowsNT4.0以上 
	#define WM_MOUSEHOVER 0x02A1 
	#define WM_MOUSELEAVE 0x02A3 
 
 #define WM_CUT 0x0300 
 #define WM_COPY 0x0301 
 #define WM_PASTE 0x0302 
 #define WM_CLEAR 0x0303 
 #define WM_UNDO 0x0304 
 #define WM_RENDERFORMAT 0x0305 
 #define WM_RENDERALLFORMATS 0x0306 
 #define WM_DESTROYCLIPBOARD 0x0307 
 #define WM_DRAWCLIPBOARD 0x0308 
 #define WM_PAINTCLIPBOARD 0x0309 
 #define WM_VSCROLLCLIPBOARD 0x030A 
 #define WM_SIZECLIPBOARD 0x030B 
 #define WM_ASKCBFORMATNAME 0x030C 
 #define WM_CHANGECBCHAIN 0x030D 
 #define WM_HSCROLLCLIPBOARD 0x030E 
 #define WM_QUERYNEWPALETTE 0x030F 
 #define WM_PALETTEISCHANGING 0x0310 
 #define WM_PALETTECHANGED 0x0311 
 #define WM_HOTKEY 0x0312
 
 Windows95/98以上 
	#define WM_PRINT 0x0317 
	#define WM_PRINTCLIENT 0x0318 
	#define WM_HANDHELDFIRST 0x0358 
	#define WM_HANDHELDLAST 0x035F 
	#define WM_AFXFIRST 0x0360 
	#define WM_AFXLAST 0x037F 
 
 #define WM_PENWINFIRST 0x0380 
 #define WM_PENWINLAST 0x038F 
 
 Windows95/98以上 
	#define WM_APP 0x8000 
 
 0x0400以下の値は将来のシステム拡張用です。自前のメッセージはWM_USERから始まります。 
	#define WM_USER 0x0400  


ウィンドウメッセージ 一覧表示 メッセージ名 意味 

WM_ACTIVATE					ウインドウがアクティブ・非アクティブになるとき 
WM_ACTIVATEAPP				別のアプリケーションのウインドウがアクティブ・非アクティブになるとき 
WM_ASKCBFORMATNAME			クリップボードのフォーマット名を文字列バッファにコピーを要求 
WM_CANCELMODEJOURNAL		ジャーナル処理がキャンセルされた 
WM_CANCELMODE				メッセージボックス等のダイアログボックスが表示されようとしている 
WM_CAPTURECHAINGED			マウスのキャプチャが変化した 
WM_CHANGECBCHAIN			クリップボ−ドビューワチェーンが変更される 
WM_CHAR						キーボードから文字入力を受け取った 
WM_CHARTOITEM				WM_CHARに応じてオーナー描画リストボックスが受け取る 
WM_CHILDACTIVATE			MDIコントロールの子ウインドウがアクティブ化 
WM_CHOOSEFONT_GETLOGFONT	[フォントの指定]ダイアログボックスに送るメッセージ 
WM_CLEAR					テキストボックス・コンボボクスの選択テキストを削除 
WM_CLOSE					ウインドウあるいはアプリケーションをクローズされた 
WM_COMMAND					メニューが選択されたあるいはコントロールにイベントが発生した 
WM_COMPAREITEM				並び替えが指定された新しい項目のリストボックスなどで相対的な位置を決定する。 
WM_COMPACTING				システムがメモリ不足の状態にある 
WM_COPY						テキストボックス，コンボボックスの選択テキストをクリップボードにコピーしたとき 
WM_COPYDATA					別のプロセス上のウィンドウにデータを送信する 
WM_CREATE					ウインドウが作成中 
WM_CTLCOLORBTN				コマンドボタンが描画されようとしている 
WM_CTLCOLORDLG				ダイアログが描画されようとしている 
WM_CTLCOLOREDIT				テキストボックスが描画されようとしている 
WM_CTLCOLORLISTBOX			リストボックスが描画されようとしている 
WM_CTLCOLORMSGBOX			メッセージボックスが描画されようとしている 
WM_CTLCOLORSCROLLBAR		スクロールバーが描画されようとしている 
WM_CTLCOLORSTATIC			ラベルが描画されようとしている 
WM_CUT						テキストボックス，コンボボックスの選択テキストをクリップボードにコピー 
WM_DEADCHAR					アクセント記号のような文字と組合せた文字を受け取った 
WM_DELETEITEM				オーナ描画リストボックスの項目が削除された 
WM_DESTROY					ウインドウが破棄された 
WM_DESTROYCLIPBOARD			EmptyClipboard()によってクリップボードが空になった 
WM_DEVICECHANGE				ハードウェアの構成が変更された 
WM_DEVMODECHANGE			WIN.INI のデフォルトのデバイス名が変更された 
WM_DISPLAYCHANGE			ディスプレイの解像度が変更された 
WM_DRAWCLIPBOARD			クリップボードの内容が変わった 
WM_DRAWITEM					オーナー描画コントロールの外観が変わった 
WM_DROPFILES				ファイルマネージャからの Drag & Drop 
WM_ENABLE					ウインドウが有効・無効になった 
WM_ENDSESSION				Windows が終了する 
WM_ENTERIDLE				ダイアログボックス，メニューが入力待ち 
WM_ENTERMENULOOP			モーダルなメニューループに入ったとき 
WM_ENTERSIZEMOVE			ウィンドウが移動・サイズ変更した 
WM_ERASEBKGND				バックグラウンドを消去(再ペイント) 
WM_EXITMENULOOP				モーダルなメニューから抜け出した 
WM_EXITSIZEMOVE				ウィンドウの移動・サイズ変更の処理が終了したとき 
WM_FONTCHANGE				システムフォントが変更された 
WM_GETDLGCODE				テキストボックス，ラベル等が現在使っているフォントのハンドル 
WM_GETFONT					テキストボックス，ラベル等が現在使っているフォントのハンドル 
WM_GETHOTKEY				ウィンドウに関連付けられているホットキーを取得する 
WM_GETICON					ウインドウに関連付けられているアイコンのハンドルを取得 
WM_GETMINMAXINFO			フォームの最大・最小化時に可能なサイズを取得 
WM_GETTEXT					コントロールのキャプション，テキストボックスのテキストをバッファにコピー 
WM_GETTEXTLENGTH			ウィンドウに関連付けられているテキストのサイズ(バイト数)を取得 
WM_HELP						[F1] or [?]の押された場合 
WM_HOTKEY					RegisterHotKey関数で登録されたホットキーが押されたとき 
WM_HSCROLL					水平スクロールバーを調整している 
WM_HSCROLLCLIPBOARD			クリップボードビューワで水平スルクールバーが使われた 
WM_ICONERASEBKGND			クラスアイコンがある時，アイコンの描画前に通知 
WM_IME_CHAR					IMEが変換後の文字を受取った 
WM_IME_COMPOSITION			キー入力によってIMEが確定後の状態を変更した 
WM_IME_COMPOSITIONFULL		確定後の文字を表示するウインドウにスペースがない 
WM_IME_CONTROL				IMEにコマンドを送る 
WM_IME_ENDCOMPOSITION		文字を確定した 
WM_IME_KEYDOWN				キーを押した 
WM_IME_KEYUP				キーを離した 
WM_IME_NOTIFY				IMEウインドウの変化をアプリケーションに知らせる 
WM_IME_REQUEST				システムがIMEを変更しようとしている 
WM_IME_SELECT				システムがIMEを変更しようとしている 
WM_IME_SETCONTEXT			IMEの入力ウインドウがアクティブになった時アプリケーションに通知する 
WM_IME_STARTCOMPOSITION		確定する前に通知する 
WM_INITDIALOG				ダイアログボックスが表示される 
WM_INITMENU					メニューが表示される直前に通知 
WM_INITMENUPOPUP			ポップアップメニューが表示される直前に通知 
WM_INPUTLANGCHANGE			言語ロケールが変更されたときトップレベルウィンドウに通知 
WM_INPUTLANGCHANGEREQUEST	言語ロケールが変更されたとき入力フォーカスを持つウィンドウに通知 
WM_KEYDOWN					キーボードのキーが押された 
WM_KEYUP					キーが解放された 
WM_KILLFOCUS				フォーカスがなくなる直前に通知 
WM_LBUTTONDBLCLK			左のマウスボタンをダブルクリックした 
WM_LBUTTONDOWN				左のマウスボタンを押した 
WM_LBUTTONUP				左のマウスボタンが解放された 
WM_MBUTTONDBLCLK			中央のマウスボタンがダブルクリックされた 
WM_MBUTTONDOWN				中央のマウスボタンが押された 
WM_MBUTTONUP				中央のマウスボタンが解放された 
WM_MDIACTIVATE				MDIで指定した子ウインドウをアクティブにする 
WM_MDICASCADE				MDI子ウインドウをカスケード表示にする 
WM_MDICREATE				MDI子ウインドウを作成する 
WM_MDIDESTROY				MDI子ウインドウを破棄する 
WM_MDIGETACTIVE				現在アクティブなMDI子ウインドウのハンドルを取得 
WM_MDIICONARRANGE			MDI子ウインドウのアイコンを整列 
WM_MDIMAXIMIZE				MDI子ウインドウを最大化 
WM_MDINEXT					次の子ウインドウをアクティブにする 
WM_MDIRESTORE				最大化・最小化されている子ウインドウを元のサイズに戻す 
WM_MDISETMENU				メニューをMDIフレームウインドウにリンク 
WM_MDITILE					MDI子ウインドウをタイル整列 
WM_MEASUREITEM				オーナー描画コントロールが作成されたとき 
WM_MENUCHAR					メニューのアクセスキーの英文字と一致しない文字を受け取った 
WM_MENUSELECT				メニューが強調表示・選択された 
WM_MOUSEACTIVATE			マウスクリックによりアクティブウインドウが移った 
WM_MOUSEMOVE				マウスが移動した 
WM_MOUSEWHELL				マウスホイールが回転した 
WM_MOVE						ウインドウが移動した 
WM_MOVING					リサイズ中にウィンドウに送られる 
WM_NCACTIVATE				非クライアント領域がアクティブ・非アクティブである 
WM_NCCALCSIZE				ウインドウのサイズを再計算する 
WM_NCCREATE					ウインドウのスタイルが変更された時に最初に送られるメッセージ 
WM_NCDESTROY				非クライアント領域が破棄 
WM_NCHITTEST				非クライアント領域のどの部分でマウスが操作されたか調べる 
WM_NCLBUTTONDBCLK			非クライアント領域で左ボタンをダブルクリック 
WM_NCLBUTTONDOWN			非クライアント領域で左ボタンを押す 
WM_NCLBUTTONUP				非クライアント領域で左ボタンを解放 
WM_NCMBUTTONDBCLK			非クライアント領域で中央ボタンをダブルクリック 
WM_NCMBUTTONDOWN			非クライアント領域で中央ボタンを押す 
WM_NCMBUTTONUP				非クライアント領域で中央ボタンを解放 
WM_NCMOUSEMOVE				非クライアント領域でマウスが移動 
WM_NCPAINT					非クライアント領域で再ペイントが必要 
WM_NCRBUTTONDBCLK			非クライアント領域で右ボタンをダブルクリック 
WM_NCRBUTTONDOWN			非クライアント領域で右ボタンを押す 
WM_NCRBUTTONUP				非クライアント領域で右ボタンを解放 
WM_NEXTDLGCTL				入力フォーカスを別の子ウインドウに移動 
WM_NEXTMENU					メニューバーまたはシステムメニュー内を左右の矢印キーを使用して移動したとき 
WM_NOTIFY					コントロールにイベントが発生した時またはコントロールがデータを要求している 
WM_NULL						これ自体は何もしないがメッセージを操作する場合に使う 
WM_PAINT					ウインドウのクライアント領域を再描画しなければならない 
WM_PAINTCLIPBOARD			ウインドウのクライアント領域を再描画しなければならない 
WM_PAINTICON				ウインドウのクライアント領域を再描画しなければならない 
WM_PALETTECHANGED			システムパレットが変更された 
WM_PALETTEISCHANGING		システムパレットが別のウインドウによって変更されようとしている 
WM_PASTE					テキストボックス、コンボボックスにおいてクリップボードからコピーするとき送る 
WM_PARENTNOTIFY				あるイベントが子ウインドウで発生したことを親ウインドウに通知する 
WM_POWER					システムがパワー節約状態に入る・出ること通知する 
WM_POWERBROADCAST			パワー管理イベントを検出するために送られてくる 
WM_PRINT					指定のデバイスコンテキストの再描画を要求 
WM_PRINTCLIENT				指定のデバイスコンテキスト内のウィンドウのクライアント領域の描画を要求 
WM_QUERYDRAGICON			アイコンを表示しようとする時にクラスアイコンを持たないウインドウに送る 
WM_QUERYENDSESSION			Windowsを終了する時に全てのウインドウに問合わせる 
WM_QUERYNEWPALETTE			入力フォーカスを受取る時論理パレットで置換えるかどうか問合わせる 
WM_QUERYOPEN				アイコン化されたウインドウが復元する時直前の位置とサイズを要求されているかどうか問合わせる 
WM_QUEUESYNC				ユーザー入力メッセージとフックプロシージャからのメッセージを区別する 
WM_QUIT						アプリケーションが処理する最後のメッセージ 
WM_RBUTTONDBCLK				右のマウスボタンをダブルクリックした 
WM_RBUTTONDOWN				右マウスボタンを押した 
WM_RBUTTONUP				右マウスボタンを解放した 
WM_RENDERALLFORMATS			ディレイドレンダリングしているときクリップボードのオーナーが破棄される前に送られる 
WM_RENDERFORMAT				ディレイドレンダリングをしているデータをアプリケーションが要求したとき 
WM_SETCURSOR				マウスをキャプチャしているウインドウがない時マウスがあるウインドウに送られる 
WM_SETFOCUS					ウインドウが入力フォーカスを得た 
WM_SETFONT					論理フォントを設定する 
WM_SETHOTKEY				ホットキーをウィンドウに関連付ける 
WM_SETICON					アイコンをウインドウに関連付ける 
WM_SETREDRAW				コントロールの再描画操作を制御する 
WM_SETTEXT					インドウ(コントロール)のタイトル・テキストを変更 
WM_SETTINGCHANGE			システム全体に影響する設定変更をしたとき 
WM_SHOWWINDOW				ウインドウが表示・非表示になった 
WM_SIZE						ウインドウのサイズが変更 
WM_SIZECLIPBOARD			クリップボードビューワのクライアント領域のサイズが変更されるとき 
WM_SIZING					ウィンドウのサイズが変更中 
WM_SPOOLERSTATUS			印刷キューに印刷ジョブが追加・削除された 
WM_STYLECHANGED				SetWindowLong関数によってウインドウスタイルを変更した 
WM_STYLECHANGING			SetWindowLong関数によってウインドウスタイルを変更されそう 
WM_SYNCPAINT				別のスレッドで作成されたウィンドウが上に重なったとき 
WM_SYSCHAR					[Alt]キーとの組合せ以外はWM_CHARと同じ 
WM_SYSCOLORCHANGE			システムカラーが変更された 
WM_SYSCOMMAND				システムメニューが操作された 
WM_SYSDEADCHAR				アクセント記号のような別の文字と組合せた文字入力を受け取った 
WM_SYSKEYDOWN				システム文字を意味するキーを押した 
WM_SYSKEYUP					システム文字を意味するキーが解放された 
WM_TIMECHANGE				システムタイマが変更された 
WM_TIMER					SetTimer()で設定した時間を経過 
WM_UNDO						テキストボックスで Undo 操作を実行する 
WM_USER						ユーザーが定義できるメッセージの使用領域を表すだけでこれ自体に意味はない 
WM_USERCHANGED				ログオン・ログオフ時に全てのウィンドウに送られる 
WM_VKEYTOITEM				リストボックスがWM_KEYDOWNに応答したとき 
WM_VSCROLL					ユーザーが垂直スクロールバーを調整した 
WM_VSCROLLCLIPBOARD			クリップボードビューワ内でスクロールイベント発生 
WM_WINDOWPOSCHANGED			etWindowPos()後に位置とサイズを変更したウインドウに送られる 
WM_WINDOWPOSCHANGING		SetWindowPos()で位置とサイズを変更しているウインドウに送られる 
WM_WININICHANGE				WIN.INIが変更された 

/**/


//BOOL CMGRFrame::OnEraseBkgnd(CDC* pDC)
//{
//	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
//
//	return TRUE;
//	//return CExFrame::OnEraseBkgnd(pDC);
//}
