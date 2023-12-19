#if !defined(AFX_RWRGNDOC_H__EFB3EB22_CD2D_4C29_9E31_A991DA09AB1F__INCLUDED_)
#define AFX_RWRGNDOC_H__EFB3EB22_CD2D_4C29_9E31_A991DA09AB1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// RwRGNDoc.h : ヘッダー ファイル
//



/**
CRwRGNDoc クラス

	抽出した領域データファイルを読み込んで rgnData に格納する．
*/




#include  "ExClass.h"
#include  "Branch.h"
#include  "CTRGNio.h"


using namespace jbxl;
using namespace jbxwl;





int  writeRgnFileWithCounter(LPCTSTR dname, RgnData** rgnData, int rgnDataNum);

RgnData*  readRgnFileWithCounter(LPCTSTR fname);





/////////////////////////////////////////////////////////////////////////////
// CRwRGNDoc ドキュメント

class CRwRGNDoc : public CExDocument
{
protected:
	CRwRGNDoc();           // 動的生成に使用されるプロテクト コンストラクタ。
	DECLARE_DYNCREATE(CRwRGNDoc)


// アトリビュート
public:
	RgnData*  rgnData;


// オペレーション
public:
	virtual BOOL	ReadDataFile(LPCTSTR str);
//	virtual BOOL	MakeViewData();
//	virtual CmnHead TranslateData();



// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CRwRGNDoc)
	public:
	virtual void Serialize(CArchive& ar);   // ドキュメント I/O に対してオーバーライドされます。
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~CRwRGNDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// メッセージ マップ関数の生成
protected:
	//{{AFX_MSG(CRwRGNDoc)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_RWRGNDOC_H__EFB3EB22_CD2D_4C29_9E31_A991DA09AB1F__INCLUDED_)
