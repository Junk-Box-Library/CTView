#include "tools++.h"
#include "Threshold.h"



///////////////////////////////////////////////////////////////////////////////////////////////////
//

/**
int  get_region_with_thd(RgnStat ns, MSGraph<sWord> vp, RgnStat** gs, MSGraph<sWord>** vr, int sm, int cm) 

  	#define  get_region_with_thd(ns, vp, gs, vr, sm, cm)   search_thd(ns, vp, gs, vr, sm, cm)

	機能：閾値を探索し，子領域のデータを返す

			BRONCHUS: 閾値(mm)以下の領域．	ml〜mm
			BLOOD:	  閾値(mm)以上の領域．	mm〜mh
			OBJECTS:  探索閾値間の領域．    ml〜mh    get_region() と同じ動作．

	引数：
		ns: 親領域
		vp: 観測面
		sm: 探索モード（BRONCHUS, BLOOD, OBJECTS）
		cm: チェックモード

	戻値：
		int   子領域の数
		**gs: 子領域の情報
		**vr: 子領域イメージ
*/
int get_region_with_thd(RgnStat ns, MSGraph<sWord> vp, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm, int cm) 
{
    int   nn = 0; 
	//nn = get_region(ns, vp, gs, vr, sm, cm);
    if      (Xabs(sm)==BRONCHUS) nn = search_thd_bronchus(ns, vp, gs, vr, rs, cm);
    else if (Xabs(sm)==OBJECTS)  nn = get_region(ns, vp, gs, vr, rs, sm, cm);		// ns.ml〜ns.mh の領域
    else if (Xabs(sm)==BLOOD)    nn = search_thd_blood(ns, vp, gs, vr, rs, cm);

    return nn;
}



int    search_thd_blood(RgnStat ns, MSGraph<sWord> vp, RgnStat** gs, MSGraph<sWord>** vr, double rs, int cm)
{
    int nn = get_region(ns, vp, gs, vr, rs, BLOOD, cm);
	
	if (nn==RGN_ERR_CMPLX) {
		while (nn==RGN_ERR_CMPLX && ns.mm<=ns.mx) {
			//free_data(gs, vr, 0, nn);
			ns.mm = ns.mm + THD_BLOOD_VAR_RENGE;
			nn = get_region(ns, vp, gs, vr, rs, BLOOD, cm);
		}
	}
	
	else if (nn<=0) {
		while (nn<=0 && ns.mm>=ns.mn) {
			ns.mm = ns.mm - THD_BLOOD_VAR_RENGE;
			nn = get_region(ns, vp, gs, vr, rs, BLOOD, cm);
		}
	}
/*
    nn = get_region(ns, vp, gs, vr, BLOOD, cm);
	for (ss=0, fg=0.0, i=0; i<nn; i++) {
		ss = ss + (*gs)->ss;
		fg = fg + (*gs)->fg;
	}
    
	// 観測面での閾値の調整
	if (THD_BLOOD_TOO_SMALL || (fg-nn>0.5)) {				// 閾値を上げる
		ns.mm = ns.mm + THD_BLOOD_VAR_RENGE;
		while ((THD_BLOOD_TOO_SMALL || (fg-nn>0.5)) && ns.mm<=ns.mx) {	
			free_data(gs, vr, 0, nn);
		    nn = get_region(ns, vp, gs, vr, BLOOD, cm);
			for (ss=0, fg=0.0, i=0; i<nn; i++) {
				ss = ss + (*gs)->ss;
				fg = fg + (*gs)->fg;
			}
			ns.mm = ns.mm + THD_BLOOD_VAR_RENGE;
		}	
	}
	else if (THD_BLOOD_TOO_BIG) {		// 閾値を下げる
		ns.mm = ns.mm - THD_BLOOD_VAR_RENGE;
		while (THD_BLOOD_TOO_BIG && ns.mm>=ns.mn) {	
			free_data(gs, vr, 0, nn);
		    nn = get_region(ns, vp, gs, vr, BLOOD, cm);
			for (ss=0, fg=0.0, i=0; i<nn; i++) {
				ss = ss + (*gs)->ss;
				fg = fg + (*gs)->fg;
			}
			ns.mm = ns.mm - THD_BLOOD_VAR_RENGE;
		}
	}

	// 個々の領域の最適閾値の計算
	for (i=0; i<nn; i++) paint_down_thd_blood(ns, vp, &gs[i], &vr[i], 0.1);	
*/
	return  nn;
}



int    search_thd_bronchus(RgnStat ns, MSGraph<sWord> vp, RgnStat** gs, MSGraph<sWord>** vr, double rs, int cm)
{
	int  nn;
	
    nn = get_region(ns, vp, gs, vr, rs, BRONCHUS, cm);
	
	if (nn==RGN_ERR_CMPLX) {
		while (nn==RGN_ERR_CMPLX && ns.mm>=ns.mn) {
			//free_data(gs, vr, 0, nn);
			ns.mm = ns.mm - THD_BRONCHUS_VAR_RENGE;
			nn = get_region(ns, vp, gs, vr, rs, BRONCHUS, cm);
		}
	}
	
	else if (nn<=0) {
		while (nn<=0 && ns.mm<=ns.mx) {
			ns.mm = ns.mm + THD_BRONCHUS_VAR_RENGE;
			nn = get_region(ns, vp, gs, vr, rs, BRONCHUS, cm);
		}
	}
/*	
	int  i, nn, ss;
	double  fg;

    nn = get_region(ns, vp, gs, vr, BRONCHUS, cm);
	for (ss=0, fg=0.0, i=0; i<nn; i++) {
		ss = ss + (*gs)->ss;
		fg = fg + (*gs)->fg;
	}
	
	// 観測面での閾値の調整
	if (THD_BRONCHUS_TOO_SMALL) {				// 閾値を上げる
		ns.mm = ns.mm + THD_BRONCHUS_VAR_RENGE;
		while (THD_BRONCHUS_TOO_SMALL && ns.mm<=ns.mx) {	
			free_data(gs, vr, 0, nn);
		    nn = get_region(ns, vp, gs, vr, BRONCHUS, cm);
			for (ss=0, fg=0.0, i=0; i<nn; i++) {
				ss = ss + (*gs)->ss;
				fg = fg + (*gs)->fg;
			}
			ns.mm = ns.mm + THD_BRONCHUS_VAR_RENGE;
		}	
	}
	else if (THD_BRONCHUS_TOO_BIG) {		// 閾値を下げる
		ns.mm = ns.mm - THD_BRONCHUS_VAR_RENGE;
		while (THD_BRONCHUS_TOO_BIG && ns.mm>=ns.mn) {	
			free_data(gs, vr, 0, nn);
		    nn = get_region(ns, vp, gs, vr, BRONCHUS, cm);
			for (ss=0, fg=0.0, i=0; i<nn; i++) {
				ss = ss + (*gs)->ss;
				fg = fg + (*gs)->fg;
			}
			ns.mm = ns.mm - THD_BRONCHUS_VAR_RENGE;
		}
	}

	// 個々の領域の最適閾値の計算
	for (i=0; i<nn; i++) paint_up_thd_bronchus(ns, vp, &gs[i], &vr[i], 0.1);
*/

	return  nn;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//
//

/**
void   search_most_thd(RgnStat ns, MSGraph<sWord> vp, RgnStat** gs, MSGraph<sWord>** vr, int sm, double rate)

	機能：各々の領域の，最適閾値を求める．
	　　　rate で各特長値の変化率の許容率を指定する．デフォルトは 0.1

*/
void   search_most_thd(RgnStat ns, MSGraph<sWord> vp, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm, double rate)
{
    if      (Xabs(sm)==BRONCHUS) paint_up_thd_bronchus(ns, vp, gs, vr, rs, rate);
    else if (Xabs(sm)==BLOOD)    paint_down_thd_blood (ns, vp, gs, vr, rs, rate);
}





/**
void   paint_down_thd_blood(RgnStat ns, MSGraph<sWord> vp, RgnStat** gs, MSGraph<sWord>** vr, double rate)

	機能：閾値を THD_BLOOD_VAR_RENGE ずつ下げながら最適な血管の閾値を求める．
	　　　rate で各特長値の変化率の許容率を指定する．

*/
void   paint_down_thd_blood(RgnStat ns, MSGraph<sWord> vp, RgnStat** gs, MSGraph<sWord>** vr, double rs, double rate)
{
    RgnStat       *gx[RGN_MAXNO];
    MSGraph<sWord> *vx[RGN_MAXNO];

	ns.mm = ns.mm - THD_BLOOD_VAR_RENGE;
	if (ns.mm<ns.mn) return;
	int xx = (*gs)->sx + (*gs)->ux;
	int yy = (*gs)->sy + (*gs)->uy;

	int nn = get_region_paint(ns, vp, gx, vx, rs, BLOOD, ALL_OK, xx, yy);
	if (nn!=1) {
		free_data(gx, vx, 0, nn);
		return; 
	}

	double ds = (double)Xabs((*gs)->ss - gx[0]->ss);
	double dg = Xabs((*gs)->fg - gx[0]->fg);
	while (ds<(*gs)->ss*rate && dg<(*gs)->fg*rate) {
		free_data(gs, vr, 0, 0);
		*gs = gx[0];
		*vr = vx[0];
		ns.mm = ns.mm - THD_BLOOD_VAR_RENGE;
		if (ns.mm<ns.mn) return;
		
		nn = get_region_paint(ns, vp, gx, vx, rs, BLOOD, ALL_OK, xx, yy);
		if (nn!=1) {
			free_data(gx, vx, 0, nn);
			break; 
		}
		ds = (double)Xabs((*gs)->ss - gx[0]->ss);
		dg = Xabs((*gs)->fg - gx[0]->fg);
	}
	free_data(gx, vx, 0, nn);

	return;
}



/**
void   paint_up_thd_bronchus(RgnStat ns, MSGraph<sWord> vp, RgnStat** gs, MSGraph<sWord>** vr, float rate)

	機能：閾値を THD_BRONCHUS_VAR_RENGE ずつ上げながら最適な気管支の閾値を求める．
	　　　rate で各特長値の変化率の許容率を指定する．

*/
void   paint_up_thd_bronchus(RgnStat ns, MSGraph<sWord> vp, RgnStat** gs, MSGraph<sWord>** vr, double rs, double rate)
{
	RgnStat       *gx[RGN_MAXNO];
    MSGraph<sWord> *vx[RGN_MAXNO];

	ns.mm = ns.mm + THD_BRONCHUS_VAR_RENGE;
	if (ns.mm>ns.mx) return;
	int xx = (*gs)->sx + (*gs)->ux;
	int yy = (*gs)->sy + (*gs)->uy;

	int nn = get_region_paint(ns, vp, gx, vx, rs, BRONCHUS, ALL_OK, xx, yy);
	if (nn!=1) {
		free_data(gx, vx, 0, nn);
		return; 
	}

	double ds = (double)Xabs((*gs)->ss - gx[0]->ss);
	double dg = Xabs((*gs)->fg - gx[0]->fg);
	while (ds<(*gs)->ss*rate && dg<(*gs)->fg*rate) {
		free_data(gs, vr, 0, 0);
		*gs = gx[0];
		*vr = vx[0];
		ns.mm = ns.mm + THD_BRONCHUS_VAR_RENGE;
		if (ns.mm>ns.mx) return;
		
		nn = get_region_paint(ns, vp, gx, vx, rs, BRONCHUS, ALL_OK, xx, yy);
		if (nn!=1) {
			free_data(gx, vx, 0, nn);
			break; 
		}
		ds = (double)Xabs((*gs)->ss - gx[0]->ss);
		dg = Xabs((*gs)->fg - gx[0]->fg);
	}
	free_data(gx, vx, 0, nn);

	return;
}



/**
int   get_region_paint_first(RgnStat ns, MSGraph<sWord> xp, RgnStat** gs, MSGraph<sWord>** vr, int sm, int cm, double fg, int xx, int yy)

	機能：観測面 xp から塗りつぶしにより，分散度がfg以下になるよう閾値を調整して子領域をそ取り出す．
		  主に探索開始点に対して適用する．

	引数：
		ns: 親領域
		xp: 観測面
		sm: 探索モード（BRONCHUS, BLOOD, OBJECTS）
		cm: チェックモード
		fg: 分散度．指定した領域がこれ以下になるように閾値を調整する．
		xx: 塗りつぶしを行う X座標．-1を指定するとで観測面中央になる
		yy: 塗りつぶしを行う Y座標．-1を指定するとで観測面中央になる

	戻値：
		int 子領域の数（１以下）
		**gs: 子領域の情報
		**vr: 子領域イメージ

*/
int   get_region_paint_first(RgnStat ns, MSGraph<sWord> xp, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm, int cm, double fg, int xx, int yy)
{
	bool flag = false;
	int  x, y;
	
	int nn = get_region_paint(ns, xp, gs, vr, rs, sm, cm, xx, yy);
	if (nn>0) {
		DEBUG_MODE print_message("get_region_paint_first: 最初の領域発見!   fg = %f, ss = %d\n", gs[0]->fg, gs[0]->ss);
	}
	else {
		DEBUG_MODE print_message("get_region_paint_first: 領域発見できません\n"); 
	}
	
	// 周辺を探索
	if (nn<=0) {
		for (int i=-1; i<=1; i++) {
			for (int j=-1; j<=1; j++) {
				x = xx + i;
				y = yy + j;
				nn = get_region_paint(ns, xp, gs, vr, rs, sm, cm, x, y);
				if (nn>0) {
					flag = true;
					xx = x;
					yy = y;
					break;
				}
			}
			if (flag) break;
		}
		if (nn<=0) return nn;
		else  {
			DEBUG_MODE print_message("get_region_paint_first: 周辺探索により最初の領域発見!   fg = %f, ss = %d\n", gs[0]->fg, gs[0]->ss);
		}
	}
	

	flag = false;

	if (Xabs(sm)==BLOOD) {
		while (gs[0]->fg>fg) {
			free_data(gs, vr, 0, nn);
			ns.mm = ns.mm + THD_BLOOD_VAR_RENGE;
			nn = get_region_paint(ns, xp, gs, vr, rs, sm, cm, xx, yy);
			DEBUG_MODE print_message("ns.mm = %d, nn= %d\n", ns.mm, nn);
			if (nn<=0) {
				flag = true; 
				break;
			}
		}
		if (flag) {	// for break
			ns.mm = ns.mm - THD_BLOOD_VAR_RENGE;
			nn = get_region_paint(ns, xp, gs, vr, rs, sm, cm, xx, yy);
			DEBUG_MODE print_message("ns.mm = %d, nn= %d\n", ns.mm, nn);
		}
	}

	else if (Xabs(sm)==BRONCHUS) {
		while (gs[0]->fg>fg) {
			free_data(gs, vr, 0, nn);
			ns.mm = ns.mm - THD_BRONCHUS_VAR_RENGE;
			nn = get_region_paint(ns, xp, gs, vr, rs, sm, cm, xx, yy);
			if (nn<=0) {
				flag = true;
				break;
			}
		}
		if (flag) {	// for break
			ns.mm = ns.mm + THD_BRONCHUS_VAR_RENGE;
			nn = get_region_paint(ns, xp, gs, vr, rs, sm, cm, xx, yy);
		}
	}
	
	return nn;
}



/*
int    search_thd_bronchus(RgnStat ns, MSGraph<sWord> xp, RgnStat** gs, MSGraph<sWord>** vr, int cm)
{
    int   nn, mm, mn, mx, mp, mq, ng;
//    bool  of=false;

	mx = Min(ns.mx, ns.mm + 50);
    mn = Max(ns.mn, ns.mm - 50);
    mm = ns.mm;

    ng = 1;
    nn = get_region(ns, xp, gs, vr, BRONCHUS, cm);
    if (nn>=ng) return nn;
    else free_data(gs, vr, 0, nn);

    mp = mq = mm;
    do {
		mp = mp + RENGE;
		if (mp<=mx) {
	    	ns.mm = mp;
            nn = get_region(ns, xp, gs, vr, BRONCHUS, cm);
	    	if (nn>=ng) return nn;
    	    else free_data(gs, vr, 0, nn);
		}

		mq = mq - RENGE;
		if (mq>=mn) {
            ns.mm = mq;
            nn = get_region(ns, xp, gs, vr, BRONCHUS, cm);
	    	if (nn>=ng) return nn;
    	    else free_data(gs, vr, 0, nn);
		}
    } while(!(mp>=mx && mq<=mn));


    return 0;
}
*/



/*
int    search_thd_blood(RgnStat ns, MSGraph<sWord> xp, RgnStat** gs, MSGraph<sWord>** vr, int cm)
{
    int   nn, mm, mn, mx, mp, mq, ng;

    mx = Min(ns.mx, ns.mm + 200);
    mn = Max(ns.mn, ns.mm - 200);
    mx = ns.mx;
    mn = ns.mn;
    mm = ns.mm;

	ng = 1;
    nn = get_region(ns, xp, gs, vr, BLOOD, cm);
	if (nn>=ng) return nn;
    else free_data(gs, vr, 0, nn);

    mp = mq = mm;
    do {
		mq = mq - RENGE;
		if (mq>=mn) {
            ns.mm = mq;
            nn = get_region(ns, xp, gs, vr, BLOOD, cm);
	    	if (nn>=ng) return nn;
    	    else free_data(gs, vr, 0, nn);
		}

		mp = mp + RENGE;
		if (mp<=mx) {
	    	ns.mm = mp;
            nn = get_region(ns, xp, gs, vr, BLOOD, cm);
	    	if (nn>=ng) return nn;
    	    else free_data(gs, vr, 0, nn);
		}
    } while(!(mp>=mx && mq<=mn));


    return 0;
}
*/



/*
int    search_thd_blood(RgnStat ns, MSGraph<sWord> vp, 
			RgnStat** gs, MSGraph<sWord>** vr, int cm)
{
    int   i, nf, nn, nx, mm, mn, mx, ng;
    MSGraph<sWord> xp, mp;
    FMask   mask;


    mp = make_MSGraph<sWord>(vp.xs, vp.ys, 1);
    xp = make_MSGraph<sWord>(vp.xs, vp.ys, 1);
    for (i=0; i<mp.xs*mp.ys; i++) mp.gp[i] = vp.gp[i];
    for (i=0; i<vp.xs*vp.ys; i++) xp.gp[i] = EMAXCT - vp.gp[i];
    set_around_space(xp);

    mask = gauss_mask(3, 2);
    fi_mask(mp, mask);
    paint(mp, 0, 0, 0, ns.ml, UMAXCT, 4);
    for (i=0; i<mp.xs*mp.ys; i++) {
		if (mp.gp[i]==UMAXCT) xp.gp[i] = 0;
    } 

    mx = ns.mx;
    ns.mm = EMAXCT - ns.mm;
    ns.mx = EMAXCT - ns.mn;
    ns.mn = EMAXCT - mx;
    mx = ns.mh;
    ns.mh = EMAXCT - ns.ml;
    ns.ml = EMAXCT - mx;

    nn = search_thd_blood2(ns, xp, mp, gs, vr, cm);

    for (i=0; i<nn; i++) {
		mx = gs[i]->mx;
		gs[i]->mm = EMAXCT - gs[i]->mm;
		gs[i]->mx = EMAXCT - gs[i]->mn;
		gs[i]->mn = EMAXCT - mx;
		mx = gs[i]->mh;
		gs[i]->mh = EMAXCT - gs[i]->ml;
		gs[i]->ml = EMAXCT - mx;
    }
    
    free(xp.gp);
    free(mp.gp);
    return nn;
}
*/



/*

int    search_thd_blood2(RgnStat ns, MSGraph<sWord> vp, MSGraph<sWord> mp,
				RgnStat** gs, MSGraph<sWord>** vr, int cm)
{
    int    i, nf, nn, kk, nx, mn, mm, mx, ng, nw;
    double fg;
    MSGraph<sWord> wp, xp, zp;

    nn = 0;
    ng = Max(1, (int)(ns.fg+0.5));	

    mn = mx = ns.mm;
    for (i=0; i<vp.xs*vp.ys; i++) {
		if (vp.gp[i]!=0) {
	    	mx = Max(vp.gp[i], mx);
	    	mn = Min(vp.gp[i], mn);
		}
    }
    mx = Min(ns.mx, mx-100);
    mn = Max(ns.mn, mn+100);
    ns.mm = mn - 2;

    wp = make_MSGraph<sWord>(vp.xs, vp.ys, vp.zs);
    xp = make_MSGraph<sWord>(vp.xs, vp.ys, vp.zs);
    zp = make_MSGraph<sWord>(vp.xs, vp.ys, vp.zs);
    for (i=0; i<vp.xs*vp.ys; i++) zp.gp[i] = vp.gp[i];

    do {
        nx = nn;
    	for (i=0; i<vp.xs*vp.ys; i++) xp.gp[i] = wp.gp[i];

        ns.mm = ns.mm + 2;
    	kk = get_region(ns, zp, gs, vr, BRONCHUS, cm);
    	for (i=0; i<kk; i++) {
	    	paint_up_blood(ns, mp, zp, wp, gs[i], 50);
	    	paint_up_blood(ns, mp, zp, wp, gs[i], 10);
	    	paint_up_blood(ns, mp, zp, wp, gs[i], 2);
		}
		free_data(gs, vr, 0, kk);

        nn = get_region(ns, wp, gs, vr, OBJECTS, cm);
		for (fg=0.0,i=0; i<nn; i++) fg += gs[i]->fg;
		free_data(gs, vr, 0, nn);
	
    } while(!(nn>=ng || nn<nx || fg>ns.fg || ns.mm>mx));


    if (fg>ns.fg) nn = get_region(ns, wp, gs, vr, OBJECTS, cm);
    else          nn = get_region(ns, xp, gs, vr, OBJECTS, cm);

    if (DebugMode==ON) {
		int dm;
		write_file("data/X1",vp);
		write_file("data/X2",wp);
		write_file("data/X3",xp);
		fprintf(stderr,"in %d %d %d = ",ns.ml,ns.mm,mn);
		scanf("%d",&dm);
		DebugMode = OFF;
    }
	
    free(wp.gp);
    free(xp.gp);
    free(zp.gp);
    return nn;
}
  */



/*
void   paint_up_bronchus(RgnStat ns, MSGraph<sWord> vp, MSGraph<sWord> wp, RgnStat* gx)
{
//    vp: Kouho Ryouiki ha paint sareru.
 //   wp: Kouho Ryouiki nomi.

    int  i;
    int  xx, yy, ok, ss, mm, sl, ol;
    int  xsize, ysize, xlsize, ylsize;
    double   fg, fl;
    //RgnStat *gs[RGN_MAXNO];
    MSGraph<sWord>  *vr[RGN_MAXNO], xp, zp;

    zp.setup(vp.xs, vp.ys, 1, vp.zero);
    xp.setup(vp.xs, vp.ys, 1, vp.zero);
    for (i=0; i<xp.xs*xp.ys; i++) xp.gp[i] = vp.gp[i];

    xx = gx->sx + gx->ux;
    yy = gx->sy + gx->uy;
    mm = gx->mm - RENGE;
    ss = gx->ss;
    fg = gx->fg;
    ok = NG;
    xsize = gx->xs;
    ysize = gx->ys;

    do {
		mm = mm + RENGE;
		sl = ss;
		fl = fg;
		ol = ok;
		ss = 0;
		xlsize = xsize;
		ylsize = ysize;

		MSGraph_Paint(xp, xx, yy, 1, mm, UMAXCT, 8);

    	for (i=0; i<xp.xs*xp.ys; i++) {
	    	if (xp.gp[i]==UMAXCT) {
				xp.gp[i] = mm;
				zp.gp[i] = EMAXCT;
				ss++;
	    	}
	    	else zp.gp[i] = 0;
 		}
    } while(!( xp.gp[0]!=0 || mm>gx->mx  || ss>sl*1.5 ));

    gx->mm = mm - RENGE;

    for (i=0; i<xp.xs*xp.ys; i++) xp.gp[i] = vp.gp[i];

	MSGraph_Paint(xp, xx, yy, 1, gx->mm, UMAXCT, 8);

    for (i=0; i<xp.xs*xp.ys; i++) {
		if (xp.gp[i]==UMAXCT) {
	    	wp.gp[i] = gx->mm;
	    	vp.gp[i] = EMAXCT;
		}
    }

    free(xp.gp);
    free(zp.gp);
}

*/



/*
void   paint_up_blood(RgnStat ns, MSGraph<sWord> mp, 
			MSGraph<sWord> vp, MSGraph<sWord> wp, RgnStat* gx, int level)
{
//    mp: mp for paint.
 //   vp: Kouho Ryouiki ha paint sareru.
//    wp: Kouho Ryouiki nomi.

    int  i, nn, ct, toras, en;
    int  xx, yy, ok, ss, mm, sl, ol;
    int  xsize, ysize, xlsize, ylsize;
  //  int  e;
    double   fg, fl;
    RgnStat *gs;
    MSGraph<sWord> xp, zp;

    xp.setup(vp.xs, vp.ys, 1, vp.zero);
    zp.setup(vp.xs, vp.ys, 1, vp.zero);
    for (i=0; i<xp.xs*xp.ys; i++) xp.gp[i] = vp.gp[i];

    gs = (RgnStat*)malloc(sizeof(RgnStat));

    xx = gx->sx + gx->ux;
    yy = gx->sy + gx->uy;
    mm = gx->mm - level;
    ct = 0;
    ok = NG;
    en = NG;
    ss = gx->ss;
    fg = gx->fg;
    gs->nr = NG;
    xsize = gx->xs - 2;
    ysize = gx->ys - 2;

    do {
		mm = mm + level;
		sl = ss;
		fl = fg;
		ol = ok;
		ss = 0;
		xlsize = xsize;
		ylsize = ysize;

		MSGraph_Paint(xp, xx, yy, gx->ml, mm, UMAXCT, 8);

		for (i=0; i<xp.xs*xp.ys; i++) {
	    	if (xp.gp[i]==UMAXCT) {
				xp.gp[i] = mm;
				zp.gp[i] = UMAXCT;
				ss++;
	    	}
	    	else zp.gp[i] = zp.zero;
 		}

		Vector<double> vt = dgree_circle_MSGraph(zp);
		fg = vt.z;

//		fg = dgree_circle(zp, &xsize, &ysize, UMAXCT);
//		RBound<int> rb = out_around_MSGraph(MSGraph<T> vp, int x, int y, int mode=8)

		gs->ss = ss;
		gs->fg = fg;
		nn = chk_region(ns, &gs, 1, BLOOD, PAINT_UP);
		ok = gs->nr;
	
    	toras = NO;
 		MSGraph_Paint(zp, 0, 0, 0, 0, EMAXCT, 8);
    	for (i=0; i<zp.xs*zp.ys; i++) {
	    	if (zp.gp[i]==0) {
				toras = YES;
	        	ok    = NG;
				break;
	    	}
        }

    } while(!( xp.gp[0]!=0 || 
	       toras==YES || 
	       (ol==OK && ok==NG) || 
	       en==OK || 
	       mm>gx->mx  || 
	       (ol == OK && (ss>sl*1.5 ||
	                     fg-fl>0.3 ||
	                     fg >  3.0 ||                                                             
                             (ns.ss<=50&& (fg>3.0 || fg-fl>0.3)) ||                       
                             (ns.ss>50 && (/*xsize>xlsize*1.2 || ysize>ylsize*1.2 || */                           
  /*                                         ss > sl*1.5    )) )                            

	       )
           )); 
    if (ol==OK) gx->mm = mm - level;
    for (i=0; i<xp.xs*xp.ys; i++) xp.gp[i] = vp.gp[i];
 	MSGraph_Paint(xp, xx, yy, gx->ml, gx->mm, EMAXCT, 8);

    for (i=0; i<xp.xs*xp.ys; i++) {
		if (xp.gp[i]==UMAXCT) wp.gp[i] = vp.gp[i] = gx->mm;
    }

    free(xp.gp);
    free(zp.gp);
    free(gs);
}
*/
 
