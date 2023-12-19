// CTView.h : CTVIEW アプリケーションのメイン ヘッダー ファイル
//

#if !defined(AFX_CTVIEW_H__8E4C3ED0_0F06_412A_A67A_1A6C5F3E7577__INCLUDED_)
#define AFX_CTVIEW_H__8E4C3ED0_0F06_412A_A67A_1A6C5F3E7577__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif




/**




*/


#include "resource.h"       // メイン シンボル


#include  "Branch.h"

#include  "ExClass.h"
#include  "CTViewTool.h"
#include  "MainFrm.h"
#include  "InfoGraphDLG.h"


using namespace jbxl;
using namespace jbxwl;



#define   RGNDATA_MAXNUM	100

#define   LDIR_PATH			256
#define   MODULES_DIR		"plugins"
#define   DLL_PROC_RANGE	100			// 読み込み可能な DLLの数



/////////////////////////////////////////////////////////////////////////////
// CCTViewApp:
// このクラスの動作の定義に関しては CTView.cpp ファイルを参照してください。
//

class CCTViewApp : public CWinApp, public CAppCallBack
{
public:
	CMainFrame*		pMainFrame;

	ExMSGraph<sWord> msGraph;		// メモリ管理付，pDocTemplMCT/MGR/SMGR/TMGR/TNで使用．読み込みデータ用．多重使用禁止．
	ExMSGraph<sWord> msGraphVOL;	// メモリ管理付，pDocTemplTVOL/SVOLで使用．ボリュームデータ用．多重使用禁止．
	ExMSGraph<sWord> msGraphRGN;	// メモリ管理付，pDocTemplRGNで使用．ボリュームデータ用．多重使用禁止．未使用．
	ExCmnHead		cmnHeadTBN;		// メモリ管理付，pDocTemplTBNで使用．サムネイル表示用．多重使用禁止．
	ExCmnHead		cmnHeadMCT;		// メモリ管理付，pDocTemplMCTで使用．マルチCT表示用．多重使用禁止．

	int				msGraph_ref;	// msGraph    のメモリ管理用参照変数
	int				msGraphVOL_ref;	// msGraphVOL のメモリ管理用参照変数
	int				msGraphRGN_ref;	// msGraphRGN のメモリ管理用参照変数．未使用．
	int				cmnHeadTBN_ref;	// cmnHeadTBN のメモリ管理用参照変数
	int				cmnHeadMCT_ref;	// cmnHeadMCT のメモリ管理用参照変数

	int				tempBase;		// 一時的な基底値
	CmnHead			infHead;		// 読み込んだ画像のヘッダ情報

	CString			Title;
	CString			preTitle;
	CString			pstTitle;


	// 領域データの格納場所
	int				rgnDataNum;
	RgnData*		rgnData[RGNDATA_MAXNUM];


	BOOL			hasMSGraph;		// msGraph    はメモリの管理を行なっているか．
	BOOL			hasMSGraphVOL;	// msGraphVOL はメモリの管理を行なっているか．
	BOOL			hasMSGraphRGN;	// msGraphRGN はメモリの管理を行なっているか．
	BOOL			hasCmnHeadTBN;	// cmnHeadTBN はメモリの管理を行なっているか．
	BOOL			hasCmnHeadMCT;	// cmnHeadMCT はメモリの管理を行なっているか．

	BOOL			volumeColorMode;
	BOOL			centerLineMode;
	BOOL			forceContrastMode;

	CInfoGraphDLG*	infoGraphDlg;

	CMultiDocTemplate*  pDocTempl2D;
	CMultiDocTemplate*  pDocTemplMGR;
	CMultiDocTemplate*  pDocTemplTMGR;
	CMultiDocTemplate*  pDocTemplSMGR;

	CMultiDocTemplate*  pDocTemplMCT;

	CMultiDocTemplate*  pDocTemplSVOL;
	CMultiDocTemplate*  pDocTemplTVOL;
	
	CMultiDocTemplate*  pDocTemplRGN;
	CMultiDocTemplate*  pDocTemplTBN;


	// DLLモジュールの管理
	DLLFunctionTBL	dllFunctionTBL;
	TCHAR			modulesDir[LDIR_PATH];


public:
	CCTViewApp();
	~CCTViewApp();

	CExFrame*	open2DFile (LPCTSTR fname);
	CExFrame*	openMGRFile(LPCTSTR fname);
	CExFrame*	openMCTFile(LPCTSTR fname, BOOL multi=FALSE, int vmode=VIEW_ZDIR);
	CExFrame*	openTBNFile(LPCTSTR fname, BOOL multi=FALSE);
	CExFrame*	openVOLFile(LPCTSTR fname, BOOL multi=FALSE);
	CExFrame*	openRGNFile(LPCTSTR fname);

	CExFrame*	viewMCTXYZ(int vmode=0, LPCTSTR title=NULL);
	CExFrame*	showTempSVOL(MSGraph<sWord>* ms, LPCTSTR title=NULL);
	
	virtual		CExFrame*  TemplateCallBack(CExFrame* pfrm, int mode, LPCTSTR title=NULL);
	virtual		CMultiDocTemplate*  GetMultiDocTemplate(int mode);

	bool		deleteMData(bool nocnfrm=false);
	bool		deleteVData(bool nocnfrm=false); 
	bool		deleteRData(bool nocnfrm=false); 


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CCTViewApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// インプリメンテーション
	//{{AFX_MSG(CCTViewApp)
	afx_msg void OnAppAbout();
	afx_msg void OnInfoGraph();
	afx_msg void OnFreeDLL();
	afx_msg void OnReadDLL();
	afx_msg void OnDelRData();
	afx_msg void OnDelVData();
	afx_msg void OnDelMData();
	afx_msg void OnToolSet();
	afx_msg void OnToolShwRefn();
	afx_msg void OnToolHeader();
	afx_msg void OnFileSaveVOL();
	afx_msg void OnFileSaveFL();
	afx_msg void OnFileSaveRGSL();
	afx_msg void OnFileOpenVOL();
	afx_msg void OnMCT();
	afx_msg void OnFileTn();
	afx_msg void OnViewOpenVOL();
	afx_msg void OnViewTn();
	afx_msg void OnViewMCT();
	afx_msg void OnViewMCTX();
	afx_msg void OnViewMCTY();
	afx_msg void OnFileOpenRGSL();
	afx_msg void OnFileOpen();
	afx_msg void OnDLLFunction(UINT id);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};






/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。


#endif // !defined(AFX_CTVIEW_H__8E4C3ED0_0F06_412A_A67A_1A6C5F3E7577__INCLUDED_)
