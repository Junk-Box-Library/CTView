#pragma once

#include "resource.h"
#include "Gdata.h"

using namespace jbxl;
//using namespace jbxwl;

// CInfoGraphDLG ダイアログ

class CInfoGraphDLG : public CDialog
{
	DECLARE_DYNAMIC(CInfoGraphDLG)

public:
	CString     format;

	int			xsize;
	int			ysize;
	int			zsize;
	int			depth;
	int			max;
	int			min;
	int			base;
	int			tmpbase;
	double		rzxy;

	RBound<int> rbound;

	CStatic*	fmtSBox;
	CStatic*	xszSBox;
	CStatic*	yszSBox;
	CStatic*	zszSBox;
	CStatic*	dptSBox;
	CStatic*	maxSBox;
	CStatic*	minSBox;
	CStatic*	basSBox;
	CStatic*	tmpSBox;

	CStatic*	xmnSBox;
	CStatic*	xmxSBox;
	CStatic*	ymnSBox;
	CStatic*	ymxSBox;
	CStatic*	zmnSBox;
	CStatic*	zmxSBox;

	CStatic*	zxySBox;

public:
	CInfoGraphDLG(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CInfoGraphDLG();

	virtual BOOL  OnInitDialog();
	void    setVal(MSGraph<sWord> vp);
	void    setVal(CmnHead hd);
	void    setToWindowItem();

// ダイアログ データIDD_INFOBOX
	enum { IDD = IDD_INFOBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
};
