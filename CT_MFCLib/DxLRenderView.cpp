// DxLRenderView.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"

#include "DxLRenderView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace jbxl;
using namespace jbxwl;



/////////////////////////////////////////////////////////////////////////////
// CDxLRenderView

IMPLEMENT_DYNCREATE(CDxLRenderView, CExView)


CDxLRenderView::CDxLRenderView()
{
	cnstXYRate = true;
//	rotation = true;

	pCounter  = NULL;
	pRgnData  = NULL;
	brcData	  = NULL;
}



CDxLRenderView::~CDxLRenderView()
{
	delete(brcData);
}



BEGIN_MESSAGE_MAP(CDxLRenderView, CDxVTXBaseView)
	//{{AFX_MSG_MAP(CDxLRenderView)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CDxLRenderView 描画
/*
void CDxLRenderView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: この位置に描画用のコードを追加してください
}
*/



/////////////////////////////////////////////////////////////////////////////
// CDxLRenderView 診断

#ifdef _DEBUG
void CDxLRenderView::AssertValid() const
{
	CDxVTXBaseView::AssertValid();
}



void CDxLRenderView::Dump(CDumpContext& dc) const
{
	CDxVTXBaseView::Dump(dc);
}
#endif //_DEBUG




/////////////////////////////////////////////////////////////////////////////
// CDxLRenderView メッセージ ハンドラ

void  CDxLRenderView::OnInitialUpdate() 
{
//	CDxVTXBaseView::OnInitialUpdate();

	pRgnData = (RgnData*)(pDoc->anyData);	
	if (pRgnData==NULL)		return;
	if (pRgnData->rs==NULL)	return;
	brcData = pRgnData->make_branch();
	if (brcData==NULL) return;

	datano = 0;
	for (int i=0; i<brcData->no; i++) {
		datano += brcData->br[i].vn - 1;
	}


	Rbound = brcData->rb;
	center = D3DXVECTOR3((Rbound.xmax + Rbound.xmin)/2.f,
						 (Rbound.ymax + Rbound.ymin)/2.f,
						 (Rbound.zmax + Rbound.zmin)/2.f);
	xsize = Rbound.xmax - Rbound.xmin + 1;
	ysize = Rbound.ymax - Rbound.ymin + 1;
	zsize = Rbound.zmax - Rbound.zmin + 1;
	msize = Max(xsize, ysize);
	msize = Max(msize, zsize);
	sizeFac = Min(sizeFac, 1.0f/msize);

	BOOL rslt = InitDevice(msize, msize);
	if (!rslt) {
		CString mesg;
		mesg.LoadString(IDS_STR_FAIL_GET_DX9DEV);
		MessageBox(_T("CDxLRenderView::OnInitialUpdate\n\n")+mesg); 
		pFrame->doneErrorMessage = true;
		return;
	}

	if (!PrepareVB()) return;
	SetState();

	SetWindowSize(500, 500);
	hasViewData = TRUE;
	return;
}



void  CDxLRenderView::SetState()
{
	// Z バッファ有効化
	lpD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	// アルファブレンディング無効化
	lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// ポリゴンの両面を描画
	lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// フィルタ設定
	lpD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	// レンダリング時の色の計算方法の設定
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	// テクスチャの色を使用
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	// 頂点の色を使用
	lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	// ライト
	lpD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// プロジェクション行列を設定: 視界 60度，近くの限界 0.1，遠くの限界 10*msize*sizeFac
	D3DXMatrixPerspectiveFovLH(&matProj, 60.0f*(float)PI/180.0f, 1.0f, 0.0f, (float)(10*msize*sizeFac));
	lpD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);


	// カメラの位置は mWheelAc で決める．
	if (mWheelAc==0.0) mWheelAc = 1.5f*msize*sizeFac;
	ExMouseWheel();

	// 頂点フォーマット設定
	lpD3DDevice->SetFVF(FVF_SVERTEXL);
	
	D3DXMatrixTranslation(&matTrans, -(float)(center.x*sizeFac), -(float)(center.y*sizeFac), (float)(center.z*sizeFac));
	lpD3DDevice->SetTransform(D3DTS_WORLD, &matTrans);
}



//
//  
// 
void   CDxLRenderView::ExMouseWheel()
{
	if (Xabs(mWheelAc)<1.) mWheelAc = (float)Sign(mWheelAc);

	// カメラの制御:　カメラの位置，物体の位置，カメラの上方向の傾き()
	D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(0.0f, (float)mWheelAc, 0.0f), &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,1));
	lpD3DDevice->SetTransform(D3DTS_VIEW, &matView);

	double pointsize = sizeFac/Xabs(mWheelAc)*1000.0;
	lpD3DDevice->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&pointsize));
}




	void    CDxLRenderView::ExecRender()
{
	HRESULT hr;

	// 画面のクリア
	hr = lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,96), 1.0f, 0);
	if (FAILED(hr)) { DEBUG_ERROR("CBREPView::ExecRender: 画面クリア失敗"); return;}


	// シーンの描画開始
	hr = lpD3DDevice->BeginScene();	
	if (FAILED(hr)) { DEBUG_ERROR("CBREPView::ExecRender: シーンの開始失敗"); return;}
	
	hr = lpD3DDevice->SetStreamSource(0, lb, 0, sizeof(SVERTEXL));
	if (FAILED(hr)) { DEBUG_ERROR("CBREPView::ExecRender: ストリームの設定失敗 WIRE"); lpD3DDevice->EndScene(); return;}
	
	hr = Dx9DrawPrimitive(lpD3DDevice, D3DPT_LINELIST, 2, datano);
	if (FAILED(hr)) { DEBUG_ERROR("CBREPView::ExecRender: プリミティブ描画失敗 WIRE"); lpD3DDevice->EndScene(); return;}

	hr = lpD3DDevice->EndScene();	// シーンの描画終了
	if (FAILED(hr)) { DEBUG_ERROR("CBREPView::ExecRender: シーンの終了失敗"); return;}
	// シーンの描画終了


	// 画面表示
	hr = lpD3DDevice->Present(NULL, NULL, NULL, NULL);
	if (FAILED(hr)) {	// デバイスチェック
//		DEBUG_ERROR("CBREPView::ExecRender: Restting Device ...."); 
		hr = ResetDx9Device(lpD3DDevice, &d3dParam, this);
		if (FAILED(hr)) {
			CString mesg;
			mesg.LoadString(IDS_STR_FAIL_RESTR_DX9DEV);
			MessageBox(_T("CBREPView::ExecRender\n\n")+mesg);
			GetParentFrame()->DestroyWindow();
		}
	}
}



// 頂点バッファの準備
BOOL   CDxLRenderView::PrepareVB()
{
	if (datano<=0) {
		CString mesg, noti;
		mesg.LoadString(IDS_STR_INFO_VERTEX_ZERO);
		noti.LoadString(IDS_STR_INFO);
		MessageBox(_T("CDxVRenderView::PrepareVB\n\n")+mesg, noti);
		pFrame->doneErrorMessage = true;
		return  FALSE;
	}

	lpD3DDevice->CreateVertexBuffer(sizeof(SVERTEXL)*datano*2, 0, FVF_SVERTEXL, D3DPOOL_MANAGED, &lb, NULL);
	if (lb==NULL) {			// メモリ確保失敗
		ReleaseVB();
		CString mesg, noti;
		mesg.LoadString(IDS_STR_FAIL_GET_VERTEX_BUF);
		noti.LoadString(IDS_STR_ERROR);
		MessageBox (_T("CDxLRenderView::PrepareVB\n\n")+mesg, noti);
		pFrame->doneErrorMessage = true;
 		return  FALSE;
	}

	SVERTEXL *lv;
	lb->Lock(0, 0, (void **)&lv, 0);

	int cnt = 0;
	for (int i=0; i<brcData->no; i++) {
		Vector_List::iterator ivect, ovect;
		ivect = ovect = brcData->br[i].lv->begin();
		ivect++;
		
		while(ivect!=brcData->br[i].lv->end()) {

			lv[cnt].x   =  (float)((*ovect)->x*sizeFac);
			lv[cnt].y   =  (float)((*ovect)->y*sizeFac);
			lv[cnt].z   = -(float)((*ovect)->z*sizeFac);
			
			lv[cnt+1].x =  (float)((*ivect)->x*sizeFac);
			lv[cnt+1].y =  (float)((*ivect)->y*sizeFac);
			lv[cnt+1].z = -(float)((*ivect)->z*sizeFac);
			
			//lv[cnt].color   = Dx9Word2Color((sWord)get_color_from_bn(brcData->br[i].gn));
			//lv[cnt+1].color = Dx9Word2Color((sWord)get_color_from_bn(brcData->br[i].gn));
			if (brcData->br[i].at==BRC_LOOP) {
				lv[cnt].color   = D3DCOLOR_RGBA(255, 0, 0, 127);
				lv[cnt+1].color = D3DCOLOR_RGBA(255, 0, 0, 127);
			}
			else if (brcData->br[i].at==BRC_INTER) {
				lv[cnt].color   = D3DCOLOR_RGBA(0, 255, 0, 127);
				lv[cnt+1].color = D3DCOLOR_RGBA(0, 255, 0, 127);
			}
			else {
				lv[cnt].color   = D3DCOLOR_RGBA(255, 255, 255, 127);
				lv[cnt+1].color = D3DCOLOR_RGBA(255, 255, 255, 127);
			}

			cnt += 2;
			ovect = ivect;
			ivect++;
		}
	}

	lb->Unlock();

	if (cnt!=datano*2) {
		DEBUG_ERROR("CDxLRenderView::PrepareVB: 頂点数が合っていません %d != %d", cnt, datano*2);//IDS_STR_ERR_VERTEX_NUM
	}

	return TRUE;
}



//
//  (angleX, angleY -> angle?, angle?)
// 
void   CDxLRenderView::ExRotationAngle()
{
	angleZ = -angleX;
	angleX = -angleY;
	angleY =  0.0;
}



void   CDxLRenderView::ClearObject()
{
	hasViewData = FALSE;
//	TimerStop();
}



void   CDxLRenderView::InitObject()
{
	SetState();
//	TimerStart();
	hasViewData = TRUE;
}




