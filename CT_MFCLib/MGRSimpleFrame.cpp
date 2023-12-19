// MGRSimpleFrame.cpp : 実装ファイル
//

#include "stdafx.h"
#include "CT_MFCLib.h"
#include "MGRSimpleFrame.h"


using namespace jbxl;
using namespace jbxwl;




// CMGRSimpleFrame

IMPLEMENT_DYNCREATE(CMGRSimpleFrame, CExFrame)

CMGRSimpleFrame::CMGRSimpleFrame()
{

}



CMGRSimpleFrame::~CMGRSimpleFrame()
{
}




BEGIN_MESSAGE_MAP(CMGRSimpleFrame, CExFrame)
	ON_WM_CREATE()
END_MESSAGE_MAP()




// CMGRSimpleFrame メッセージ ハンドラ

int CMGRSimpleFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CExFrame::OnCreate(lpCreateStruct) == -1) return -1;

	// TODO:  ここに特定な作成コードを追加してください。

	return 0;
}
