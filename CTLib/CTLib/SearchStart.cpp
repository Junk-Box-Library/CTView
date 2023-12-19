#include  "SearchStart.h"
#include  "StatusCheck.h"
#include  "Threshold.h"



MSGraph<sWord>*  OnePointRRSearch(MSGraph<sWord> gd, Vector<double> pt, Vector<double> vt, Vector<int> th, int sm, FILE* fp)
{
    int   nn;
	bool  tempfile=false;
    RgnStat       *gs[RGN_MAXNO], ns;
    MSGraph<sWord> *vr[RGN_MAXNO], gx, vp, *xp;

	xp = new MSGraph<sWord>();

	gx = gd;
	gx.getm(gd.xs, gd.ys, gd.zs, gd.zero);
	gx.max = gd.max;
	gx.min = gd.min;
	if (gx.isNull()) {
		xp->state = JBXL_GRAPH_MEMORY_ERROR;
		print_message("OnePointRRSearch: 抽出空間のメモリを確保できない!! (%dx%dx%d)\n", gd.xs, gd.ys, gd.zs);
		return xp;
	}

	// グローバル変数
	GLMPgd = &gd;				// 探索空間	
	GLMPgx = &gx;				// 抽出空間	
    Region_No = Branch_No = Tree_No = 0;
	isCanceled = false;
	SavedRegionNum = 0;

	ns.xx = pt.x;
	ns.yy = pt.y;
	ns.zz = pt.z;
	ns.ex = ns.px = vt.x;
	ns.ey = ns.py = vt.y;
	ns.ez = ns.pz = vt.z;
    ns.mh = th.x;				// 閾値
    ns.mx = th.x;
    ns.mm = th.y;
    ns.mn = th.z;
    ns.ml = th.z;
	ns.nc = 0;
	ns.vs = 100;
	ns.at = RGN_DUMMY;			// 

	vp = look_forward(gd, ns, 0.0, FTMODE);		// rs = 0.0
//	GLMPDisp->displayNew(vp);
	nn = get_region_paint_first(ns, vp, gs, vr, 0.0, sm, ALL_OK, 1.5, (vp.xs-1)/2, (vp.ys-1)/2);	// fg<=1.1 に閾値を調整
	if (nn<=0) {
		DEBUG_MODE print_message("RecursiveResionSearch: I cannot find start region on 1st step!!\n");
		vp.free();
		gx.free();
		return xp;
	}
//	search_most_thd(ns, vp, &gs[0], &vr[0], 0.0, sm);	// 最適閾値を求める． 今の所最適じゃないみたい．
//	GLMPDisp->displayNew(*vr[0]);

	// 計算時間短縮のための閾値探索の範囲　
/*	if (Xabs(sm)==BRONCHUS) {	
		ns.mx = Min(ns.mm+200, ns.mh);
		ns.mn = ns.mm;
	}
	else if (Xabs(sm)==BLOOD) {
		ns.mx = ns.mm;
		ns.mn = Max(ns.mm-400, ns.ml);
	}
*/

// 最初の領域
	get_world_center(ns, gs[0], 0.0);			// ワールド座標での重心の計算　rs = 0.0
	gs[0]->ex = ns.ex;							// 方向ベクトルは，設定値を使う
	gs[0]->ey = ns.ey;
	gs[0]->ez = ns.ez;
	gs[0]->px = ns.px;
	gs[0]->py = ns.py;
	gs[0]->pz = ns.pz;
	gs[0]->gn = 0;								// グループ番号
    gs[0]->tn = ++Tree_No;						// 木番号
    gs[0]->bn = ++Branch_No;					// 枝番号
    gs[0]->bp = 0;								// 親枝の枝番号
    gs[0]->nc = ++Region_No;					// 領域番号
    gs[0]->np = 0;								// 親領域の領域番号
    gs[0]->pn = 0;								// 親領域での分岐数（姉妹領域の数）．ここでは nnは木の数とする．
    gs[0]->rn = 1;								// 分岐の深さ．
    gs[0]->ip = false;							// 領域間の補間領域ではない．
    gs[0]->ib = false;							// 枝間の補間領域ではない．
	gs[0]->st = true;							// 木のスタートの領域
    gs[0]->sb = true;							// 枝のスタートの領域
	gs[0]->nr = true;							// 適当な領域
	gs[0]->at = RGN_NORMAL;						// 通常の領域
	gs[0]->un = 1;
	gs[0]->on = 0;
	gs[0]->ob = 0;
	gs[0]->op = false;
    gs[0]->lt = false;							// この枝はループしていない（最初の領域だから）
	gs[0]->ov = 0.0;
//	add_search_ring(ns, *gs[0]);				// 探索開始領域として探索リングに入れる．リングには *gs[0]のコピーが入る．
	pSearchRing->addData(ns);
	pSearchRing->addData(*gs[0]);

	vp.free();
	for (int i=0; i<nn; i++) {	// 子領域のグラフィックデータは破棄しない．      　
		free(gs[i]);
		free(vr[i]);
	}

	if (fp==NULL) {
		fp = wOpenRgnSLFile(TEMP_RGN_FILENAME);
		if (fp==NULL) {
			print_message("OnePointRRSearch: 一時的領域保存ファイルのオープンエラー\n");
		}
		else {
			print_message("OnePointRRSearch: 一時的領域保存ファイルのオープン\n");
			tempfile = true;
		}
	}

//	探索開始．/////////////////////////////////////////////////////////////////
	xp = RecursiveRegionSearch(gd, gx, sm, fp);
///////////////////////////////////////////////////////////////////////////////

	gx.free();

	
	if (fp!=NULL) {
		int nn = htonl(SavedRegionNum);
		fwrite(&nn, 4, 1, fp); 
		if (tempfile) fclose(fp);
	}

	return xp;
}



/*
void  search_objects (MSGraph<sWord> gd, RgnStat ns, double rs, FILE* fp)
{
    int   i, j, kk, mm, nn;
    RgnStat  *gs[RGN_MAXNO], *gx[RGN_MAXNO];
    MSGraph<sWord>  gg, *vr[RGN_MAXNO], *vx[RGN_MAXNO], vp;
    RBound<int>  rb;
 
    DEBUG_MODE print_message("search_objects: searching first area.\n");

	// 探索開始候補を探す．
    nn = pre_scan_top_plane(gd, ns, gx, vx, OBJECTS); 
    DEBUG_MODE print_message("search_objects: detected %d first area!!\n", nn);

    ns.nc = 0;		// 領域番号
    ns.bn = 0;		// 枝番号
    ns.ip = false;	// 領域間の補間
    ns.nn = nn;		// （子）領域の数
    ns.gp = NULL;

    rb.xmin = rb.ymin = rb.zmin = 0;
    Region_No = Branch_No = 0;
    kk = 0;

	// 各候補領域に対して，改めて探索を行い，探索開始領域を決める．
    for (i=0; i<nn; i++) {
		vp = look_forward(gd, *gx[i], 0.0, FTMODE, 0); 
		mm = get_region(*gx[i], vp, gs, vr, OBJECTS, NORMAL);
		for (j=0; j<mm; j++) get_vector(*gx[i], gs[j], 0.0);

		vp.free();
        if (mm<=0) {
            DEBUG_MODE print_message("SEARCH_OBJECTS: Warning. Miss Thresholding %d !!\n",i+1);
            continue;
        }

        kk++;
      	gs[0]->tn = kk;					// 木番号
        gs[0]->bn = ++Branch_No;		// 枝番号
        gs[0]->bp = 0;					// 親枝の枝番号
        gs[0]->nc = ++Region_No;		// 領域番号
        gs[0]->np = 0;					// 親領域の領域番号
        gs[0]->pn = 0;					// 親領域での分岐数（姉妹領域の数）．ここでは nnは木の数とする．
        gs[0]->rn = 1;					// 分岐の深さ．
        gs[0]->lp = false;				// ループはしていない（最初の領域だから）
        gs[0]->st = true;				// 木のスタートの領域
		gs[0]->nr = true;				// 適当な領域
		gs[0]->at = NORMAL_RGN;			// 通常の領域
      	gs[0]->mh = ns.mh;				// 閾値
      	gs[0]->mx = ns.mx;
      	gs[0]->mm = ns.mm;
      	gs[0]->mn = ns.mn;
      	gs[0]->ml = ns.ml;

		add_search_ring(ns, *gs[0]);	// 最初の物だけ探索開始領域として探索リングに入れる．リングには *gs[0]のコピーが入る．
		delete(gs[0]);					// 元データは破棄． 
		free_data(gs, vr, 1, mm);		// 残りも破棄．      　

		vx[i]->free();					// 候補領域も必要なくなったので消す．				
        delete(gx[i]);
        delete(vx[i]);
	}

	// 探索開始．
    nn = start_search_tree(gd, gg, rs, OBJECTS, fp);


	// 結果
    DEBUG_MODE print_message("\n");
    if (nn<=0) {
		DEBUG_MODE print_message("SEARCH_OBJECTS: I cannot find first region!!\n");
    }
    else {
		DEBUG_MODE print_message("search_objects: I found %d regions!\n", nn);
    }
}

*/



/*
int  searchStart(int sm)
{
    RBound   rb;
    RgnData  rd;
    MSGraph<sWord>  gd;
    RgnStat  ns;



    FilterMode = fm;
    Inter_Path = ip;

    if (file_exist(fi_name)==TRUE) {
		rd = read_rgn_slfile(fi_name, ON);
		gd = *rd.ws;
		free(rd.rs);
    }

    fp = wopen_rgn_slfile(fw_name, rb);	// 出力

    WPgd  = &gd;


    // Model of Virtual TOP Region 
    ns.mh = mh + ZEROCT;
    ns.mx = mx + ZEROCT;
    ns.mm = mm + ZEROCT;
    ns.mn = mn + ZEROCT;
    ns.ml = ml + ZEROCT;
    ns.ex = 0.0;
    ns.ey = 0.0;
    ns.ez = 1.0;

	
	if (Xabs(sm)==BRONCHUS) {
		debug_mesg("start BRONCHUS mode.");
		search_bronchus(gd, ns, rs, sm, fp);
	}
    else if (Xabs(sm)==BLOOD) {
		debug_mesg("start BLOOD mode.");
   		search_blood   (gd, ns, rs, fp, fo_name);
	}
    else if (Xabs(sm)==OBJECTS) {
		debug_mesg("start OBJECTS mode.");
  		search_objects (gd, ns, rs, sm, fp);
	}

    free(gd.gp);
    fclose(fp);

    return NORM;
}
*/



/*
void search_bronchus(MSGraph<sWord> gd, RgnStat ns, double rs, int sm, FILE* fp)
{
    int   i, j, kk, mm, nn;
    RgnStat  *gs[RGN_MAXNO], *gx[RGN_MAXNO];
    MSGraph<sWord>  *vr[RGN_MAXNO], *vx[RGN_MAXNO], vp;
    RBound   rb;
    openX    xd;


    DEBUG_MODE print_message("search_bronchus: searching first area.\n");
    nn = pre_scan_top_plane(gd, ns, gx, vx, sm); 
    DEBUG_MODE print_message("search_bronchus: detected %d first area!!\n",nn);

    ns.nc = 0;
    ns.bn = 0;
    ns.ip = NO;
    ns.nn = nn; 
    ns.gp = NULL;

    rb.xmin = rb.ymin = rb.zmin = 0;
    Region_No = Branch_No = 0;
    kk = 0;
    for (i=0; i<nn; i++) {
		vp = look_forward(gd, *gx[i], 0.0, FTMODE, 0, rb); 
		mm = search_thd(*gx[i], vp, gs, vr, sm, NORMAL);
    	for (j=0; j<mm; j++) get_vector(*gx[i], gs[j], 0.0);
   		free(vp.gp);
		free(gx[i]->gp);
        free(gx[i]);
        free(vx[i]);

        if (mm<=0) {
            DEBUG_MODE print_message("SEARCH_BRONCHUS: warning.\n");
            DEBUG_MODE print_message("miss threshulding %d !!\n",i+1);
            continue;
        }

        kk++;
        gs[0]->nc = ++Region_No; // Region No. 
        gs[0]->np = 0;           // Previous Region No.
        gs[0]->bn = ++Branch_No; // Branch No. 
        gs[0]->bp = 0;           // Previous Branch No. 
        gs[0]->pn = 0;	         // Previous Bunki Number 
        gs[0]->rn = 1;	         // Bunki kaisuu 
        gs[0]->lp = NO;
        gs[0]->st = YES;
        gs[0]->nr = NORMAL_RGN;
      	gs[0]->mh = ns.mh;
      	gs[0]->mx = ns.mx;
      	gs[0]->mm = ns.mm;
      	gs[0]->mn = ns.mn;
      	gs[0]->ml = ns.ml;
      	gs[0]->tn = kk;

		add_search_ring(ns, *gs[0]);
		free(gs[0]);
		free_data(gs, vr, 1, mm);
    }

    nn = start_search_tree(gd, rs, sm, fp);

    DEBUG_MODE print_message("\n");
    if (nn<=0) {
		DEBUG_MODE print_message("SEARCH_BRONCHUS: I cannot find first region!!\n");
    }
    else {
		DEBUG_MODE print_message("search_bronchus: I found %d regions!\n", nn);
    }
}



void  search_blood(MSGraph<sWord> gd, RgnStat ns, double rs, FILE* fp, char* fn)
{
    int  i, j, k, kk, nn, mm, lsize;
    RgnStat *gx[RGN_MAXNO], gz;
    RgnStat *gs[RGN_MAXNO];
    MSGraph<sWord> *vr[RGN_MAXNO], vp;
    RBound  rb;
    FILE*   fx;
    CmnHead hx;


    fx = fopen(fn,"r");
    if (fx==NULL) {
		DEBUG_MODE print_message("SEARCH_BLOOD: error.\n");
		DEBUG_MODE print_message("canot open blood starting file. bye bye.\n");
		exit(1);
    }
    fread(&hx, sizeof(CmnHead), 1, fx);

    nn = 0;
    rb.xmin = rb.ymin = rb.zmin = 0;
    fread(&gz, sizeof(RgnStat), 1, fx);
    while(!feof(fx)) {
    	ntoh_st(&gz, 4);
        lsize = gz.xs*gz.ys*sizeof(sWord);
        gz.gp = (sWord*)malloc(lsize);
        fread(gz.gp, lsize, 1, fx);
    	ntoh_ar(gz.gp, lsize);
		gz.xx = gz.xx + ntohl(hx.xsize); 
		gz.yy = gz.yy + ntohl(hx.ysize); 
		gz.zz = gz.zz + ntohl(hx.zsize); 

		gx[nn]  = (RgnStat*)malloc(sizeof(RgnStat));
		*gx[nn] = gz;

		nn++;
		if (nn>=RGN_MAXNO) {
	    	DEBUG_MODE print_message("SEARCH_BLOOD: error.\n");
	    	DEBUG_MODE print_message("reading region number is too large!!\n");
	    	exit(1);
		}
    	fread(&gz, sizeof(RgnStat), 1, fx);
    }
    fclose(fx);
    DEBUG_MODE print_message("search_blood: readed region number = %d\n",nn);

    ns.nc = 0;
    ns.bn = 0;
    ns.ip = NO;
    ns.nn = nn;

    rb.xmin = rb.ymin = rb.zmin = 0;
    Region_No = Branch_No = 0;
    kk = 0;
    for (i=0; i<nn; i++) {

		vp = look_forward(gd, *gx[i], 0.0, FTMODE, 0, rb); 
		mm = search_thd(*gx[i], vp, gs, vr, BLOOD, NORMAL);
    	for (j=0; j<mm; j++) get_vector(*gx[i], gs[j], 0.0);
		free(gx[i]->gp);
        free(gx[i]);
   		free(vp.gp);

		if (mm<=0) {
	    	DEBUG_MODE print_message("SEARCH_BLOOD: warning.\n");
	    	DEBUG_MODE print_message("miss threshulding %d !!\n",i+1);
	    	continue;
 		}
*/
/*
		mm = 1;
		gs[0] = gx[i];

		kk++;
        gs[0]->nc = ++Region_No; // Region No. 
        gs[0]->np = 0;           // Previous Region No.
        gs[0]->bn = ++Branch_No; // Branch No. 
        gs[0]->bp = 0;           // Previous Branch No. 
        gs[0]->pn = 0;	         // Previous Bunki Number 
        gs[0]->rn = 1;	         // Bunki kaisuu 
        gs[0]->lp = NO;
        gs[0]->st = YES;
        gs[0]->nr = NORMAL_RGN;
      	gs[0]->mh = ns.mh;
      	gs[0]->mx = ns.mx;
      	gs[0]->mm = ns.mm;
      	gs[0]->mn = ns.mn;
      	gs[0]->ml = ns.ml;
      	gs[0]->tn = kk;

		add_search_ring(ns, *gs[0]);
		free(gs[0]);
		free_data(gs, vr, 1, mm);
    }

    nn = start_search_tree(gd, rs, BLOOD, fp);

    DEBUG_MODE print_message("\n");
    if (nn<=0) {
		DEBUG_MODE print_message("SEARCH_BLOOD: I cannot find first region!!\n");
    }
    else {
       DEBUG_MODE print_message("search_blood: I found %d blood tree!!\n",nn);
    }
}
*/



/*
int  continus_search (MSGraph<sWord> gd, RgnStat ns, 
	double rs, int sm, FILE* fp, char* fc_name, int cn)
{

    int   lsize;
    FILE* fc;
    CmnHead  hx;


    if ((fc=fopen(fc_name,"r"))==NULL) {
		DEBUG_MODE print_message("Read Data File %s Open Error!!\n", fc_name);
   		return ERR;
    }

    fread(&hx, sizeof(CmnHead), 1, fc);
    fread(&ns, sizeof(RgnStat), 1, fc);
    while(!(feof(fc)||ns.nc==cn)) {
        lsize = ns.xs*ns.ys*sizeof(sWord);
        fseek(fc, lsize, 1);
        fread(&ns, sizeof(RgnStat), 1, fc);
    }
    if (ns.nc!=cn) {
        DEBUG_MODE print_message("Not Found Region %d !!\n",cn);
   		exit(1);
    }

    lsize = ns.xs*ns.ys*sizeof(sWord);
    ns.gp = (sWord*)malloc(lsize*sizeof(sWord));
    fread(ns.gp, lsize, 1, fc);

    No = ns.nc;
    Tn = ns.tn;
    search_tree(gd, &ns, rs, sm, fp);
    fprintln_file(fp, ns);

    fclose(fc);
    free(ns.gp);
    
    return NORM;
}
*/

 
