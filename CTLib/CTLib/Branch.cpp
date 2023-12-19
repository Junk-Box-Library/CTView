
/**
	再帰的領域探索のための領域操作用基本プログラム





  */


#include "Branch.h"



MSGraphDisp<sWord>*	GLMPDisp = NULL;


DupBrc  dupBranch;


/*
// RingBuffer 制御変数
int 			Search_Ring_Addpt  = 0;
int				Search_Ring_Point  = 0;
int				Search_Ring_Number = 0;

RgnStat			Search_Ring_Buffer[RING_BUF_SIZE];
*/









/**
MSGraph<sWord>  pre_paint_region(RgnStat ns, MSGraph<sWord> wp, int sm, int xx, int yy)

	機能：wpの(xx,yy)を閾値 ns.mm で塗りつぶして取り出した画像を返す．
		　wpは破壊されない．

	引数：sm	モード
			取り出す領域は
			BRONCHUS: 閾値以下の領域．	ml〜mm
			BLOOD:	  閾値以上の領域．	mm〜mh
			OBJECTS:  探索閾値間の領域．ml〜mh

		  vp	目的の領域の輝度値を mm に変換した画像．その他は vp.zero．

*/
MSGraph<sWord>  pre_paint_region(RgnStat ns, MSGraph<sWord> wp, int sm, int xx, int yy)
{
	int  i;
	MSGraph<sWord>  vp;

	vp = wp;
	vp.getm(wp.xs, wp.ys, 1, wp.zero);
	if (vp.isNull()) return vp;
	vp.max = wp.max;
	vp.min = wp.min;

	sm = Xabs(sm);
	for (i=0; i<wp.xs*wp.ys; i++) vp.gp[i] = wp.gp[i];

//	GLMPDisp->displayNew(vp);

	if (xx<0) xx = (vp.xs-1)/2;
	if (yy<0) yy = (vp.ys-1)/2;

	if (sm==BRONCHUS)     MSGraph_Paint(vp, xx, yy, ns.ml, ns.mm, UMAXCT, 8);
	else if (sm==BLOOD)   MSGraph_Paint(vp, xx, yy, ns.mm, ns.mh, UMAXCT, 8);
	else if (sm==OBJECTS) MSGraph_Paint(vp, xx, yy, ns.mn, ns.mh, UMAXCT, 8);

//	GLMPDisp->displayNew(vp);

	for (i=0; i<vp.xs*vp.ys; i++){
		if (vp.gp[i]==UMAXCT) vp.gp[i] = ns.mm;
		else                  vp.gp[i] = vp.zero;
	}
	return vp;
}







/**
MSGraph<sWord>  pre_cut_region(RgnStat ns, MSGraph<sWord> wp, int sm)

	機能：wpから cut_color()を使って，目的の領域の輝度値を閾値 ns.mm に変換して取り出して返す．
		　wpは破壊されない．

	引数：sm	モード
			取り出す領域は
			BRONCHUS: 閾値以下の領域．	ml〜mm
			BLOOD:	  閾値以上の領域．	mm〜mh
			OBJECTS:  探索閾値間の領域．ml〜mh

		  vp	目的の領域の輝度値を mm に変換した画像．その他は vp.zero．

*/
MSGraph<sWord>  pre_cut_region(RgnStat ns, MSGraph<sWord> wp, int sm)
{
	int  i;
	MSGraph<sWord>  vp;

	vp = wp;
	vp.getm(wp.xs, wp.ys, 1, wp.zero);
	if (vp.isNull()) return vp;
	vp.max = wp.max;
	vp.min = wp.min;

	for (i=0; i<wp.xs*wp.ys; i++) vp.gp[i] = wp.gp[i];
	cut_color(ns, vp, sm);
	return vp;
}







/**
void  cut_color(RgnStat ns, MSGraph<sWord> vp, int sm)

	機能：観測面の縁に接しない，目的の領域の輝度値を閾値 ns.mm に変換する．その他の領域は vp.zeroになる．

	引数：sm
		BRONCHUS: 閾値以下の領域を取り出す．	ml〜mm
		BLOOD:	  閾値以上の領域を取り出す．	mm〜mh
		OBJECTS:  探索閾値間の領域を取り出す．	ml〜mh

*/
void  cut_color(RgnStat ns, MSGraph<sWord> vp, int sm)
{
	int  i;

	sm = Xabs(sm);
	if (sm==BRONCHUS){		// 閾値以下の領域を取り出す．ml〜mm
		for (i=0; i<vp.xs*vp.ys; i++){
	        if (vp.gp[i]>=ns.ml && vp.gp[i]<=ns.mm) {
				vp.gp[i] = UMAXCT;
	    	}
		}
		set_around_MSGraph(vp, UMAXCT);
		MSGraph_Paint(vp, 0, 0, UMAXCT, UMAXCT, vp.zero, 8);
		for (i=0; i<vp.xs*vp.ys; i++){
   	    	if (vp.gp[i]==UMAXCT) vp.gp[i] = ns.mm;
		    else                  vp.gp[i] = vp.zero;
		}
	}

	else if (sm==BLOOD) {	// 閾値以上の領域を取り出す．mm〜mh
	    for (i=0; i<vp.xs*vp.ys; i++){
	        if (vp.gp[i]>=ns.mm && vp.gp[i]<=ns.mh) {
				vp.gp[i] = UMAXCT;
	    	}
	    }
		set_around_MSGraph(vp, UMAXCT);
		MSGraph_Paint(vp, 0, 0, UMAXCT, UMAXCT, vp.zero, 8);
		for (i=0; i<vp.xs*vp.ys; i++){
   	    	if (vp.gp[i]==UMAXCT) vp.gp[i] = ns.mm;
		    else                  vp.gp[i] = vp.zero;
		}
	}

	else if (sm==OBJECTS) {	// 探索閾値間の領域を取り出す．	ml〜mh
	    for (i=0; i<vp.xs*vp.ys; i++){
	        if (vp.gp[i]>=ns.ml && vp.gp[i]<=ns.mh) {
				vp.gp[i] = UMAXCT;
	    	}
	    }
		set_around_MSGraph(vp, UMAXCT);
		MSGraph_Paint(vp, 0, 0, UMAXCT, UMAXCT, vp.zero, 8);
		for (i=0; i<vp.xs*vp.ys; i++){
   	    	if (vp.gp[i]==UMAXCT) vp.gp[i] = ns.mm;
		    else                  vp.gp[i] = vp.zero;
		}
	}

	return;
}






/**
int  cut_region(RgnStat ns, MSGraph<sWord> xp, MSGraph<sWord> vp, RgnStat** gs, MSGraph<sWord>** vr, int sm)

	機能：観測面 xpとその子領域を塗りつぶした画像 vpから,一つ一つの子領域の候補を取り出す．
		　xpのデータは破壊されないが，vpのデータは破壊される．
  　
	戻値：子領域の候補数．
		　-1  子領域の候補数が多すぎる > RGN_MAXNO

*/
int  cut_region(RgnStat ns, MSGraph<sWord> xp, MSGraph<sWord> vp, RgnStat** gs, MSGraph<sWord>** vr/*, int sm*/)
{
	int  i, j, k, n, ii, ij, ix, iy;
	int  x, y, cy, cx, ci, cj, ss, cm, mm, ux, uy;
	int  wx, wy, rx, ry, rr, mo;

//	MSGraph<sWord> wp;
//	copy_data_MSGraph(vp, wp);

//	DEBUG_MODE print_message("start cut_region\n");
//	sm = Xabs(sm);
	n  = 0;
	for (y=1; y<vp.ys-1; y++) {
	    cy = y*vp.xs;
	    for (x=1; x<vp.xs-1; x++) {
	        cx = cy + x;

	        if (vp.gp[cx]!=vp.zero) {
				mm = vp.gp[cx];
	        	MSGraph_Paint(vp, x, y, mm, mm, UMAXCT, 8);

				RBound<int> rb = out_around_MSGraph(vp, x, y, 8);
				gs[n]  = new RgnStat(ns);	// 親領域をコピー	
				vr[n]  = new MSGraph<sWord>(rb.xmax-rb.xmin+3, rb.ymax-rb.ymin+3, 1, vp.zero);

				gs[n]->mm = mm;
				gs[n]->zr = xp.zero;
	        	gs[n]->sx = rb.xmin - 1;
	        	gs[n]->sy = rb.ymin - 1;
	        	gs[n]->sr = rb.zmin + 0.414213562373f*rb.zmax;	// + (√2 - 1)*rb.zmax
				gs[n]->os = ns.vs;								// 観測面のサイズ
				gs[n]->xs = vr[n]->xs;
				gs[n]->ys = vr[n]->ys;
				gs[n]->vx = Max(gs[n]->xs, gs[n]->ys);
				gs[n]->gp = vr[n]->gp;


 	        	for (j=1; j<vr[n]->ys-1; j++) {
		    		cj = j*vr[n]->xs;
		    		for (i=1; i<vr[n]->xs-1; i++) {
		        		ci = cj + i;
	        	        k = (j+rb.ymin-1)*vp.xs + (i+rb.xmin-1);
	        	        if (vp.gp[k]==UMAXCT) {
							vr[n]->gp[ci] = mm;
		 	    			//if      (sm==BRONCHUS) vr[n]->gp[ci] = mm;
		 	    			//else if (sm==BLOOD)    vr[n]->gp[ci] = xp.gp[k]; 
		 	    			//else if (sm==OBJECTS)  vr[n]->gp[ci] = mm; 
	       	    			vp.gp[k] = vp.zero; 
		        		}
		    		}
	        	}


	    		ss = wx = wy = cm = 0;
 	        	for (j=0; j<gs[n]->ys; j++){
		    		cj = j*gs[n]->xs;
		    		for (i=0; i<gs[n]->xs; i++){
		        		ci = cj + i;
 		        		if (vr[n]->gp[ci]!=vr[n]->zero) {
		  	    			wx = wx + i;
			    			wy = wy + j;
			    			k = (j+rb.ymin-1)*vp.xs + (i+rb.xmin-1);
			    			cm = cm + xp.gp[k];
	    	 	    		ss++;
						}
	    	    	}
				}
 				
				MSGraph<int> wz = euclidDistance(*(vr[n]), mm, rr);
				wz.free();

				wx = wx/ss;
				wy = wy/ss;
				rx = wx + gs[n]->sx - vp.xs/2; 
				ry = wy + gs[n]->sy - vp.ys/2; 

				gs[n]->wx = wx;
				gs[n]->wy = wy;
				gs[n]->ss = ss;
				gs[n]->dg = cm/ss;
				gs[n]->rr = rr; 
				gs[n]->r2 = rx*rx + ry*ry;
				if (gs[n]->sr>=Zero_Eps) gs[n]->ra = 2.f*ss/gs[n]->sr;

				Vector<double> vt = dgree_circle_MSGraph(*vr[n]);
				gs[n]->su = vt.x;				// 周囲長
				//gs[n]->ss = (int)vt.y;		//
				gs[n]->fg = vt.z;				// 分散度（1/円形度）


				ux = uy = mo = 0;
 	        	for (j=0; j<gs[n]->ys; j++){
		    		cj = j*gs[n]->xs;
		    		for (i=0; i<gs[n]->xs; i++){
		        		ci = cj + i;
 		        		if (vr[n]->gp[ci]!=vp.zero) {
			    			ii = (wx - i)* (wx - i);
			    			ij = (wy - j)* (wy - j);
			   	 			ix = (wx - ux)*(wx - ux);
			    			iy = (wy - uy)*(wy - uy);
			    			if (ii+ij < ix+iy) {
								ux = i;
			    				uy = j;
			    			}
			    			mo = mo + (i-wx)*(i-wx) + (j-wy)*(j-wy);
						}
	    	    	}
				}
				gs[n]->ux = ux;
				gs[n]->uy = uy;
				gs[n]->mo = mo;

	        	gs[n]->ip = false;
	        	gs[n]->ib = false;
				gs[n]->st = false;
				gs[n]->sb = false;
				gs[n]->nr = true;		// 後で再チェック
				//gs[n]->at = NORMAL_RGN;
				gs[n]->vs = view_wide(gs[n]);
				gs[n]->ob = 0;

	        	n++;
				if (n==RGN_MAXNO){
					//GLMPDisp->displayNew(wp);
					//wp.free();
		    		free_data(gs, vr, 0, n-1);
					DEBUG_MODE print_message("CUT_REGION: Error: 子領域の候補数が多すぎる %d\n", RGN_MAXNO);
		    		return -1;
				}
	    	}
	    }
	}

	//wp.free();

	return n;
}










/**
void  add_search_ring(RgnStat pn, RgnStat ns)

	機能：探索用リングバッファの先頭に，親子領域のデータのコピーを加える．
	      グラフィックデータはコピーされないため，バッファに格納後，外部で
		  グラフィックデータを破棄しないこと．

	引数：
		pn : 親領域の状態
		ns : その子領域の状態



	リングバッファ                 Search_Ring_Addpt
	                                  ↓
		-------------------------------------------
		|   |   | P | C | P | C | P | C |   |   |      P: Parent Region,    C: Child Region
		-------------------------------------------
				 ↑
		Search_Ring_Pointer  

*//*
void  add_search_ring(RgnStat pn, RgnStat ns)
{
	Search_Ring_Number += 2;
	if (Search_Ring_Number>RING_BUF_SIZE) {
		DEBUG_MODE print_message("ADD_SEARCH_RING: Error: Ring Buffer is Over-Flowed!!\n");
		return;
	}
	
	Search_Ring_Buffer[Search_Ring_Addpt] = pn;
	Search_Ring_Addpt++;
	if (Search_Ring_Addpt>=RING_BUF_SIZE) Search_Ring_Addpt -= RING_BUF_SIZE;
	
	Search_Ring_Buffer[Search_Ring_Addpt] = ns;
	Search_Ring_Addpt++;
	if (Search_Ring_Addpt>=RING_BUF_SIZE) Search_Ring_Addpt -= RING_BUF_SIZE;
}

*/




/**
void  get_search_ring(RgnStat* pn, RgnStat* ns)

	機能：探索用リングバッファの後ろから，親子領域のデータのコピーを取り出す．

	引数：
		*pn : 親領域の状態
		*ns : その子領域の状態
*//*
void  get_search_ring(RgnStat* pn, RgnStat* ns)
{
	Search_Ring_Number -= 2;
	if (Search_Ring_Number<0) {
		DEBUG_MODE print_message("GET_SEARCH_RING: Error: Unknown Error in Ring Buffer!!\n");
		return;
	}

	*pn = Search_Ring_Buffer[Search_Ring_Point];
	Search_Ring_Point++;
	if (Search_Ring_Point>=RING_BUF_SIZE) Search_Ring_Point -= RING_BUF_SIZE;

	*ns = Search_Ring_Buffer[Search_Ring_Point];
	Search_Ring_Point++;
	if (Search_Ring_Point>=RING_BUF_SIZE) Search_Ring_Point -= RING_BUF_SIZE;
}
*/













/**
void  free_data(RgnStat** gs, MSGraph<sWord>** vr, int nn, int mm)

	機能：*gs[nn], *vr[nn] 〜 *gs[mm-1], *vr[mm-1] を削除する．

		nn: 始まりのインデックス
		mm: 終わりのインデックス+1

*/
void  free_data(RgnStat** gs, MSGraph<sWord>** vr, int nn, int mm)
{
	for (int i=nn; i<mm; i++) {
		vr[i]->free();
		delete(vr[i]);
		delete(gs[i]);
	}
}





/**
int  rsorting(RgnStat** gs, MSGraph<sWord>** vr, int n)

	機能：観測面の中心に近い領域から並べ替える．単純ソート．

	戻値：並べ替えた領域の数．
  
*/
int  rsorting(RgnStat** gs, MSGraph<sWord>** vr, int n)
{
	int  i, j;  
	MSGraph<sWord>* vw;
	RgnStat*        st;

	for(i=0; i<n-1; i++){
	    for(j=i+1; j<n; j++){
	        if (gs[i]->r2 > gs[j]->r2){
	        	st    = gs[i];
				gs[i] = gs[j];
				gs[j] = st;
				vw    = vr[i];
				vr[i] = vr[j];
				vr[j] = vw;
	    	}
	    }
	}
   
	return n;
}






/**
int  nsorting(RgnStat** gs, MSGraph<sWord>** vr, int n)

	機能：不正な領域を排除して，並べ替える．
	
	戻値：有効な（並べ替えた）領域の数．
	      有効な領域の数が 0で，状態の情報(at)がある場合は，それを返す．
  
*/
int    nsorting(RgnStat** gs, MSGraph<sWord>** vr, int nn)
{
	int   kk, at;
	MSGraph<sWord>** pv;
	RgnStat**        ps;

	if (nn==0) return nn; 
	at = gs[0]->at;

	kk = 0;
	ps = gs;
	pv = vr;
	for (int i=0; i<nn; i++)  {
	    if (gs[i]->nr) {
	        *(ps++) = gs[i];
	        *(pv++) = vr[i];
	        kk++;
	    }
	    else {
	        vr[i]->free();
	        delete(vr[i]);
	        delete(gs[i]);
	    }
	}

	if (kk==0 && at<0) kk = at;
	return kk;
}
 





/**
MSGraph<sWord>  fviewport(MSGraph<sWord> gd, RgnStat ns, double rs)

	機能：前方平面の観測
		ノーマル（実数計算:線形補間）モード

*/
MSGraph<sWord>  fviewport(MSGraph<sWord> gd, RgnStat ns, double rs)
{
	int    i, j, cy, cx;
	double xx, yy, zz;
	double px, py, pz;
	double cst, snt, csf, snf;
	MSGraph<sWord> vp;
	Vector<double> vx;

	vp.set(ns.vs, ns.vs, 1, gd.zero);
	vx.set(ns.ex, ns.ey, ns.ez);
	ToPola(vx, cst, snt, csf, snf);

	px = rs;
	for (j=1; j<vp.ys-1; j++){
	    cy = j*vp.xs; 
	    pz = vp.ys/2.f - j;
	    for (i=1; i<vp.xs-1; i++){
	        cx = cy + i;
	    	py = vp.xs/2.f - i;
	        xx = px*snt*csf - py*snf - pz*cst*csf + ns.xx;
	        yy = px*snt*snf + py*csf - pz*cst*snf + ns.yy;
	        zz = px*cst              + pz*snt     + ns.zz;
	    	vp.gp[cx] = (sWord)get_fPoint(gd, xx, yy, zz);
	    }
	}
	return vp;
}



/**
MSGraph<sWord>  wviewport(MSGraph<sWord> gd, RgnStat ns, double rs)

	機能：前方平面の観測
		  sWordモード
*/
MSGraph<sWord>  wviewport(MSGraph<sWord> gd, RgnStat ns, double rs)
{
	int    i, j, cy, cx;
	double xx, yy, zz;
	double px, py, pz;
	double cst, snt, csf, snf;
	MSGraph<sWord> vp;
	Vector<double> vx;

	vp.set(ns.vs, ns.vs, 1, gd.zero);
	vx.set(ns.ex, ns.ey, ns.ez);
	ToPola(vx, cst, snt, csf, snf);

	px = rs;
	for (j=1; j<vp.ys-1; j++){
	    cy = j*vp.xs; 
		pz = vp.ys/2.f - j;
	    for (i=1; i<vp.xs-1; i++){
	        cx = cy + i;
	    	py = vp.xs/2.f - i;
	        xx = px*snt*csf - py*snf - pz*cst*csf + ns.xx;
	        yy = px*snt*snf + py*csf - pz*cst*snf + ns.yy;
	        zz = px*cst              + pz*snt     + ns.zz;
	    	vp.gp[cx] = gd.get_vPoint(xx, yy, zz);
	    }
	}
	return vp;
}



/**
	MSGraph<sWord>  pviewport(MSGraph<sWord> gd, RgnStat ns, double rs, double st)

	機能：前方球面の観測
		（実数計算:線形補間）

	引数：
		rs 　球の半径
		st 　視野角（πで半球）

	                 z      x
	                 |     /
	                 |    /
	                 |   /
	                 |  /
	                 | /
	   y ------------+
*/
MSGraph<sWord>  pviewport(MSGraph<sWord> gd, RgnStat ns, double rs, double st)
{
	int    i, j, vs;
	double rr, az, ay, dt;
	double cst, snt, csf, snf;
	double csx, snx, csy, sny;
	double xx, yy, zz, th;
	double px, py, pz;
	MSGraph<sWord> map;
	Vector<double> vc;

	// vs = √(観測球の表面積) *2.0				2.0は安全ファクター
	//	  = √{(4*PI*rs*rs)/(2*PI)*st} *2.0
	//    = 2.0*rs*√(2*st)  
	//	 ≒ 3.0*rs*√(st)
	vs = (int)(3.*rs*sqrt(st)+0.5) + 4;	
	dt = st/(vs-3);

	map.set(vs, vs, 1, gd.zero);
//	map.set(ns.xs, ns.ys, 1, gd.zero);
	vc.set(ns.ex, ns.ey, ns.ez);
	ToPola(vc, cst, snt, csf, snf);


	for (j=1; j<vs-1; j++) {
	    for (i=1; i<vs-1; i++) {
	        ay  = (vs-1)/2.0 - i;
	        az  = (vs-1)/2.0 - j;
	        rr  = sqrt(ay*ay+az*az);
	        th  = dt*rr;
	    	if (th>st/2.) continue;
	        
			csx = cos(th);
	        snx = sin(th);
	        csy = 0.0;
	        sny = 1.0;
	        if (Xabs(rr)>Zero_Eps) {
	            csy = az/rr;
	            sny = ay/rr;
	        }

	    	if (snx<Sin_Tolerance) {
				csx = Sign(csx);
				snx = 0.0f;
				csy = 0.0f;
				sny = 1.0f;
	    	}

  	    	px = rs*csx;
	    	py = rs*snx*sny;
	    	pz = rs*snx*csy;
	        xx = px*snt*csf - py*snf - pz*cst*csf + ns.xx;
	        yy = px*snt*snf + py*csf - pz*cst*snf + ns.yy;
	        zz = px*cst              + pz*snt     + ns.zz;
	        if (zz>=0 && zz*gd.RZxy<=gd.zs-1.0f)  {
				map.gp[j*vs+i] = (sWord)get_fPoint(gd, xx, yy, zz);
	    	}
	    }
	}
	return map;
}



/**
MSGraph<sWord>  xviewport(MSGraph<sWord> gd, RgnStat ns, double rs, int vm, int nz)

	機能：３Ｄ観測（前方の３Ｄ画面）

	引数：
*/
MSGraph<sWord>  xviewport(MSGraph<sWord> gd, RgnStat ns, double rs, int vm, int nz)
{
	MSGraph<sWord> vp, xp;

	xp.set(ns.vs, ns.vs, nz, gd.zero);

	if (vm==FTMODE) {
		int j = 0;
		for (double r=-(xp.zs-1)/2+rs; r<=(xp.zs-1)/2+rs; r++){
	    	vp = fviewport(gd, ns, r); //* normal mode 
	        for (int i=0; i<xp.xs*xp.ys; i++) xp.gp[j++] = vp.gp[i];
		}
	}
	else if (vm==WDMODE) {
		int j = 0;
		for (double r=-(xp.zs-1)/2+rs; r<=(xp.zs-1)/2+rs; r++){
	    	vp = wviewport(gd, ns, r); //* word mode 
	        for (int i=0; i<xp.xs*xp.ys; i++) xp.gp[j++] = vp.gp[i];
		}
	}
	else if (vm==SPMODE) {
		int j = 0;
		double rr = Max(1.f, rs-(xp.zs-1)/2.f);
		for (double r=rr; r<=rs+(xp.zs-1)/2.f; r++){
	    	vp = pviewport(gd, ns, r, PI); //* spherical mode 
	        for (int i=0; i<xp.xs*xp.ys; i++) xp.gp[j++] = vp.gp[i];
		}
	}
	else {
		DEBUG_MODE print_message("XVIEWPORT: no support mode = %d\n", vm);
	}

	free(vp.gp);
	return xp;
}



/**
void   get_world_center(RgnStat ns, RgnStat *gs, double rs)

	機能：領域gsの重心の（グローバル）座標を計算する．
		  方向ベクトルは親領域のものを継承する．

 		　平面観測以外（球面観測）では使用不可
*/
void   get_world_center(RgnStat ns, RgnStat *gs, double rs)
{
	int    ax, ay;
	double px, py, pz;
	double cst, snt, csf, snf;
	Vector<double> vx; //, vn;

	vx.set(ns.ex, ns.ey, ns.ez);
	ToPola(vx, cst, snt, csf, snf);

	ax = gs->sx + gs->wx;
	ay = gs->sy + gs->wy;
	px = rs;
	py = (gs->os-1)/2.f - ax;
	pz = (gs->os-1)/2.f - ay;

	gs->xx = px*snt*csf - py*snf - pz*cst*csf + ns.xx;
	gs->yy = px*snt*snf + py*csf - pz*cst*snf + ns.yy;
	gs->zz = px*cst              + pz*snt     + ns.zz;
	
	gs->px = ns.ex;
	gs->py = ns.ey;
	gs->pz = ns.ez;

	gs->ex = ns.ex;
	gs->ey = ns.ey;
	gs->ez = ns.ez;
	
	return;
}



/**
void   get_direction_vector(RgnStat ns, RgnStat *gs)

	機能：領域の方向ベクトルを計算する

*/
void   get_direct_vector(RgnStat ns, RgnStat *gs)
{
	double  dm;
	Vector<double>  ve, vn;

	ve.set(ns.ex, ns.ey, ns.ez);
	vn.set(gs->xx-ns.xx, gs->yy-ns.yy, gs->zz-ns.zz);
	vn.normalize();

	dm = Max(1.f, gs->ra)*2.f;	// 
//	dm = Max(1.f, sqrt(gs->rr))*2.f;	//
//	dm = Max(1.f, ns.vx);		//

	ve = ve + (vn-ve)/dm;
	ve.normalize();
	
//	gs->px = ns.ex;		// この領域を観測したベクトル
//	gs->py = ns.ey;
//	gs->pz = ns.ez;
 
	gs->ex = gs->px = ve.x;
	gs->ey = gs->py = ve.y;
	gs->ez = gs->pz = ve.z;

	return;
}



/**
int  view_wide(RgnStat *n)

	機能：領域の情報から，次の領域（子領域）を探すための観測面の大きさを計算する．

*/
int  view_wide(RgnStat *n)
{
	double fg;
	int    vs;

	fg = 2.0*sqrt(Max(n->fg, 1.0)); 
	vs = (int)(fg*n->vx + 2);

	return vs;
}



/**
void   Local2World_RS(MSGraph<sWord> gx, RgnStat ns, int mode)

	機能：RgnStat ns の画像データをグローバルな領域 gxに埋め込む
	　　　埋め込み先は，計算点の周りの全ての格子点

	引数：
		  mode	0 : ns.gp[]の値で埋める．	
				1 : 対象領域をクリアする(gx.zero)
				2 : EMAXCTで埋める．
				3 : UMAXCTで埋める．
				4 : get_color_from_bn()での計算値で埋める．
				5 : get_color_from_ns()での計算値で埋める．


*/
void   Local2World_RS(MSGraph<sWord> gx, RgnStat ns, int mode)
{
	int    x, y, cx, cy, set;
	double py, pz, xx, yy, zz;
	double cst, snt, csf, snf;
	Vector<double> vx;

	vx.set(ns.px, ns.py, ns.pz);
	ToPola(vx, cst, snt, csf, snf);

	for (y=0; y<ns.ys; y++) {
	    cy = y*ns.xs;
	    for (x=0; x<ns.xs; x++) {
	        cx = cy + x;
	        if (ns.gp[cx]!=ns.zr) {
	            py = (double)(ns.wx - x);
	            pz = (double)(ns.wy - y);
	            xx = - py*snf - pz*cst*csf + ns.xx;
	            yy =   py*csf - pz*cst*snf + ns.yy;
	            zz =            pz*snt     + ns.zz;
		
	            if (mode==1)      set = gx.zero;
	            else if (mode==2) set = EMAXCT;
	            else if (mode==3) set = UMAXCT;
				else if (mode==4) set = get_color_from_bn(ns.bn);
	            else              set = ns.gp[cx];
	            gx.put_vPoint(xx, yy, zz, set, ON);
	        }
	    }
	}
}



/**
void   bi_paint(MSGraph<sWord> fvp, MSGraph<sWord> bvp, int x, int y, int mn, int mx, int c, int m, int id)

	機能：前面画像 fvp と背面画像 bvp を繋がった画像として塗りつぶしを行う．

		id: FORWARD  座標は前面画像のもの
			BACKWARD 座標は背面画像のもの

*/
void   bi_paint(MSGraph<sWord> fvp, MSGraph<sWord> bvp, int x, int y, int mn, int mx, int c, int m, int id)
{
	int  i, j, k, xs, cc;
	double r, s;
	MSGraph<sWord> *vp[2];


	if (c<=mx && c>=mn) {
		DEBUG_MODE print_message("BI_PAINT: c = %d. Not be %d< c <%d\n", c, mn, mx); 
	    return;
	}
	if (id!=FORWARD && id!=BACKWARD) {
		DEBUG_MODE print_message("BI_PAINT: Invarid direction. Forward(0) or Backward(1) ->%d\n",id); 
	    return;
	}

	vp[0] = &fvp;
	vp[1] = &bvp;

	xs = fvp.xs;
	r  = (xs-3)/2.f;		// (xs-3-2)/2.;
	s  = (xs-1)/2.f;		// (xs-3+2)/2.;
	cc = vp[id]->gp[y*xs+x];
	if (cc>mx || cc<mn) return;

	if (x>0) {
		while(x>vp[id]->zero && vp[id]->gp[y*xs+x-1]<=mx && vp[id]->gp[y*xs+x-1]>=mn) x--; 
	}
	k = x;

	while(k<xs && vp[id]->gp[y*xs+k]<=mx && vp[id]->gp[y*xs+k]>=mn) {
	    vp[id]->gp[y*xs+k] = c; 
	    k++;
	}
	k--;

	if ((x-s)*(x-s)+(y-s)*(y-s)>=r*r) {
		bi_paint(fvp, bvp, xs-1-x, y, mn, mx, c, m, 1-id);
	}
	if ((k-s)*(k-s)+(y-s)*(y-s)>=r*r) {
		bi_paint(fvp, bvp, xs-1-k, y, mn, mx, c, m, 1-id);
	}

	for (i=x; i<=k; i++){ 
	    if (y-1>=0 && y-1<xs){ 
	        j = (y-1)*xs+i;
	        if (vp[id]->gp[j]<=mx && vp[id]->gp[j]>=mn) {
				bi_paint(fvp, bvp, i, y-1, mn, mx, c, m, id);
	    	}

	        if (Xabs(m)==8) {  /* 8-neighborhood */ 
	    		if (i-1>=0 && vp[id]->gp[j-1]<=mx && vp[id]->gp[j-1]>=mn) 
		    					bi_paint(fvp, bvp, i-1, y-1, mn, mx, c, m, id);
	        	if (i+1<xs && vp[id]->gp[j+1]<=mx && vp[id]->gp[j+1]>=mn) 
		    					bi_paint(fvp, bvp, i+1, y-1, mn, mx, c, m, id);
	    	}
		}

	    if (y+1>=0 && y+1<xs){
	        j = (y+1)*xs+i;
	        if (vp[id]->gp[j]<=mx && vp[id]->gp[j]>=mn) {
				bi_paint(fvp, bvp, i, y+1, mn, mx, c, m, id);
	    	}

	        if (Xabs(m)==8) {  /* 8-neighborhood */ 
	        	if (i-1>=0 && vp[id]->gp[j-1]<=mx && vp[id]->gp[j-1]>=mn) 
		    				bi_paint(fvp, bvp, i-1, y+1, mn, mx, c, m, id);
	        	if (i+1<xs && vp[id]->gp[j+1]<=mx && vp[id]->gp[j+1]>=mn) 
		    				bi_paint(fvp, bvp, i+1, y+1, mn, mx, c, m, id);
	    	}
 		}
	}
	return;
}



/**

  */
void   println_RgnStat(RgnStat ns)
{
	char fmt[] = "(%6.2f %6.2f %6.2f) (%5.2f %5.2f %5.2f)  [ %4.2f %4d %5d ]  %3d->%3d->%3d   %3d\n";
	DEBUG_MODE print_message(fmt, ns.xx, ns.yy, ns.zz, ns.ex, ns.ey, ns.ez, ns.fg, ns.ss, ns.mm, ns.bp, ns.bn, ns.ob, ns.rn);
}



/**
	ns->gp のワールド座標における境界と別の境界 rbを結合し，それを返す．

  */
RBound<int>  check_rgn_world(RgnStat ns, RBound<int> rb)
{
	int    x, y, cx, cy;
	double py, pz, xx, yy, zz;
	double cst, snt, csf, snf;
	Vector<double> vx;

	vx.set(ns.px, ns.py, ns.pz);
	ToPola(vx, cst, snt, csf, snf);

	for (y=0; y<ns.ys; y++) {
	    cy = y*ns.xs;
	    for (x=0; x<ns.xs; x++) {
	        cx = cy + x;
	        if (ns.gp[cx]!=ns.zr) {
	            py = (double)(ns.wx - x);
	            pz = (double)(ns.wy - y);
	            xx = - py*snf - pz*cst*csf + ns.xx;
	            yy =   py*csf - pz*cst*snf + ns.yy;
	            zz =            pz*snt     + ns.zz;
		
	            rb.xmax = Max((int)(xx)+1, rb.xmax);
	            rb.ymax = Max((int)(yy)+1, rb.ymax);
	            rb.zmax = Max((int)(zz)+1, rb.zmax);
	            rb.xmin = Min((int)(xx),   rb.xmin);
	            rb.ymin = Min((int)(yy),   rb.ymin);
	            rb.zmin = Min((int)(zz),   rb.zmin);
	        }
	    }
	}

	return  rb;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////
// RgnData のメソッド
//

/**
BrcData*  RgnData::make_branch()

	機能：領域の集合クラス RgnData から 枝の集合クラス BrcData を生成する．

  */
BrcData*  RgnData::make_branch()
{
	int  bmax = 0;
	int  bn, i, j;

	Branch  *branch;
	BrcData *bd;
	Vector<double>* vc;

	bd = new BrcData();
	if (bd==NULL) return bd;
	for (bn=0, i=0; i<no; i++) {
		if (rs[i].sb) bn++;
	}
	branch = new Branch[bn]();
	if (branch==NULL) {
		print_message("RgnData::make_branch(): もっとメモリを!!\n");
		return bd;
	}


	// 枝の開始点領域を探す．
   for (bn=0, i=0; i<no; i++) {
	    if (rs[i].sb) {
//			branch[bn].name = 
	        branch[bn].kd = JBXL_NONE;
	        branch[bn].rl = JBXL_NONE;

	        branch[bn].id = i;			// ID．枝の開始点の領域が格納されている RgnData.rs[]のインデックス
//			branch[bn].pd = -1;			// 親枝が格納されている BrcData.br[]のインデックス．木の根っこの場合は-1．
	        branch[bn].gn = rs[i].gn;	// グループ番号 
	        branch[bn].tn = rs[i].tn;	// 木番号 
	        branch[bn].bn = rs[i].bn;	// 枝番号 
	        branch[bn].bp = rs[i].bp;	// 親枝の番号 
	        branch[bn].ob = rs[i].ob;	// LOOP時の次の枝の番号 
	        branch[bn].rn = rs[i].rn;	// 分岐の深さ
	        branch[bn].pn = rs[i].pn;	// 親の分岐数（姉妹枝の数） 
	        branch[bn].ss = rs[i].ss;
			branch[bn].fg = rs[i].fg;


			// 補間領域（補間領域の場合は連続性を優先して，前後の領域を始点・終点とする）
   			if (rs[i].ib/*at==RGN_INTER*/) {
				bool find = false;
				for (j=0; j<no; j++) {
					if (rs[i].np==rs[j].nc && rs[i].gn==rs[j].gn) {
						find = true;
						break;
					}
				}
				if (find) vc = new Vector<double>(rs[j].xx, rs[j].yy, rs[j].zz);
				else      vc = new Vector<double>(rs[i].xx, rs[i].yy, rs[i].zz);
			}
			else vc = new Vector<double>(rs[i].xx, rs[i].yy, rs[i].zz);


			branch[bn].vs = *vc;
			branch[bn].lv->push_back(vc);
			branch[bn].vn = 1;

			bmax = Max(bmax, rs[i].bn);
			bn++;
	    }
	}


	// 枝の中間領域をリストしながら終端を探す．
	for (i=0; i<bn; i++) { 
		// 中間領域のリストアップ
		int id = next(branch[i].id);
		int od = id;
		if (id>=0) {
			while (rs[id].at==RGN_NORMAL || rs[id].at==RGN_INTER) {
				if (rs[id].at==RGN_NORMAL) {
					branch[i].lv->push_back(new Vector<double>(rs[id].xx, rs[id].yy, rs[id].zz));
					branch[i].vn++;		// push_back する度に１増加．
				}
				od = id;
				id = next(od);
				if (id<0) break;
			}
		}
		if (id<0) {
			DEBUG_MODE print_message("BRANCH_LINK: 警告．途中で領域を見失った．%d -> %d\n", od, id);
			DEBUG_MODE print_message("BRANCH_LINK:       nc=%d, nn=%d, nx[0]=%d, at = %d\n", rs[od].nc, rs[od].nn, rs[od].nx[0], rs[od].at);
			delete [] branch;
			delete bd;
			return NULL;
		}

		// 終点の領域
		if (rs[id].at==RGN_END)			  branch[i].at = BRC_END;
		else if (rs[id].ib)               branch[i].at = BRC_INTER;		// && rs[id].at==RGN_CONTINUE
		else if (rs[id].at==RGN_CONTINUE) branch[i].at = BRC_NORMAL;
		else if (rs[id].at==RGN_FORK)	  branch[i].at = BRC_NORMAL;
		else if (rs[id].at==RGN_DUMMY)	  branch[i].at = BRC_DUMMY;
		else if (rs[id].at==RGN_OVLP_PNT) branch[i].at = BRC_LOOP;
		else {
			DEBUG_MODE print_message("BRANCH_LINK: エラー．不正な領域属性．id = %d, at = %d\n", branch[i].id, rs[id].at);
			delete [] branch;
			delete bd;
			return NULL;
		}

	    branch[i].nn = rs[id].nn;
	    branch[i].se = rs[id].ss;
	    branch[i].fe = rs[id].fg;


		// 補間領域（補間領域の場合は連続性を優先して，前後の領域を始点・終点とする）
		if (branch[i].at==BRC_INTER) {
			bool find = false;
			for (j=0; j<no; j++) {
				if (rs[id].nc==rs[j].np && rs[id].gn==rs[j].gn) {
					find = true;
					break;
				}
			}
			if (find) vc = new Vector<double>(rs[ j].xx, rs[ j].yy, rs[ j].zz);
			else      vc = new Vector<double>(rs[id].xx, rs[id].yy, rs[id].zz);
		}
		else vc = new Vector<double>(rs[id].xx, rs[id].yy, rs[id].zz);
		
		
		branch[i].ve = *vc;
		branch[i].lv->push_back(vc);
		branch[i].vn++;
	}


	// 枝間のリンク情報（親へのリンクpd）を生成
	for (i=0; i<bn; i++) { 
		if (branch[i].bp==0) {
			branch[i].pd = -1;		// 木の根
		}
		else {
			for (int j=0; j<bn; j++) {
				if (branch[i].bp==branch[j].bn && branch[i].gn==branch[j].gn) {
					branch[i].pd = j;
					break;
				}
			}
			if (branch[i].pd==-1) {
				DEBUG_MODE print_message("BRANCH_LINK: 警告．親枝が見つからない？ %d -> %d\n", branch[i].bn, branch[i].bp);
			}
		}
	}



	// ループ領域を枝として登録．
	for (i=0; i<bn; i++) {
		int nx = -1;
	    if (branch[i].at==BRC_LOOP) {
			branch[i].clear_vector();
	        
			vc = new Vector<double>(branch[branch[i].pd].ve);
			branch[i].vs = *vc;
			branch[i].lv->push_back(vc);
			branch[i].vn++;

			for (int j=0; j<bn; j++) {
				if (branch[i].ob==branch[j].bn && branch[i].gn==branch[j].gn) {
					nx = j;
					break;
				}
			}
			vc = new Vector<double>(branch[nx].ve);
			branch[i].ve = *vc;
			branch[i].lv->push_back(vc);
			branch[i].vn++;
	   }
	}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
	// 枝の間を埋める．

	// 枝の開始点領域を探す．
/*	bmax++;
	for (in=0, i=0; i<bn; i++) {
		int pd = branch[i].pd;
	    if (pd!=-1 && branch[i].at!=BRC_DUMMY && branch[i].at!=BRC_LOOP) {
			//DEBUG_MODE print_message("at = %d -> %d\n", branch[i].at, branch[pd].at);
//			branch[bn+in].name = 
	        branch[bn+in].kd = NONE;
	        branch[bn+in].rl = NONE;

	        branch[bn+in].id = end(branch[pd].id);				// ID．枝の開始点の領域が格納されている RgnData.rs[]のインデックス
			branch[bn+in].pd = pd;				// 親枝が格納されている BrcData.br[]のインデックス．木の根っこの場合は-1．
	        branch[bn+in].gn = branch[pd].gn;	// グループ番号 
	        branch[bn+in].tn = branch[pd].tn;	// 木番号 
	        branch[bn+in].bn = bmax;			// 枝番号 
	        branch[bn+in].bp = branch[pd].bn;	// 親枝の番号 
	        branch[bn+in].rn = branch[pd].rn;	// 分岐の深さ
	        branch[bn+in].pn = branch[pd].nn;	// 親の分岐数（姉妹枝の数） 
			branch[bn+in].nn = 1;
	        branch[bn+in].ss = branch[pd].ss;
	        branch[bn+in].se = branch[ i].se;
			branch[bn+in].fg = branch[pd].fg;
			branch[bn+in].fe = branch[ i].fe;
	        
			vc = new Vector<double>(branch[pd].ve);
			branch[bn+in].vs = *vc;
			branch[bn+in].lv->push_back(vc);

			vc = new Vector<double>(branch[i].vs);
			branch[bn+in].ve = *vc;
			branch[bn+in].lv->push_back(vc);
			branch[bn+in].vn = 2;

			branch[bn+in].at = BRC_INTER;
			if (branch[i].at==BRC_LOOP) branch[bn+in].at = BRC_LOOP;

			branch[i].bp = bmax;
			branch[i].pn = 1;
			branch[i].pd = bn + in;

			bmax++;
			in++;
	    }
	}


	// 枝のリンク情報を整理
	for (i=0; i<bn+in; i++) { 
		if (branch[i].bp==0) {
			branch[i].pd = -1;		// 木の根
		}
		else {
			for (int j=0; j<bn; j++) {
				if (branch[i].bp==branch[j].bn && branch[i].gn==branch[j].gn) {
					branch[i].pd = j;
					break;
				}
			}
			if (branch[i].pd==-1) {
				DEBUG_MODE print_message("BRANCH_LINK: 警告．親枝が見つからない？ %d -> %d\n", branch[i].bn, branch[i].bp);
			}
		}
	}
*/


//    bd->no = bn+in;
	bd->no = bn;
	bd->rb = rb;
	bd->br = branch;
	return  bd;
}



/**
int  RgnData::end(int id)

	機能：RgnData中の領域データ rs[id] を含む枝の最後の領域の id を返す．
	　	　n = RgnData::end(id); の場合，rs[id]を含む枝の最後の領域がrs[n]．
		　rs[id]には枝の最初の部分を指定する場合が多い．
	      枝の領域が１つしかない場合は，idと同じ値が帰る．つまり
		  自分自身が最後のの領域となる．

  　戻り値： 0 以上　枝の最後の領域の ID
	        -1       領域が見つからなかった．
			-2       領域の属性が不正．
*/
int  RgnData::end(int id)
{
	int  nd=-1;

	// １領域＝１枝
	if (rs[id].at==RGN_FORK || rs[id].at==RGN_DUMMY || rs[id].at==RGN_OVLP_PNT || rs[id].at==RGN_CONTINUE) {
		return id;
	}
	
	for (int i=0; i<no; i++) {
		if (rs[id].bn==rs[i].bn && rs[id].gn==rs[i].gn) {
			int at = rs[i].at;
	    	if (at==RGN_END || at==RGN_FORK || at==RGN_CONTINUE) {
				nd = i;
				break;
	    	}
			else if (at==RGN_NORMAL) {
				continue;
			}
			else {
				DEBUG_MODE print_message("FIND_BRANCH_END: 不正な領域属性．at = %d\n", at);
				return -2;
			}
		}
	}

	return nd;
}



/**
int  RgnData::next(int id)

	機能：RgnData中の領域データ rs[id] の次の領域の id を返す．
	　	　n = RgnData::end(id); の場合，rs[id]の次の領域はrs[n]．
	      枝の領域が１つしかない場合は，idと同じ値が帰る．つまり
		  自分自身が次の領域となる．

  　戻り値： 0 以上　次の領域の ID
	        -1       領域が見つからなかった．
			-2       領域の属性が不正．
*/
int  RgnData::next(int id)
{
	int  nd = -1;

	// １領域＝１枝
	if (rs[id].at==RGN_FORK || rs[id].at==RGN_DUMMY || rs[id].at==RGN_OVLP_PNT || rs[id].at==RGN_CONTINUE) {
		return id;
	}

	for (int i=0; i<no; i++) {
		if (rs[id].bn==rs[i].bn && rs[id].nc==rs[i].np && rs[id].gn==rs[i].gn) {
			int at = rs[i].at;
	    	if (at==RGN_END || at==RGN_FORK || at==RGN_CONTINUE || at==RGN_NORMAL || at==RGN_INTER) {
				nd = i;
				break;
	    	}
			else {
				DEBUG_MODE print_message("FIND_BRANCH_NEXT: 不正な領域属性．at = %d\n", at);
				return -2;
			}
		}
	}

	return nd;
}



/**
void  RgnData::well_formed()

	機能：トポロジー（各領域のリンク情報）的に完全な状態に修正する．
		　抽出した領域をその場でファイルに書き込む（write_rgn_data()）場合
		　どうしてもリンクが崩れてしまう場合があるので，それを修正する．
  */
void  RgnData::well_formed()
{
	int  i, j, k;

	// 子領域(nx[])が親として自分を指してくれていないものをチェック．（削除）
	for (i=0; i<no; i++) {
		int en = 0;
		for (j=0; j<rs[i].nn; j++) {
			int nc = rs[i].nx[j];								// 領域の子領域へのポインタ rs[i].nx[j] から
			bool find = false;
			for (k=0; k<no; k++) {							
				if (nc==rs[k].nc && rs[i].gn==rs[k].gn) {		// 実際の子領域 rs[k] を探す
					find = true;								// 見つけた．
					if (rs[k].np!=rs[i].nc) {					// rs[k]が元の親を指していない場合はマークする．
						rs[i].nx[j] = -1;
						en++;
					}
					break;
				}
			}
			if (!find) rs[i].nx[j] = -1;	// そもそも子領域が存在しない
		}

		// マークしたものを削除
		int t = 0;
		int f;
		for (f=0; f<rs[i].nn; f++) {
			if (rs[i].nx[f]!=-1) rs[i].nx[t++] = rs[i].nx[f];
		}
		for (f=t; f<rs[i].nn; f++) rs[i].nx[f] = 0;
		rs[i].nn -= en;
	}


	// 親領域が自分を指してくれないものをチェック（追加）
	for (i=0; i<no; i++) {
		int np = rs[i].np;										// 領域 rs[i] に対して
		for (j=0; j<rs[i].nn; j++) {
			if (np==rs[j].nc && rs[i].gn==rs[j].gn) {			// その親領域 rs[j] を捜す．
				bool find = false;
				for (k=0; k<rs[j].nn; k++) {					// rs[j]の子領域をチェック
					if (rs[i].nc==rs[j].nx[k]) {
						find = true;							// 親は自分を指していた．
						break;
					}
				}
				if (!find) rs[j].nx[rs[j].nn++] = rs[i].nc;		// 親へ自分を追加．
			}
		}
	}
}



/**
void  RgnData::well_form_check()

	機能：トポロジー（各領域のリンク情報）的に完全な状態かどうか検査する．
		　抽出した領域をその場でファイルに書き込む（write_rgn_data()）場合
		　どうしてもリンクが崩れてしまう場合があるので，それをチェックする．
 
	戻り値:	true:  トポロジー的に完全
			false: リンクが不完全なものがある．

	注：この関数はデバッグ用．通常は well_formed() のみで十分．

  */
bool  RgnData::well_form_check()
{
	int  i, j, k;
	bool ret = true;

	// 子領域(nx[])が親として自分を指してくれていないものをチェック．（削除）
	for (i=0; i<no; i++) {
		for (j=0; j<rs[i].nn; j++) {
			int nc = rs[i].nx[j];								// 領域の子領域へのポインタ rs[i].nx[j] から
			bool find = false;
			for (k=0; k<no; k++) {							
				if (nc==rs[k].nc && rs[i].gn==rs[k].gn) {		// 実際の子領域 rs[k] を探す
					find = true;								// 見つけた．
					if (rs[k].np!=rs[i].nc) {					// rs[k]が元の親を指していない場合はマークする．
						DEBUG_MODE print_message("RGNDATA::WELL_FORM_CHECK: 子が自分を指してくれない! %d -> %d -> %d\n", rs[i].nc, nc, rs[k].np);
						ret = false;
					}
					break;
				}
			}
			if (!find && nc!=0) {
				DEBUG_MODE print_message("RGNDATA::WELL_FORM_CHECK: 子領域が存在しない %d -> %d  at = %d\n", rs[i].nc, nc, rs[i].at);
				ret = false;
			}
		}
	}


	// 親領域が自分を指してくれないものをチェック（追加）
	for (i=0; i<no; i++) {
		int np = rs[i].np;										// 領域 rs[i] に対して
		for (j=0; j<rs[i].nn; j++) {
			if (np==rs[j].nc && rs[i].gn==rs[j].gn) {			// その親領域 rs[j] を捜す．
				bool find = false;
				for (k=0; k<rs[j].nn; k++) {					// rs[j]の子領域をチェック
					if (rs[i].nc==rs[j].nx[k]) {				// 親は自分を指していた．
						find = true;	
						break;
					}
				}
				if (!find) {
					DEBUG_MODE print_message("RGNDATA::WELL_FORM_CHECK: 親が自分を指してくれない %d -> %d\n", rs[j].nc, rs[i].nc);
					ret = false;
				}
			}
		}
	}

	return ret;
}



/**


  */
void  RgnData::find_loop_point()
{
	int  i, j;

	for (i=0; i<no; i++) {
		if (rs[i].at==RGN_OVLP_PNT) {
			bool find = false;
			DEBUG_MODE print_message("重複ポイントを発見!! 　　 %d -> %d -> %d\n", rs[i].bp, rs[i].bn, rs[i].ob);
			
			if (rs[i].bp!=rs[i].ob) {		// 単枝ループは考慮しない
				for (j=0; j<no; j++) {
					if (rs[j].at==RGN_OVLP_PNT && rs[i].bp==rs[j].ob && rs[i].ob==rs[j].bp && rs[i].gn==rs[j].gn) {
						find = true;
						rs[j].at = RGN_DUMMY;
						DEBUG_MODE print_message("対の重複ポイントを発見!!  %d -> %d -> %d\n", rs[j].bp, rs[j].bn, rs[j].ob);
						//break;
					}
				}
			}

			if (!find) rs[i].at = RGN_DUMMY;
		}
	}
/**/
}
				


/**
		mode : 画像データをワールド座標系に展開する場合の輝度値の指定方法．
				0 : ns.gp[]の値で埋める．	
				1 : 対象領域をクリアする(gx.zero)
				2 : EMAXCTで埋める．
				3 : UMAXCTで埋める．
				4 : get_color_from_bn()での計算値で埋める．デフォルト．
*/

MSGraph<sWord>  RgnData::make_MSGraph(int mode)
{
	MSGraph<sWord> ms;

	if (no>0) {
		ms.set(rb, rs[0].zr);
		for (int i=0; i<no; i++) {
			if (rs[i].at!=RGN_DUMMY && rs[i].at!=RGN_OVLP_PNT) {
				Local2World_RS(ms, rs[i], mode);
			}
		}
	}
	return  ms;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//
void  DupBrc::set(int n) 
{
	if (n<=0) return;

	bool find = false;
	for (int i=0; i<no; i++) {
		if (bn[i]==n) {
			mg[i]++;
			find = true;
			break;
		}
	}
	if (!find) {
		if (no+1>=DUPRGN_MAXNO) {
			DEBUG_MODE print_message("DupBrc::set()  重複の枝が多すぎる\n");
			return;
		}
		bn[no] = n;
		mg[no] = 1;
		no++;
	}
}



int  DupBrc::get_maxbrc() 
{
	int mx=0, n=0;
	for (int i=0; i<no; i++) {
		if (mg[i]>mx) {
			mx = mg[i];
			n  = bn[i];
		}
	}
	return n;
}
	
 
