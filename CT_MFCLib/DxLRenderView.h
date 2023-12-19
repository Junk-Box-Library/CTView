#if !defined(AFX_DXLRENDERVIEW_H__DDE00F5B_3724_453F_8797_C8959EC118EC__INCLUDED_)
#define AFX_DXLRENDERVIEW_H__DDE00F5B_3724_453F_8797_C8959EC118EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DxLRenderView.h : ヘッダー ファイル
//


/**
CDxLRenderView クラス

	CDxVTXBaseViewクラスからの継承クラス
	
	  		
	対応する Document の msGraph (pDoc->msGraph) からデータを受け取り
	（cmnHeadではないので注意），ライン描画を行い，
	DirectXの機能を使って 3D表示する．


  　3D表示する場合の色は，ボクセルの輝度値(16bit)に対して，ARGB 4bit
    と解釈する．輝度値(16bit)の設定には
		sWord  color2Word(int r, int g, int b, int a=0)　　　(要 #include "ExClass.h")
	を使用すると良い．因みに逆の関数は
		D3DCOLOR  Dx9Word2Color(sWord pp)　　                (要 #include "Dx9.h") 
*/


#include  "DxBaseClass.h"
#include  "ProgressBarDLG.h"

#include  "Branch.h"
#include  "DxSRenderView.h"


using namespace jbxl;
using namespace jbxwl;



typedef struct 
{
	float     x,  y,  z;	// D3DXVECTOR3	頂点座標
	float    ps;			// ピクセルサイズ
	D3DCOLOR color;			// DWORD		ディフューズ色
} SVERTEXL;



#define	 FVF_SVERTEXL (D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE)




/////////////////////////////////////////////////////////////////////////////
// CDxLRenderView ビュー

class CDxLRenderView : public CDxVTXBaseView
{
protected:
	CDxLRenderView();           // 動的生成に使用されるプロテクト コンストラクタ
	DECLARE_DYNCREATE(CDxLRenderView)


// アトリビュート
public:
	RBound<int>	Rbound;
	CProgressBarDLG*  pCounter;
	RgnData*	pRgnData;
	BrcData*	brcData;

	int		xsize;
	int		ysize;
	int		zsize;
	int		msize;

	int		datano;


// オペレーション
public:
	virtual void    SetState();
	virtual BOOL	PrepareVB();

	virtual void	ExecRender();
	virtual void	ClearObject();
	virtual void	InitObject();

	virtual void	ExRotationAngle();
	virtual void	ExMouseWheel();

	int		GetValidVoxelNum();






// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。

	//{{AFX_VIRTUAL(CDxLRenderView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC) {}
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	virtual ~CDxLRenderView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(CDxLRenderView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};








/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_DXLRENDERVIEW_H__DDE00F5B_3724_453F_8797_C8959EC118EC__INCLUDED_)
