
#include "Tree.h"
#include "Threshold.h"


/**
	再帰的領域探索プログラム

  */


int			Tree_No    = 0;
int			Region_No  = 0;
int			Branch_No  = 0;
bool		Inter_Path   = true;
bool		Sphere_Start = false;

int			SavedRegionNum = 0;
bool		isCanceled = false;


// グローバル空間へのポインタ
MSGraph<sWord>*		GLMPgd = NULL;	
MSGraph<sWord>*		GLMPgx = NULL;

RgnStatRingBuffer*  pSearchRing = new RgnStatRingBuffer(500);	// 枝探索用
RgnStatRingBuffer*  pInterpRing = new RgnStatRingBuffer(500);	// 枝間の補間用


/**
MSGraph<sWord>*  RecursiveRegionSearch(MSGraph<sWord> gd, MSGraph<sWord> gx, int sm, FILE* fp)

  	機能：リングバッファ（Search_Ring_Buffer）に保存された領域データを元に再帰探索を行う．
		　リングバッファ（Search_Ring_Buffer）に保存するには add_search_ring() を使う．

	引数：gd : 探索空間
		  gx : 抽出空間．gdと同じ大きさのメモリが確保されていること．
		  sm : 探索モード
		  fp :
*/
MSGraph<sWord>*  RecursiveRegionSearch(MSGraph<sWord> gd, MSGraph<sWord> gx, int sm, FILE* fp)
{
	int  nn;
    MSGraph<sWord>* vp = new MSGraph<sWord>();

    nn  = start_search_tree(gd, gx, 1.0, sm, fp);
	*vp = cut_object_MSGraph(gx, gx.zero+1, MAXCT);

	DEBUG_MODE print_message("RecursiveResionSearch: %d の枝を見つけた\n", nn);
	
	return vp;
}



/**
MSGraph<sWord>  look_forward(MSGraph<sWord> gd, RgnStat ns, double rs, int vm, int nz)

	機能：前方(ex, ey, ez)方向を観測．観測面を gd.zero で縁取りしたデータを返す．
		//　ただし，rs=0 の場合は，(px, py, pz)方向を観測．

	引数：gd :
		  ns : 親領域の状態	
		  rs :
		  vm :
		  nz : ３Ｄ観測を行う場合の Z方向の深さ

*/
MSGraph<sWord>  look_forward(MSGraph<sWord> gd, RgnStat ns, double rs, int vm, int nz)
{
    MSGraph<sWord>  vp, wp;

    if      (vm==FTMODE) vp = fviewport(gd, ns, rs);				// ノーマルな平面モード（実数による線形補間）
    else if (vm==WDMODE) vp = wviewport(gd, ns, rs);				// ノーマルな平面モード（sWord型）
    else if (vm==SPMODE) vp = pviewport(gd, ns, rs, PI);			// 球面観測モード
    else if (vm==XVMODE) vp = xviewport(gd, ns, rs, FTMODE, nz);	// ３Ｄ観測モード 
    else if (vm> XVMODE) vp = xviewport(gd, ns, rs, vm-XVMODE, nz);	// 拡張３Ｄ観測モード

	if (FilterMode>0) {
        wp = xfilter(ns, vp);
		free(vp.gp);
		vp = wp;
    }
    set_around_space(vp);

    return vp;
}



/**
MSGraph<sWord>  look_me(MSGraph<sWord> gd, RgnStat ns)

	機能：前方(px, py, pz)方向を観測．観測面を gd.zero で縁取りしたデータを返す．

	引数：gd :
		  ns : 親領域の状態	
		  rs :
		  vm :
		  nz : ３Ｄ観測を行う場合の Z方向の深さ

*/
MSGraph<sWord>  look_me(MSGraph<sWord> gd, RgnStat ns, int vm)
{
    MSGraph<sWord>  vp, wp;
	Vector<double> vt;

	vt.set(ns.ex, ns.ey, ns.ez);
	ns.ex = ns.px;
	ns.ey = ns.py;
	ns.ez = ns.pz;

    if      (vm==FTMODE) vp = fviewport(gd, ns, 0.0);				// ノーマルな平面モード（実数による線形補間）
    else if (vm==WDMODE) vp = wviewport(gd, ns, 0.0);				// ノーマルな平面モード（sWord型）
    else {
		DEBUG_MODE print_message("LOOK_ME: 知らないモード %d\n", vm);
		return vp;
	}
	
	ns.ex = vt.x;
	ns.ey = vt.y;
	ns.ez = vt.z;

	if (FilterMode>0) {
        wp = xfilter(ns, vp);
		free(vp.gp);
		vp = wp;
    }
    set_around_space(vp);

    return vp;
}



/**
int   pre_scan_top_plane(MSGraph<sWord> gd, RgnStat ns, RgnStat** gs, MSGraph<sWord>** vr, int sm)

	機能：Z=0 の面から OBJECTS, BRONCHUSの探索開始領域を探す．


	戻値： >=0  領域の数
			-1	 メモリエラー
			-2   モードエラー

*/
int   pre_scan_top_plane(MSGraph<sWord> gd, RgnStat ns, RgnStat** gs, MSGraph<sWord>** vr, int sm)
{
    int   i, kk, cm;
    MSGraph<sWord>  vp;

//    DEBUG_MODE print_message("start pre_scan_top_plane.\n");

	vp.set(gd.xs, gd.ys, 1, gd.zero);
	if (vp.isNull()) return -1;
	for (i=0; i<vp.xs*vp.ys; i++) vp.gp[i] = gd.gp[i];		// トップ面(Z=0)だけコピー
   	set_around_space(vp);

	if      (Xabs(sm)==BRONCHUS) cm = PRE_SCAN_TRACHEA;		// cm: チェックモード
	else if (Xabs(sm)==OBJECTS)  cm = PRE_SCAN_OBJECTS;
	else return -2;

    kk = get_region(ns, vp, gs, vr, 0.0, sm, cm); 

    for(i=0; i<kk; i++) {
		gs[i]->xx = (double)gs[i]->sx + gs[i]->wx;
		gs[i]->yy = (double)gs[i]->sy + gs[i]->wy;
		gs[i]->zz = 0.0f;
    }

	vp.free();
    return kk;
}



/**
int   get_region(RgnStat ns, MSGraph<sWord> xp, RgnStat** gs, MSGraph<sWord>** vr, int sm, int cm)

	機能：観測面 xp から選択により，子領域を取り出す．

		  取り出す領域は
			BRONCHUS: 閾値(mm)以下の領域．	ml〜mm
			BLOOD:	  閾値(mm)以上の領域．	mm〜mh
			OBJECTS:  探索閾値間の領域．    ml〜mh

	引数：
		ns: 親領域
		xp: 観測面
		sm: 探索モード（BRONCHUS, BLOOD, OBJECTS）
		cm: チェックモード

	戻値：
		int 子領域の数
		**gs: 子領域の情報
		**vr: 子領域イメージ

*/
int   get_region(RgnStat ns, MSGraph<sWord> xp, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm, int cm)
{
    int   kk, nn;
    MSGraph<sWord>  vp;

//	DEBUG_MODE print_message("start get_region\n");    
    vp = pre_cut_region(ns, xp, sm);				// 領域の輝度値を閾値(ns.mm)に変換した画像を返す．
    nn = cut_region(ns, xp, vp, gs, vr);//, sm);	// xp とvp から子領域の候補 gs, vr を抜き出す．
   	vp.free();
    if (nn<=0) return nn;
	
	for (int i=0; i<nn; i++) get_world_center(ns, gs[i], rs);
    nn = rsorting(gs, vr, nn);
    kk = chk_region_status(ns, gs, nn, sm, cm);
    kk = nsorting(gs, vr, nn);

    return kk;
}



/**
int   get_region_paint(RgnStat ns, MSGraph<sWord> xp, RgnStat** gs, MSGraph<sWord>** vr, int sm, int cm, int xx, int yy)

	機能：観測面 xp から塗りつぶしにより，子領域を取り出す．

		  取り出す領域は
			BRONCHUS: 閾値(mm)以下の領域．	ml〜mm
			BLOOD:	  閾値(mm)以上の領域．	mm〜mh
			OBJECTS:  探索閾値間の領域．    ml〜mh

	引数：
		ns: 親領域
		xp: 観測面
		sm: 探索モード（BRONCHUS, BLOOD, OBJECTS）
		cm: チェックモード

	戻値：
		int 子領域の数（１以下）
		**gs: 子領域の情報
		**vr: 子領域イメージ
*/
int   get_region_paint(RgnStat ns, MSGraph<sWord> xp, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm, int cm, int xx, int yy)
{
    int   kk, nn;
    MSGraph<sWord>  vp;

	vp = pre_paint_region(ns, xp, sm, xx, yy);	// 領域の指定された部分を閾値(ns.mm)で塗りつぶした画像を返す．その他は zero
    nn = cut_region(ns, xp, vp, gs, vr);		// xp とvp から子領域の候補を抜き出す．
   	vp.free();
   	if (nn<=0) return nn;

	for (int i=0; i<nn; i++) get_world_center(ns, gs[i], rs);
	nn = rsorting(gs, vr, nn);
	kk = chk_region_status(ns, gs, nn, sm, cm);
	kk = nsorting(gs, vr, nn);
	if (kk>1) {
		DEBUG_MODE print_message("GET_REGION_PAINT: Warnning：領域数が１より大きい!?\n");
		free_data(gs, vr, 1, kk);
	}

    return kk;
}



int   start_search_tree(MSGraph<sWord> gd, MSGraph<sWord> gx, double rs, int sm, FILE* fp)
{
 	int     nn=0;
	RgnStat next_pr, next_ch;


   while (pSearchRing->getDataNum()>0) {
		next_pr = pSearchRing->getData();
		next_ch = pSearchRing->getData();
		next_ch.sb = true;
		dupBranch.init();	// 重複領域チェック用


/*
		if (Inter_Path) {
			int nc = -1;
			if (nc!=next_pr.nc && !next_pr.st) {
				while (pInterpRing->getDataNum()>0) {
					RgnStat inter_pr = pInterpRing->getData();
					RgnStat inter_ch = pInterpRing->getData();
 	    			inter_branch_path(gd, gx, &inter_pr, &inter_ch, sm, fp);
	    			output_RgnStat(gx, inter_ch, true, fp); 
					free(inter_ch.gp);
				}
				nc = next_pr.nc;
			}
		}
*/
		
		search_tree(gd, gx, &next_ch, rs, sm, fp);
		if (isCanceled) break;	// リングバッファをクリアしないと.....

//		枝の開始点の領域の属性１：RGN_NORMAL, RGN_FORK, RGN_CONTINUE
//		枝の開始点の領域の属性２：RGN_DUMMY, RGN_DEL, RGN_END, RGN_OVLP_PNT
		if (next_ch.at==RGN_NORMAL || next_ch.at==RGN_FORK || next_ch.at==RGN_CONTINUE) {
			if (Inter_Path && !next_ch.st) inter_branch_path(gd, gx, &next_pr, &next_ch, sm, fp);
			output_RgnStat(gx, next_ch, true, fp); 
			free(next_ch.gp);

/*			if (Inter_Path && !next_ch.st) {
				pInterpRing->addData(next_pr);
				pInterpRing->addData(next_ch);
			}
			else {
				output_RgnStat(gx, next_ch, true, fp); 
				free(next_ch.gp);
			}*/
			nn++;
		}
		
		else if (next_ch.at==RGN_END || next_ch.at==RGN_DEL) {
			next_ch.at = RGN_DUMMY;
	    	output_RgnStat(gx, next_ch, false, fp); 
			free(next_ch.gp);
		}

		else if (next_ch.at==RGN_DUMMY) {
	    	output_RgnStat(gx, next_ch, false, fp); 
			free(next_ch.gp);
		}

		
		else if (next_ch.at==RGN_OVLP_PNT) {
			next_ch.ob = dupBranch.get_maxbrc();
	    	output_RgnStat(gx, next_ch, false, fp); 
			free(next_ch.gp);
		}

		else {
			DEBUG_MODE print_message("START_SEARCH_TREE: 枝の探索開始領域は不明な属性？ %d.\n", next_ch.at);
			free(next_ch.gp);
		}

	}


/*	while (pInterpRing->getDataNum()>0) {
		RgnStat inter_pr = pInterpRing->getData();
		RgnStat inter_ch = pInterpRing->getData();
 	    inter_branch_path(gd, gx, &inter_pr, &inter_ch, sm, fp);
	    output_RgnStat(gx, inter_ch, true, fp); 
		free(inter_ch.gp);
	}
*/

	DEBUG_MODE print_message("START_SEARCH_TREE: 出ます．\n");
    return nn;
}



/**
	機能：正確には枝探索．

*/
void  search_tree(MSGraph<sWord> gd, MSGraph<sWord> gx, RgnStat* ns, double rs, int sm, FILE* fp)
{
	int    i, nn;
    MSGraph<sWord>* vr[RGN_MAXNO];
    RgnStat* gs[RGN_MAXNO];
    RgnStat* rg;
    MSGraph<sWord>  vp, wp;

    if (ns->at==RGN_DUMMY) return;

/////////////////////////////////////////////////////////
//	println_RgnStat(*ns);
	
	CVCounter* counter = GetUsableGlobalCounter();
	if (counter!=NULL) {
		counter->SetPos(ns->nc);
		if (counter->isCanceled()) isCanceled = true;
	}
	if (isCanceled) return;
/////////////////////////////////////////////////////////

	double lp = loop_region_check(gx, ns, BRANCH_OVLP_UNIT);	// 重複度のチェック
	// 注１
/*	if (lp==1.0 && !ns->sb) {
		ns->at = RGN_OVLP_TERM;
		DEBUG_MODE print_message("search_tree: 領域重複. lp = %f,  %d-> %d\n", lp, ns->bn, ns->ob);
		return;
	}
*/	
	// 探索範囲外
    if ((ns->xx<0.0 || ns->xx>gd.xs-1.0) ||	(ns->yy<0.0 || ns->yy>gd.ys-1.0) ||
        (ns->zz<0.0 || ns->zz*gd.RZxy>gd.zs-1.0)) { 
		DEBUG_MODE print_message("SEARCH_TREE: 探索空間外．\n");
		
		double lt = ((double)ns->on)/ns->un;
		if (lt>=BRANCH_OVLP_CHECK) {				// 枝が重複している 
			if (ns->sb) ns->at = RGN_OVLP_PNT;
			else		ns->at = RGN_OVLP_TERM;		// 重複の末端
			DEBUG_MODE print_message("search_tree: 領域重複. lt = %f,  %d-> %d\n", lt, ns->bn, ns->ob);
		}
		else ns->at = RGN_DEL;
		return;
	}


	// 枝が深すぎる
    if (ns->rn>=STOP_SEARCH_DEPTH_OF_BRANCH) {
		DEBUG_MODE print_message("SEARCH_TREE: 規定深度超過．\n");

		double lt = ((double)ns->on)/ns->un;
		if (lt>=BRANCH_OVLP_CHECK) {				// 枝が重複している 
			if (ns->sb) ns->at = RGN_OVLP_PNT;
			else		ns->at = RGN_OVLP_TERM;		// 重複の末端
			DEBUG_MODE print_message("search_tree: 領域重複. lt = %f,  %d-> %d\n", lt, ns->bn, ns->ob);
		}
		else ns->at = RGN_DEL;
		return;
	}

////////////////////////////////////////////////////////////////////
	nn = labeling(gd, gx, ns, gs, vr, rs, sm);		// 子領域の探索/ 
////////////////////////////////////////////////////////////////////
 
	if (nn!=1) {	// 停止条件
		DEBUG_MODE print_message("SEARCH_TREE: 枝番号 %d の探索は停止\n", ns->bn);

		double lt = ((double)ns->on)/ns->un;
		if (lt>=BRANCH_OVLP_CHECK) {				// 枝が重複している 
			ns->at = RGN_OVLP_TERM;					// 重複の末端
			DEBUG_MODE print_message("search_tree: 領域重複. lt = %f,  %d-> %d\n", lt, ns->bn, ns->ob);
		}

		if (nn==0) {
			if (ns->at!=RGN_OVLP_TERM) ns->at = RGN_END;
			else if (ns->sb)		   ns->at = RGN_OVLP_PNT;
			DEBUG_MODE print_message("SEARCH_TREE: 枝の末端．nn=0\n");
		}
		else if (nn>=2) {
			if (ns->at==RGN_OVLP_TERM) {
				if (ns->sb) ns->at = RGN_OVLP_PNT;
				free_data(gs, vr, 0, nn);
				DEBUG_MODE print_message("SEARCH_TREE: 枝の末端．\n");
			}
			else {
				ns->at = RGN_FORK;
	    		for (i=0; i<nn; i++) {
					pSearchRing->addData(*ns);
					pSearchRing->addData(*gs[i]);
					//add_search_ring(*ns, *gs[i]); 
	    			free(gs[i]);
	    			free(vr[i]);
	    		}
	    		DEBUG_MODE print_message("search_tree: 次の領域数 = %d\n", nn);
			}
		}
		return;
	}

/////////////////////////////////////////////////////////////
	rg = gs[0];
	search_tree(gd, gx, rg, rs, sm, fp);	// 再帰
/////////////////////////////////////////////////////////////

	// 属性処理
	if (rg->at==RGN_NORMAL || rg->at==RGN_CONTINUE) {
 	    if (Inter_Path&& !rg->st) inter_region_path(gd, gx, ns, rg, sm, fp);
		output_RgnStat(gx, *rg, true, fp); 
	}

	else if (rg->at==RGN_END) {
		if (rg->ov>=BRANCH_OVLP_UNIT) {	// 末端の重複領域を削除する．（出力される部分）
			ns->nn = 0;
			ns->at = RGN_END;
			//rg->at = RGN_DEL; // or RGN_DUMMY
			//rg->nn = 0;
		}
		else {
 			if (Inter_Path&& !rg->st) inter_region_path(gd, gx, ns, rg, sm, fp);
			output_RgnStat(gx, *rg, true, fp);
		}
	}

	else if (rg->at==RGN_FORK) {
		if (rg->ov>=BRANCH_OVLP_UNIT) {	// 末端の重複領域を削除する．（出力される部分）
			ns->at = RGN_FORK;
			ns->nc = rg->nc;
			ns->nn = rg->nn;
			for (int i=0; i<ns->nn; i++) ns->nx[i] = rg->nx[i];
			//rg->at = RGN_DEL; // or RGN_DUMMY
			//rg->nn = 0;
		}
		else {
 			if (Inter_Path && !rg->st) inter_region_path(gd, gx, ns, rg, sm, fp);
			output_RgnStat(gx, *rg, true, fp);
		}
	}
	
	else if (rg->at==RGN_DEL) {
		ns->nn = 0;
		ns->at = RGN_END;
	}

	else if (rg->at==RGN_OVLP_TERM) {
		ns->nn = 0;
//		注１（上を見よ）と関連
//		if (ns->ob==0) ns->ob = rg->ob;
//		ns->ob = rg->ob;
		if (ns->sb) ns->at = RGN_OVLP_PNT;
		else		ns->at = RGN_OVLP_TERM;
	}

//	else if (rg->at==RGN_DUMMY) {}

	free_data(gs, vr, 0, 1);
    return;
}



/**
	機能：親領域nsの情報を元に gdから次の領域（gs, vr）を探す． 

*/
int    labeling(MSGraph<sWord> gd, MSGraph<sWord> gx, RgnStat* ns, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm)
{
	int    kk, i, k;
	bool   mf;

    RgnStat        *gz[RGN_MAXNO];
    MSGraph<sWord>  *vz[RGN_MAXNO];
 
	// 領域の抽出
    kk = search_region_with_thd(gd, *ns, gs, vr, rs, sm, NORMAL);
    if (kk<0) kk = 0;

	// 球面探索
	int mm = 0;		// 球面での探索した場合の領域数
	int nx = 6;
	double st = 0.6;

    if ((ns->fg>=1.5 || kk!=1) || (Sphere_Start && ns->st)) { 
		mf = false;
			for (k=0; k<=nx; k++) {
			free_data(gz, vz, 0, mm);
            double rc = st + k*0.1f;
            double rx = ns->vx/2.*rc + 0.5;
   			mm = sphere_next_region(gd, gx, *ns, gz, vz, rx, sm);
	    	if ((mm>=2 || (Sphere_Start && ns->st && mm>=1)) && mm<NXTRGN_MAXNO) {
				mf = true;
				break;
	    	}
		}


		//
/*		if (!mf) {
	    	free_data(gz, vz, 0, mm);
	    	mm = 0;
			for (k=0; k<=nx; k++) {
				free_data(gz, vz, 0, mm);
				double rc = st - 0.2 + k*0.1;
				double rx = ns->vx/2.*rc + 0.5;
   				mm = sphere_next_region(gd, gx, *ns, gz, vz, rx, sm);
	    		if (mm>=1 && mm<NXTRGN_MAXNO) {
					mf = true;
					break;
	    		}
			}
		}
*/

		if (!mf) {
	    	free_data(gz, vz, 0, mm);
	    	mm = 0;
		}
    }
    

	// 平面探索で0, 球面探索で1の場合は分岐として取り扱う（不要な部分は後で消す）
    if (kk==0 && mm==1) {
		mm = 2;
		vz[1]  = new MSGraph<sWord>(ns->xs, ns->ys, 1, gd.zero);	
		gz[1]  = (RgnStat*)malloc(sizeof(RgnStat));
		*gz[1] = *ns;
		gz[1]->gp = vz[1]->gp;
		gz[1]->at = RGN_DUMMY;
		gz[1]->nr = true;
	}


	if (mm>kk || (Sphere_Start && ns->st)) {
		// 球面探索を採用
		free_data(gs, vr, 0, kk);
		kk = mm;
		for (i=0; i<mm; i++) {
	    	gs[i] = gz[i];
	    	vr[i] = vz[i];
		}
	}
	else {			
		// 平面探索を採用
		free_data(gz, vz, 0, mm);
    }

    if (kk>NXTRGN_MAXNO) {
        DEBUG_MODE print_message("LABELING: エラー!!　子領域の候補数が多すぎる!!  %d\n", kk);
		free_data(gs, vr, 0, kk);
    	return 0;
    }


    ns->nn = kk;
	if (kk==1) {
		gs[0]->bn = ns->bn;
		gs[0]->bp = ns->bp;
		gs[0]->rn = ns->rn;
        gs[0]->pn = kk;
        gs[0]->gp = vr[0]->gp;
        gs[0]->nc = ++Region_No;
        gs[0]->np = ns->nc;
        gs[0]->at = RGN_NORMAL;
        gs[0]->nr = true;
		gs[0]->sb = false;
		gs[0]->st = false;
		gs[0]->un = ns->un + 1;
		gs[0]->on = ns->on;
		gs[0]->ob = ns->ob;
    	ns->nx[0] = gs[0]->nc;
	}
	else {	// 分岐ポイント
		for(i=0; i<kk; i++) {
            gs[i]->bn = ++Branch_No;
            gs[i]->bp = ns->bn;
            gs[i]->rn = ns->rn + 1;
			gs[i]->pn = kk;
			gs[i]->gp = vr[i]->gp;
			gs[i]->nc = ++Region_No;
			gs[i]->np = ns->nc;
			gs[i]->at = RGN_NORMAL;
			gs[i]->nr = true;
			gs[i]->sb = true;
			gs[i]->st = false;
			gs[i]->un = 1;
			gs[i]->on = 0;
			gs[i]->ob = 0;
			gs[i]->op = false;
			gs[i]->lt = false;
			gs[i]->ov = 0.0;
    		ns->nx[i] = gs[i]->nc;
		}
    }

    return kk;
}



/**
int   search_region_with_thd(MSGraph<sWord> gd, RgnStat ns, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm, int cm)

	機能：親領域の情報を用いて，平面領域で子領域を探索する．閾値は自動調整．

	引数：
		gd : 探索空間
		ns : 親領域
		gs : 子領域配列へのポインタ（戻り値）
		vr : 子領域の画像データ配列へのポインタ（戻り値）
		sm : サーチモード
		cm : チェックモード

	戻り値：子領域の数
		gs : 子領域配列へのポインタ（戻り値）
		vr : 子領域の画像データ配列へのポインタ（戻り値）

*/  
int   search_region_with_thd(MSGraph<sWord> gd, RgnStat ns, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm, int cm)
{
    MSGraph<sWord> vp;

    vp = look_forward(gd, ns, rs, FTMODE);						// 前方の平面を観測
    int nn = get_region_with_thd(ns, vp, gs, vr, rs, sm, cm);	// 閾値を自動計算し，子領域を取り出さす．
    for (int i=0; i<nn; i++) {
		//get_world_center (ns, gs[i], rs);						// 各子領域の重心座標の計算
		get_direct_vector(ns, gs[i]);							// 方向ベクトルの計算
		//relook_region_with_thd(gd, ns, &gs[i], &vr[i], sm);		// 自分の方向ベクトルを使って，領域を見直す．
		relook_region(gd, ns, &gs[i], &vr[i], sm);				// 自分の方向ベクトルを使って，領域を見直す．
	}

    free(vp.gp);
    return nn;
}

 

/**
int   search_region(MSGraph<sWord> gd, RgnStat ns, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm, int cm)

	機能：親領域の情報を用いて，平面領域で子領域を探索する．閾値は固定．

	引数：
		gd : 探索空間
		ns : 親領域
		gs : 子領域配列へのポインタ（戻り値）
		vr : 子領域の画像データ配列へのポインタ（戻り値）
		sm : サーチモード
		cm : チェックモード

	戻り値：子領域の数
		gs : 子領域配列へのポインタ（戻り値）
		vr : 子領域の画像データ配列へのポインタ（戻り値）

*/
int   search_region(MSGraph<sWord> gd, RgnStat ns, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm, int cm)
{
    MSGraph<sWord> vp;

    vp = look_forward(gd, ns, rs, FTMODE);					// 前方の平面を観測
	int nn = get_region(ns, vp, gs, vr, rs, sm, cm);		// 観測面 vp から，閾値 ns.mm で子領域 gs, vr を取り出す．
    for (int i=0; i<nn; i++) {
		//get_world_center (ns, gs[i], rs);					// 各子領域の重心座標の計算
		get_direct_vector(ns, gs[i]);						// 方向ベクトルの計算
		relook_region(gd, ns, &gs[i], &vr[i], sm);			// 自分の方向ベクトルを使って，領域を見直す．
	}
    free(vp.gp);
    return nn;
}



void  relook_region_with_thd(MSGraph<sWord> gd, RgnStat ns, RgnStat** cs, MSGraph<sWord>** vz, int sm)
{
    MSGraph<sWord> *vr[RGN_MAXNO], vp, *vw;
    RgnStat        *gs[RGN_MAXNO], *gw;

    vp = look_me(gd, **cs, FTMODE);					// 前方の平面を観測

	// 観測面 vp から，閾値 ns.mm で子領域 gs, vr を取り出す．
	int nn = get_region_with_thd(**cs, vp, gs, vr, 0.0, sm, RELOOK);	
	free(vp.gp);
 
	if (nn>=1) {
		gw  = *cs;
		*cs = *gs;
		*gs = gw;
		vw  = *vz;
		*vz = *vr;
		*vr = vw;
	}
	free_data(gs, vr, 0, nn);

    return;
}



/**
	機能：検出した（子)領域で,方向ベクトルを再設定した場合の領域の再検出
		　この関数を呼び出す前に，方向ベクトルの再設定が必要．
		　

*/
void   relook_region(MSGraph<sWord> gd, RgnStat ns, RgnStat** cs, MSGraph<sWord>** vz, int sm)
{
    MSGraph<sWord> *vr[RGN_MAXNO], vp, *vw;
    RgnStat       *gs[RGN_MAXNO], *gw;

    vp = look_me(gd, **cs, FTMODE);				// 前方の平面を観測
	int nn = get_region(**cs, vp, gs, vr, 0.0, sm, RELOOK);	// 観測面 vp から，閾値 ns.mm で子領域 gs, vr を取り出す．
    free(vp.gp);
 
	if (nn>=1) {
		gw  = *cs;
		*cs = *gs;
		*gs = gw;
		vw  = *vz;
		*vz = *vr;
		*vr = vw;
	}
	free_data(gs, vr, 0, nn);

    return;
}



int    sphere_next_region(MSGraph<sWord> gd, MSGraph<sWord> gx, RgnStat ns, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm)
{
    int  i, kk, nn, mm;
//    double    lp;
    Vector<double>*  vz;
    MSGraph<sWord>* vx[RGN_MAXNO];
    RgnStat* gr[RGN_MAXNO];


    nn = sphere_next_vector(gd, ns, &vz, rs, sm);

    kk = 0;
    for (i=0; i<nn; i++) {
		ns.ex = vz[i].x;
		ns.ey = vz[i].y;
		ns.ez = vz[i].z;

    	mm = search_region(gd, ns, gr, vx, rs, sm, SPHERE_NEXT);

        if (mm>0) {
           double lp = loop_region_check(gx, gr[0], 0.6f);
           if (lp<0.6) {
                gs[kk] = gr[0];
                vr[kk] = vx[0];
                kk++;
                free_data(gr, vx, 1, mm);
			}
			else {
				free_data(gr, vx, 0, mm);
			}
        }
    }

	if (nn>0) delete [] vz;
    //for (i=0; i<nn; i++) delete &vz[i];
	//free(vz);

    kk = rsorting(gs, vr, kk);
    nn = chk_region_status(ns, gs, kk, sm, SPHERE_CHK);
    nn = nsorting(gs, vr, kk);

    return nn;
}
	


int  sphere_next_vector(MSGraph<sWord> gd, RgnStat ns, Vector<double>** vz, double rx, int sm)
{
    int       i, j, x, y, ci, cx, nn, sf, sb;
    double    snt, cst, snf, csf;
    double    xx, yy, rr, th, sn, dt;
    double    st = PI;
    double    fx, fy, fz;
    double    bx, by, bz;
    double    ex, ey, ez;

    MSGraph<sWord>   fmap, bmap, fvp, bvp;
    Vector<double>  vx, vc[RGN_MAXNO];

	vx.set(ns.ex, ns.ey, ns.ez);
    ToPola(vx, cst, snt, csf, snf);

    fmap = pviewport(gd, ns, rx, st);
    ns.ex = -ns.ex;
    ns.ey = -ns.ey;
    ns.ez = -ns.ez;
    bmap = pviewport(gd, ns, rx, st);
//GLMPDisp->displayNew(fmap);
//GLMPDisp->displayNew(bmap);

//	DEBUG_MODE print_message("--> %d  %d\n", ns.ml, ns.mm);
/*	for (int q=0; q<fmap.xs; q++)
		for (int r=0; r<fmap.ys; r++){
			if (fmap.point(q, r)!=0) DEBUG_MODE print_message("  %d \n", fmap.point(q, r));
		}
*/

	fvp = pre_cut_region(ns, fmap, sm);
    bvp = pre_cut_region(ns, bmap, sm);
	if (!ns.st) sphere_back_clean(fvp, bvp);	// 木の開始点の場合はバックを消去しない
//GLMPDisp->displayNew(fvp);
//GLMPDisp->displayNew(bvp);

	dt = st/(fmap.xs-3);
    nn = 0;
    for (i=2; i<fvp.xs-2; i++) {
        for (j=2; j<fvp.ys-2; j++) {
	    	ci = j*fvp.xs + i;
	    	if (fvp.gp[ci]!=0) {
				bi_paint(fvp, bvp, i, j, 1, EMAXCT, UMAXCT, 8, FORWARD);

				// 前面の領域の重心を求める
				fx = fy = fz = 0.0;
				xx = yy = 0.0;
				sf = 0;
    			for (x=2; x<fvp.xs-2; x++) {
        	    	for (y=2; y<fvp.ys-2; y++) {
						cx = y*fvp.xs + x;
						if (fvp.gp[cx]==UMAXCT) {
			    			xx = xx + x;
			    			yy = yy + y;
			   		 		sf++;
						}
		    		}
				}	
				if (sf!=0) {
		    		xx = (fvp.xs-1)/2.f - xx/sf;
		    		yy = (fvp.ys-1)/2.f - yy/sf;
					rr = sqrt(xx*xx + yy*yy);
					th = dt*rr;
					sn = sin(th);
					fx = cos(th);
					fy = 1.0f;
					fz = 0.0f;
					if (Xabs(rr)>Zero_Eps) {
						fy = xx/rr;
						fz = yy/rr;
					}
	    			if (sn<Sin_Tolerance) {
						fx = Sign(fx);
						sn = 0.0f;
						fy = 1.0f;
						fz = 0.0f;
					}
					fy = sn*fy;
					fz = sn*fz;
				}

				// 背面の領域の重心を求める
				bx = by = bz = 0.0;
				xx = yy = 0.0;
				sb = 0;
    			for (x=2; x<fvp.xs-2; x++) {
        	    	for (y=2; y<fvp.ys-2; y++) {
						cx = y*fvp.xs + x;
						if (bvp.gp[cx]==UMAXCT) {
			    			xx = xx + x;
			    			yy = yy + y;
			    			sb++;
						}
		    		}
				}	
				if (sb!=0) {
		    		xx = (bvp.xs-1)/2.f - xx/sb;
		    		yy = (bvp.ys-1)/2.f - yy/sb;
		    		rr = sqrt(xx*xx + yy*yy);
		    		th = PI - rr*dt;
		    		sn = sin(th);
					bx = cos(th);
					by = -1.0;
					bz = 0.0;
					if (Xabs(rr)>Zero_Eps) {
						by = -xx/rr;
						bz =  yy/rr;
					}
	    			if (sn<Sin_Tolerance) {
						bx = Sign(bx);
						sn = 0.0;
						by = -1.0;
						bz = 0.0;
					}
					by = sn*by;
					bz = sn*bz;
				}

				bi_paint(fvp, bvp, i, j, 1, UMAXCT, 0, 8, FORWARD);

				//if (sf+sb>fvp.xs) {	// 要再考
				if (sf+sb>1) {
		    		fx = fx*sf;
		    		fy = fy*sf;
		    		fz = fz*sf;
		    		bx = bx*sb;
		    		by = by*sb;
		    		bz = bz*sb;

		    		vc[nn++].set(fx+bx, fy+by, fz+bz);
		    		if (nn>=RGN_MAXNO) {
		    			DEBUG_MODE print_message("SPHERE_NEXT_VECTOR: フェース１: 領域の数が多すぎる. %d\n", RGN_MAXNO);
						fmap.free();
						bmap.free();
						fvp.free();
						bvp.free();
		    			return -1;
		    		}
				}
	    	}
		}
    }

	// 背面の残りを探索（前面は全て終わっている）
    for (i=2; i<bvp.xs-2; i++) {
        for (j=2; j<bvp.ys-2; j++) {
	    	ci = j*bvp.xs + i;
	    	if (bvp.gp[ci]!=0) {
				bi_paint(fvp, bvp, i, j, 1, EMAXCT, UMAXCT, 8, BACKWARD);

				bx = by = bz = 0.0f;
				xx = yy = 0.0f;
				sb = 0;
    			for (x=2; x<bvp.xs-2; x++) {
        	    	for (y=2; y<bvp.ys-2; y++) {
						cx = y*bvp.xs + x;
						if (bvp.gp[cx]==UMAXCT) {
			    			xx = xx + x;
			    			yy = yy + y;
			    			sb++;
						}
		    		}
				}	
				bi_paint(fvp, bvp, i, j, 1, UMAXCT, 0, 8, BACKWARD);

				//if (sb>bvp.xs) {	// 要再考
				if (sb>1) {
		    		xx = (bvp.xs-1)/2.0 - xx/sb;
		    		yy = (bvp.ys-1)/2.0 - yy/sb;
		    		rr = sqrt(xx*xx + yy*yy);
		    		th = PI - rr*dt;
		    		sn = sin(th);
					bx = cos(th);
					by = -1.0;
					bz = 0.0;
					if (Xabs(rr)>Zero_Eps) {
						by = -xx/rr;
						bz =  yy/rr;
					}
	    			if (sn<Sin_Tolerance) {
						bx = Sign(bx);
						sn = 0.0;
						by = -1.0;
						bz = 0.0;
					}
					by = sn*by;
					bz = sn*bz;

		    		vc[nn++].set(bx, by, bz);
		    		if (nn>=RGN_MAXNO) {
		    			DEBUG_MODE print_message("SPHERE_NEXT_VECTOR: フェース２: 領域の数が多すぎる. %d\n", RGN_MAXNO);
						fmap.free();
						bmap.free();
						fvp.free();
						bvp.free();
		    			return -1;
		    		}
				}
	    	}
		}
    }

    *vz = new Vector<double>[nn];
    for (i=0; i<nn; i++) {
		vc[i].normalize();
        ex = vc[i].x*snt*csf - vc[i].y*snf - vc[i].z*cst*csf;
        ey = vc[i].x*snt*snf + vc[i].y*csf - vc[i].z*cst*snf;
        ez = vc[i].x*cst                   + vc[i].z*snt;
		vx.set(ex, ey, ez);
		vx.normalize();
		(*vz)[i] = vx;
    }

	fmap.free();
	bmap.free();
	fvp.free();
	bvp.free();

    return nn;
}


 
void  sphere_back_clean(MSGraph<sWord> fvp, MSGraph<sWord> bvp)
{
    int  i, j, xs, ys, cc, ii, ij, ix, iy, xx, yy;

    xs = bvp.xs;
	ys = bvp.ys;
    cc = bvp.gp[((ys-1)/2)*xs+(xs-1)/2];

    if (cc!=0) bi_paint(fvp, bvp, (xs-1)/2, (ys-1)/2, 1, EMAXCT, 0, 8, BACKWARD);
    else {
		xx = yy = 0;
		for (i=xs/3; i<xs*2/3; i++)
		for (j=ys/3; j<ys*2/3; j++) {
	    	if (bvp.gp[j*xs+i]!=0) {
	    		ii = (i - (xs-1)/2)* (i - (xs-1)/2);
	    		ij = ((ys-1)/2 - j)* ((ys-1)/2 - j);
	    		ix = (xx - (xs-1)/2)*(xx - (xs-1)/2);
	    		iy = ((ys-1)/2 - yy)*((ys-1)/2 - yy);
				if (ii+ij <= ix+iy) {
		    		xx = i;
		    		yy = j;
				}
	    	}
		}

		if (!(xx==0 && yy==0)) {
	    	bi_paint(fvp, bvp, xx, yy, 1, EMAXCT, 0, 8, BACKWARD);
		}
/*
		else  {
 	    	fprintf(stderr,"SPHERE_BACK_CLEAN: miss back!!");
		}
*/
    }
}



void  inter_region_path(MSGraph<sWord> gd, MSGraph<sWord> gx, RgnStat* ps, RgnStat* ns, int sm, FILE* fp)
{
    int    nn, np, pp, j;
    double fg, ss, mx, mn, mm;

    MSGraph<sWord> *vr[RGN_MAXNO], vp;
    RgnStat       *gs[RGN_MAXNO], gr[INTERPATH_MAX], ds;
	Vector<double> pe, ne, dr, de, ee;

	dr.set(ns->xx - ps->xx, ns->yy - ps->yy, ns->zz - ps->zz);
//	np = (int)(dr.n*2) + 1;
    np = ((int)dr.n + 1)*2;
	np = Min(np, INTERPATH_MAX-2);
	dr = dr/np;

	pe.set(ps->px, ps->py, ps->pz);
	ne.set(ns->px, ns->py, ns->pz);
	de = (ne - pe)/np;

    mx = (ns->mx - ps->mx)/(double)np;
    mn = (ns->mn - ps->mn)/(double)np;
    mm = (ns->mm - ps->mm)/(double)np;
    ss = (ns->ss - ps->ss)/(double)np;
    fg = (ns->fg - ps->fg)/np;

    pp = ps->nc;
    ds = *ps;
    int k  = 0;
	int i;
    for (i=1; i<=np-1; i++){
		ee = pe + de*i;	
		ee.normalize();

		ds.xx = ps->xx + dr.x*i;
		ds.yy = ps->yy + dr.y*i;
		ds.zz = ps->zz + dr.z*i;
		ds.ex = ds.px = ee.x;
		ds.ey = ds.py = ee.y;
		ds.ez = ds.pz = ee.z;
		ds.mx = ps->mx + (int)(mx*i);
		ds.mn = ps->mn + (int)(mn*i);
		ds.mm = ps->mm + (int)(mm*i);
		ds.ss = ps->ss + (int)(ss*i);
		ds.fg = ps->fg + fg*i;

		vp = look_me(gd, ds, FTMODE);
//		GLMPDisp->displayNew(vp);
		nn = get_region(ds, vp, gs, vr, 0.0, sm, INTER_PATH);
		//for (int j=0; j<nn; j++) get_world_center (ds, gs[j], 0.0);
		free(vp.gp);

        if (nn>0) {
            gs[0]->ip = true;					// 領域間の補間領域
            gs[0]->ib = false;					// 枝間の補間領域
			gs[0]->st = false;
			gs[0]->sb = false;
			gs[0]->at = RGN_NORMAL;				// 通常の領域
			gs[0]->pn = 1;
            gs[0]->nn = 1;
	    	gs[0]->np = pp;
            pp = ++Region_No;
            gs[0]->nc = pp;

			if (k==0) {			// 最初の領域
				//ps->nx[0] = pp;
				gs[0]->nx[0] = pp + 1;
			}
			else {				// 途中の領域
				gs[0]->nx[0] = pp + 1;
			}

	    	gr[k] = *gs[0];

	    	free_data(gs, vr, 1, nn);
	    	free(gs[0]);
	    	free(vr[0]);

	    	k++;
	    	if (k>=INTERPATH_MAX) {
				DEBUG_MODE print_message("INTER_PATH: 領域間の補間数が多すぎる．配列(INTERPATH_MAX)が足りない!!\n");
    			for (j=0; j<k; j++) free(gr[j].gp);
				k = 0;
				break;
	    	}
		}
    }

	if (k>0) {	// 最後の領域
		gr[k-1].nx[0] = ns->nc;
		ns->np = gr[k-1].nc;
	}

    for (i=0; i<k; i++) {
		output_RgnStat(gx, gr[i], true, fp);
		free(gr[i].gp);
    }

    return;
}



//
//	注：この関数を使用する場合，枝の最後の領域の nx[] は不正確になる．
//
void  inter_branch_path(MSGraph<sWord> gd, MSGraph<sWord> gx, RgnStat* ps, RgnStat* ns, int sm, FILE* fp)
{
    int    nn, np, bn, j;
    double fg, ss, mx, mn, mm;

    MSGraph<sWord> *vr[RGN_MAXNO], vp;
    RgnStat       *gs[RGN_MAXNO], gr[INTERPATH_MAX], ds;
	Vector<double> pe, ne, dr, de, ee;


	dr.set(ns->xx-ps->xx, ns->yy-ps->yy, ns->zz-ps->zz);
	//np = (int)(dr.n*2) + 1;
    np = ((int)dr.n + 1)*2;
	np = Min(np, INTERPATH_MAX-2);
	dr = dr/np;

	pe.set(ps->px, ps->py, ps->pz);
	ne.set(ns->px, ns->py, ns->pz);
	de = (ne - pe)/np;

    mx = (ns->mx - ps->mx)/(double)np;
    mn = (ns->mn - ps->mn)/(double)np;
    mm = (ns->mm - ps->mm)/(double)np;
    ss = (ns->ss - ps->ss)/(double)np;
    fg = (ns->fg - ps->fg)/np;

    int pp = ps->nc;
    ds = *ps;
	ds.vs = (ps->vs + ns->vs);	//	/2*2.0  枝間は変化に富むので．
	bn = (++Branch_No);

	int i;
	int k  = 0;
    for (i=1; i<=np-1; i++){
//    for (int i=0; i<=np; i++){
		ee = pe + de*i;	
		ee.normalize();

		ds.xx = ps->xx + dr.x*i;
		ds.yy = ps->yy + dr.y*i;
		ds.zz = ps->zz + dr.z*i;
		ds.ex = ds.px = ee.x;
		ds.ey = ds.py = ee.y;
		ds.ez = ds.pz = ee.z;
		ds.mx = ps->mx + (int)(mx*i);
		ds.mn = ps->mn + (int)(mn*i);
		ds.mm = ps->mm + (int)(mm*i);
		ds.ss = ps->ss + (int)(ss*i);
		ds.fg = ps->fg + fg*i;
		//ds.vs = vs;

		vp = look_me(gd, ds, FTMODE);
//		GLMPDisp->displayNew(vp);
		nn = get_region(ds, vp, gs, vr, 0.0, sm, INTER_PATH);
//		for (j=0; j<nn; j++) get_world_center (ds, gs[j], 0.0);
		free(vp.gp);

//		DEBUG_MODE print_message("INTER_PATH: nn = %d\n", nn);

        if (nn>0) {
//		GLMPDisp->displayNew(*vr[0]);
            gs[0]->ip = false;					// 領域間の補間領域
            gs[0]->ib = true;					// 枝間の補間領域
			gs[0]->st = false;
			gs[0]->sb = false;
//			gs[0]->at = RGN_NORMAL;				// 枝間の補間領域
			gs[0]->at = RGN_INTER;				// 枝間の補間領域
//			gs[0]->bn = 0;
            gs[0]->pn = 1;
            gs[0]->nn = 1;
			gs[0]->gn = ps->gn;
	    	gs[0]->np = pp;
			gs[0]->bn = bn;
            pp = ++Region_No;
            gs[0]->nc = pp;
			
			if (k==0) {			// 最初の領域
				gs[0]->nx[0] = pp + 1;
				gs[0]->sb = true;
			}
			else {				// 途中の領域
				gs[0]->nx[0] = pp + 1;
			}
			
			gr[k] = *gs[0];
	    	free_data(gs, vr, 1, nn);
	    	free(gs[0]);
	    	free(vr[0]);

	    	k++;
	    	if (k>=INTERPATH_MAX) {
				DEBUG_MODE print_message("INTER_BRANCH: 枝間の補間数が多すぎる．配列(INTERPATH_MAX)が足りない!!\n");
    			for (j=0; j<k; j++) free(gr[j].gp);
				k = 0;
				break;
	    	}
		}
    }

	if (k>0) {	// 最後の領域
		gr[k-1].nx[0] = ns->nc;
		ns->np = gr[k-1].nc;
		gr[k-1].at = RGN_CONTINUE;
	}

//	if (k<2) {
//		DEBUG_MODE print_message("INTER_BRANCH: 枝間の補間数が少なすぎる %d < 2\n", k);
//	}

    for (i=0; i<k; i++) {
		output_RgnStat(gx, gr[i], true, fp);
		free(gr[i].gp);
    }

    return;
}



/**
	枝間の補間データは保存しない．

  */
void   output_RgnStat(MSGraph<sWord> gx, RgnStat gs, bool sw, FILE* fp)
{ 
    if (sw) {
		if (gs.ib) gs.bn = 0;		// 枝間の領域は枝番号0の色とする．
		Local2World_RS(gx, gs, 4);
	}

	if (fp!=NULL) {
		writeRgnData(fp, gs);
		SavedRegionNum++;
	}
    return;
}


