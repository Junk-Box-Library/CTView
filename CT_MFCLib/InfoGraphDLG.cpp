// InfoGraphDLG.cpp : 実装ファイル
//

#include "stdafx.h"

#include "tools++.h"
#include "InfoGraphDLG.h"
#include "WinBaseResource.h"


using namespace jbxl;
//using namespace jbxwl;



// CInfoGraphDLG ダイアログ

IMPLEMENT_DYNAMIC(CInfoGraphDLG, CDialog)

CInfoGraphDLG::CInfoGraphDLG(CWnd* pParent /*=NULL*/)
	: CDialog(CInfoGraphDLG::IDD, pParent)
{
	format  = _T("Unknown");
	xsize   = 0;
	ysize   = 0;
	zsize   = 0;
	depth   = 0;
	max     = 0;
	min     = 0;
	base    = 0;
	tmpbase = 0;
	rzxy    = 0.0;

	rbound.init();

	fmtSBox = NULL;
	xszSBox = NULL;
	yszSBox = NULL;
	zszSBox = NULL;
	dptSBox = NULL;
	maxSBox = NULL;
	minSBox = NULL;
	basSBox = NULL;
	tmpSBox = NULL;

	xmnSBox = NULL;
	xmxSBox = NULL;
	ymnSBox = NULL;
	ymxSBox = NULL;
	zmnSBox = NULL;
	zmxSBox = NULL;

	zxySBox = NULL;
}







CInfoGraphDLG::~CInfoGraphDLG()
{
}

void CInfoGraphDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInfoGraphDLG, CDialog)
END_MESSAGE_MAP()


// CInfoGraphDLG メッセージ ハンドラ



BOOL   CInfoGraphDLG::OnInitDialog(void)
{
	fmtSBox = (CStatic*)GetDlgItem(IDC_INFO_FORMAT);
	xszSBox = (CStatic*)GetDlgItem(IDC_INFO_XSIZE);
	yszSBox = (CStatic*)GetDlgItem(IDC_INFO_YSIZE);
	zszSBox = (CStatic*)GetDlgItem(IDC_INFO_ZSIZE);
	dptSBox = (CStatic*)GetDlgItem(IDC_INFO_DEPTH);
	maxSBox = (CStatic*)GetDlgItem(IDC_INFO_MAX);
	minSBox = (CStatic*)GetDlgItem(IDC_INFO_MIN);
	basSBox = (CStatic*)GetDlgItem(IDC_INFO_BASE);
	tmpSBox = (CStatic*)GetDlgItem(IDC_INFO_TEMPBASE);
	xmnSBox = (CStatic*)GetDlgItem(IDC_INFO_XMIN);
	xmxSBox = (CStatic*)GetDlgItem(IDC_INFO_XMAX);
	ymnSBox = (CStatic*)GetDlgItem(IDC_INFO_YMIN);
	ymxSBox = (CStatic*)GetDlgItem(IDC_INFO_YMAX);
	zmnSBox = (CStatic*)GetDlgItem(IDC_INFO_ZMIN);
	zmxSBox = (CStatic*)GetDlgItem(IDC_INFO_ZMAX);	
	zxySBox = (CStatic*)GetDlgItem(IDC_INFO_RZXY);

	setToWindowItem();

	return TRUE;
}






void  CInfoGraphDLG::setVal(CmnHead hd)
{
	TCHAR buf[LMNAME];

	sWord kind = hd.kind & 0x00ff;
	// see JunkBox_Lib++/gheader.h
	if      (kind==UN_KNOWN_DATA)	format = _T(UN_KNOWN_DATA_STR);
	else if (kind==PROC_DATA)		format = _T(PROC_DATA_STR);
	else if (kind==CREATE_DATA)		format = _T(CREATE_DATA_STR);
	else if (kind==USERSET_DATA)	format = _T(USERSET_DATA_STR);
	else if (kind==DICOM_DATA)		format = _T(DICOM_DATA_STR);
	else if (kind==RAS_DATA)		format = _T(RAS_DATA_STR);
	else if (kind==CT_DATA)			format = _T(CT_DATA_STR);
	else if (kind==MOON_DATA)		format = _T(MOON_DATA_STR);
	else if (kind==CT_3DM)			format = _T(CT_3DM_STR);
	else if (kind==JPEG_RGB_DATA)   format = _T(JPEG_RGB_DATA_STR);
	else if (kind==JPEG_MONO_DATA)  format = _T(JPEG_MONO_DATA_STR);
	else if (kind==PNG_DATA)		format = _T(PNG_DATA_STR);
	else if (kind==TIFF_DATA)		format = _T(TIFF_DATA_STR);
	else if (kind==CT_3D_VOL)		format = _T(CT_3D_VOL_STR);
	else {
		sntprintf(buf, 10, _T("(%04x)"), kind);
		format = _T("ERROR ");
		format+= buf;
	}

	xsize   = hd.xsize;
	ysize   = hd.ysize;
	zsize	= hd.zsize;
	depth   = hd.depth;
	tmpbase = TempBase;

	if (hd.bsize>0 && hd.buf!=NULL) {
		CTHead* chd = (CTHead*)(hd.buf);
		rbound.xmax = chd->cutright;
		rbound.xmin = chd->cutleft;
		rbound.ymax = chd->cutdown;
		rbound.ymin = chd->cutup;
		rbound.zmax = chd->ctmax;
		rbound.zmin = chd->ctmin;
//		if (chd->anydata[1]==RZXY_RATE) {
//			rzxy = (float)chd->anydata[0]/RZXY_RATE;
//		}
//		if (chd->anydata[2]!=0) base = chd->anydaya[2];
	}
}





void  CInfoGraphDLG::setVal(MSGraph<sWord> vp)
{
	if (xsize==0) xsize = vp.xs;
	if (ysize==0) ysize = vp.ys;
	if (zsize==0) zsize = vp.zs;
	if (depth==0 && vp.gp!=NULL) depth = sizeof(vp.gp[0])*8;
	max  = vp.max;
	min  = vp.min;
	rzxy = vp.RZxy;
	base = vp.base;
}





void  CInfoGraphDLG::setToWindowItem()
{
	TCHAR buf[LMNAME];
//	memset(buf, 0, LMNAME);

	fmtSBox->SetWindowText(format);

	sntprintf(buf, LMNAME, _T("%d"), xsize);
	xszSBox->SetWindowText(buf);

	sntprintf(buf, LMNAME, _T("%d"), ysize);
	yszSBox->SetWindowText(buf);

	sntprintf(buf, LMNAME, _T("%d"), zsize);
	zszSBox->SetWindowText(buf);

	sntprintf(buf, LMNAME, _T("%d"), depth);
	dptSBox->SetWindowText(buf);

	sntprintf(buf, LMNAME, _T("%d (%d)"), min, min-base-tmpbase);
	minSBox->SetWindowText(buf);

	sntprintf(buf, LMNAME, _T("%d (%d)"), max, max-base-tmpbase);
	maxSBox->SetWindowText(buf);

	sntprintf(buf, LMNAME, _T("%d"), base);
	basSBox->SetWindowText(buf);

	sntprintf(buf, LMNAME, _T("%d"), tmpbase);
	tmpSBox->SetWindowText(buf);

	sntprintf(buf, LMNAME, _T("%d (/%d)"), (int)(rzxy*RZXY_RATE), RZXY_RATE);
	zxySBox->SetWindowText(buf);

	sntprintf(buf, LMNAME, _T("%d"), rbound.xmin);
	xmnSBox->SetWindowText(buf);

	sntprintf(buf, LMNAME, _T("%d"), rbound.xmax);
	xmxSBox->SetWindowText(buf);

	sntprintf(buf, LMNAME, _T("%d"), rbound.ymin);
	ymnSBox->SetWindowText(buf);

	sntprintf(buf, LMNAME, _T("%d"), rbound.ymax);
	ymxSBox->SetWindowText(buf);

	sntprintf(buf, LMNAME, _T("%d"), rbound.zmin);
	zmnSBox->SetWindowText(buf);

	sntprintf(buf, LMNAME, _T("%d"), rbound.zmax);
	zmxSBox->SetWindowText(buf);
}
