
/**
	CT アプリケーションライブラリ ヘッダ

*/


#include  "CT.h"
#include  "CT_App_Lib.h"


using namespace jbxl;


int  Stack_Paint3D = 0;
int  Stack_Max_Paint3D = 0;
int  Stack_MaxLimit_Paint3D  = STACK_MAX_PAINT3D;

int  Volume_Paint3D = 0;
int  Volume_MaxLimit_Paint3D = 0;


bool Stop_Paint3D  = false;
bool Error_Paint3D = false;



int   andPaint3D(MSGraph<sWord> vp, int x, int y, int z, int mn, int mx, int v, int w, int mode, RBound<int>* rbound)
{
	int cc, num = 0;
	RBound<int> rb;

    Stack_Paint3D = 0;
	Stack_Max_Paint3D = 0;
	Volume_Paint3D = 0;
	Stop_Paint3D  = false;
	Error_Paint3D = false;

	if (rbound==NULL) rbound = &rb;
	rbound->set(vp.xs-1, 0, vp.ys-1, 0, vp.zs-1, 0);
		
	cc = vp.point(x, y, z);
	if (cc>=mn && cc<=mx) {
		num = continuePaint3D(vp, x, y, z, mn, mx, v, w, mode, rbound);
	}

	return num;
}



int   orPaint3D(MSGraph<sWord> vp, int x, int y, int z, int mn, int mx, int v, int w, int mode, RBound<int>* rbound)
{
	int  cc, num = 0;
	RBound<int> rb;


	Stack_Paint3D = 0;
	Stack_Max_Paint3D = 0;
	Volume_Paint3D = 0;
	Stop_Paint3D  = false;
	Error_Paint3D = false;

	if (rbound==NULL) rbound = &rb;
	rbound->set(vp.xs-1, 0, vp.ys-1, 0, vp.zs-1, 0);

	cc = vp.point(x, y, z);
	if (cc>=mn && cc<=mx) {
		num = variablePaint3D(vp, x, y, z, mn, mx, v, w, mode, rbound);
	}

	return num;
}



/**
MSGraph<sWord>*  RegionGrowing(MSGraph<sWord> vp, Vector<int> pnt, Vector<int> thd, int method, int mode)

  リージョングローイング

  	機能：vp の (x, y, z)座標から塗りつぶし探索をはじめて，輝度値 mn(thd.x) 〜 mx(thd.y) +αの点の集まりを返す．
	　　　探索モードによって v(thd.z) の情報を付加する．

	引数：method 1: 閾値優先モード (AND モード)
					輝度値 mn(thd.x) 〜 mx(thd.y) の範囲で，かつ隣接点との輝度値の差が v(thd.z) 以下の
					点を塗りつぶす．
					隣接点への探索条件は，cx<=cc+v && cx>=cc-v && cx<=mx && cx>=mn　
							cc: 現点の輝度値．cx: 隣接点の輝度値
					使い方は，mn〜mxは適当に大きく取って，vで微調整する．
					v>=mx-mn で通常のリージョングローイングになる．

				 2: 可変値優先モード（OR モード）
					輝度値 mn(thd.x) 〜 mx(thd.y) の範囲，または隣接点との輝度値の差が v(thd.z) 以下の
					点を塗りつぶす．
					隣接点への探索条件は，(cx<cc+v && cx>cc-v) || (cx<=mx && cx>=mn)
						cc: 現点の輝度値．cx: 隣接点の輝度値
					使い方は，mn〜mxは必要最低限の大きさに取って，vで微調整する．
					v==0 で通常のリージョングローイングになる．

		  mode	26: 26近傍
				18: XY平面 8近傍，Z方向 5近傍x2
	　　		その他: 6近傍  XY平面 4近傍，Z方向1近傍x2

	注意：輝度値にマイナス または UMAXCT以上のものがあると誤作動する．
	      作業領域として UMAXCT〜MAXCT を使用する．
*/
MSGraph<sWord>*  RegionGrowing(MSGraph<sWord> vp, Vector<int> pnt, Vector<int> thd, int method, int mode, RBound<int>* rbound, int* size, int ecnt)
{
	int   num=0;
	MSGraph<sWord>* xp = NULL;
	CVCounter* counter = NULL;
	bool  cancel = false;

    Stack_Paint3D = 0;
	Stack_Max_Paint3D = 0;
	Error_Paint3D = false;
	Stop_Paint3D  = false;

	if (rbound!=NULL) rbound->set(vp.xs-1, 0, vp.ys-1, 0, vp.zs-1, 0);

	if (method==AND_PAINT)      num = continuePaint3D(vp, pnt.x, pnt.y, pnt.z, thd.x, thd.y, thd.z, thd.z, mode, rbound);
	else if (method==OR_PAINT)  num = variablePaint3D(vp, pnt.x, pnt.y, pnt.z, thd.x, thd.y, thd.z, thd.z, mode, rbound);
	else {
		if (rbound!=NULL) rbound->init();
		if (size!=NULL) *size =	0;
		return xp;
	}
	if (size!=NULL) *size = num;


 	// カウンタ
	if (ecnt==TRUE) counter = GetUsableGlobalCounter();
	if (counter!=NULL) {
		if (counter->isCanceled()) cancel = true;
	}

	// 切り出して，元の画素値に戻す．
	xp  = new MSGraph<sWord>();
	*xp = cut_object_MSGraph(vp, UMAXCT, MAXCT);
	for (int i=0; i<xp->xs*xp->ys*xp->zs; i++) {
		if (xp->gp[i]>=UMAXCT && xp->gp[i]<MAXCT) {
			xp->gp[i] -= UMAXCT;
		}
	}

	// vp を元に戻す．
    for (int i=0; i<vp.xs*vp.ys*vp.zs; i++) {	
		if (vp.gp[i]>=UMAXCT && vp.gp[i]<MAXCT) vp.gp[i] -= UMAXCT;
    }

	return xp;
}



/**
固定閾値（閾値優先）モード 2Dペイント

	xp の (x, y)座標から，輝度値 mn〜mx の範囲で，かつ隣接点との輝度値の差が v 以下の点に add を加える．

	隣接点の塗りつぶしの条件は，cx<=cc+v && cx>=cc-v && cx<=mx && cx>=mn　
		cc: 現点の輝度値．cx: 隣接点の輝度値

    引数: vp   -- グラフィックデータ構造体．
          x    -- 塗りつぶしを始める点の X座標．
          y    -- 塗りつぶしを始める点の Y座標．
          v    -- 塗りつぶしを行う輝度値の差．
          mn   -- 塗りつぶされる輝度値の最小値
          mx   -- 塗りつぶされる輝度値の最大値．
          mode -- モード．8以上：平面上で8近傍の塗りつぶし
                         その他：平面上で4近傍の塗りつぶし

    使い方：mn〜mxは適当に大きく取って，vで微調整する．
			v>=mx-mn で通常の塗りつぶしになる．MSGraph_Paint()

	注意：輝度値に addがプラスの場合はadd以上　または addがマイナスの場合は
		　add+輝度値がプラスになるものがあると誤作動する．
	      作業領域として add〜 を使用する．
*/
int   continuePaint(MSGraph<sWord> xp, int x, int y, int z, int mn, int mx, int v, int w, int add, int mode, RBound<int>* rbound)
{
    int  i, k, cc, cx, st, ed, num=0;

	if (x<0 || x>=xp.xs || y<0 || y>=xp.ys) return 0;

	// 左端へ移動
    cc = (int)xp.point(x, y);
    while(x>0) {
		cx = (int)xp.point(x-1, y);
    	if (cx>cc+v || cx<cc-w || cx>mx || cx<mn) break;
		cc = cx;
    	x--; 
    }
    st = k = x;

	// 右方向へ塗りつぶし
    while(k<xp.xs) {
    	cx = (int)xp.point(k, y);
    	if (cx>cc+v || cx<cc-w || cx>mx || cx<mn) break;
        xp.point(k, y) += add;
		if (rbound!=NULL) rbound->fusion(k, y, z);
		num++;
		cc = cx;
        k++;
    }
    ed = k - 1;

	// 上下ライン
    for (i=st; i<=ed; i++){ 
		cc = (int)xp.point(i, y) - add;
   		
		// 1ライン上へ
		if (y-1>=0 && y-1<xp.ys){ 
	    	cx = (int)xp.point(i, y-1);
            if (cx<=cc+v && cx>=cc-w && cx<=mx && cx>=mn) {
				num += continuePaint(xp, i, y-1, z, mn, mx, v, w, add, mode, rbound);
	    	}

            if (mode>=8) {   
	    		if (i-1>=0) {
		    		cx = (int)xp.point(i-1, y-1);
            	    if (cx<=cc+v && cx>=cc-w && cx<=mx && cx>=mn) {
		        		num += continuePaint(xp, i-1, y-1, z, mn, mx, v, w, add, mode, rbound);
		    		}
				}
            	if (i+1<xp.xs) {
		    		cx = (int)xp.point(i+1, y-1);
            	    if (cx<=cc+v && cx>=cc-w && cx<=mx && cx>=mn) {
		    			num += continuePaint(xp, i+1, y-1, z, mn, mx, v, w, add, mode, rbound);
					}
				}
	    	}
		}

		// 1ライン下へ
        if (y+1>=0 && y+1<xp.ys){
            cx = (int)xp.point(i, y+1);
            if (cx<=cc+v && cx>=cc-w && cx<=mx && cx>=mn) {
				num += continuePaint(xp, i, y+1, z, mn, mx, v, w, add, mode, rbound);
	    	}

            if (mode>=8) {  
	        	if (i-1>=0) {
		    		cx = (int)xp.point(i-1, y+1);
            	    if (cx<=cc+v && cx>=cc-w && cx<=mx && cx>=mn) {
		    			num += continuePaint(xp, i-1, y+1, z, mn, mx, v, w, add, mode, rbound);
		    		}
				}
            	if (i+1<xp.xs) {
		    		cx = (int)xp.point(i+1, y+1);
            	    if (cx<=cc+v && cx>=cc-w && cx<=mx && cx>=mn) {
		    			num += continuePaint(xp, i+1, y+1, z, mn, mx, v, w, add, mode, rbound);
		    		}
				}
	    	}
 		}
    }

    return num;
}



/**
固定閾値（閾値優先）モード 3Dペイント

	xp の (x, y, z)座標から，輝度値 mn〜mx の範囲で，かつ隣接点との輝度値の差が v 以下の点にUMAXCTを加える．

	隣接点の塗りつぶしの条件は，cx<=cc+v && cx>=cc-v && cx<=mx && cx>=mn　
		cc: 現点の輝度値．cx: 隣接点の輝度値

    引数: vp   -- グラフィックデータ構造体．
          x    -- 塗りつぶしを始める点の X座標．
          y    -- 塗りつぶしを始める点の Y座標．
          mn   -- 塗りつぶされる輝度値の最小値
          mx   -- 塗りつぶされる輝度値の最大値．
          v    -- 塗りつぶしを行う輝度値の差．
          mode -- モード．26以上：平面上で26近傍の塗りつぶし
                          その他：平面上で 6近傍の塗りつぶし

    使い方：mn〜mxは適当に大きく取って，vで微調整する．
			v>=mx-mn で通常の塗りつぶしになる．MSGraph_Paint3D()

	注意：輝度値にマイナス または UMAXCT以上のものがあると誤作動する．
	      作業領域として UMAXCT〜MAXCT を使用する．
*/
int   continuePaint3D(MSGraph<sWord> vp, int x, int y, int z, int mn, int mx, int v, int w, int mode, RBound<int>* rbound)
{
	int  ii, jj, xx, yy, cc, cx;
	int  sy, cy, cz;
	int  num = 0;
	int  mode2d = 8;
    MSGraph<sWord> xp;
	RBound<int> rb;

    if (Stop_Paint3D) return 0;

    Stack_Paint3D++;
    if (Stack_Paint3D > Stack_MaxLimit_Paint3D) {
		print_message("CONTINUE_PAINT3D: Maximum Stacks. stack = %d\n", Stack_Paint3D);
		Stop_Paint3D  = true;
		Error_Paint3D = true;
		Stack_Paint3D--;
		return 0;
    }
	Stack_Max_Paint3D = Max(Stack_Paint3D, Stack_Max_Paint3D);

	// 平面塗り
	if (mode<18) mode2d = 4;
	rb.set(vp.xs-1, 0, vp.ys-1, 0, vp.zs-1, 0);; 
   	xp = vp;
    xp.zs = 1;
    xp.gp = (sWord*)&vp.gp[z*vp.xs*vp.ys];
    num += continuePaint(xp, x, y, z, mn, mx, v, w, UMAXCT, mode2d, &rb);
	if (num==0) return 0;
	if (rbound!=NULL) rbound->fusion(rb);

	Volume_Paint3D += num;
	if (Volume_MaxLimit_Paint3D>0 && Volume_Paint3D>Volume_MaxLimit_Paint3D) {
		print_message("CONTINUE_PAINT3D: Maximum Volume. vol = %d\n", Volume_Paint3D);
		Stop_Paint3D  = true;
		Error_Paint3D = true;
		return num;
	}

//	DEBUG_MODE print_message("continuePaint3D: zz = %3d   stack = %5d\r", z, Stack_Paint3D);
	
 	// カウンタ
	CVCounter* counter = GetUsableGlobalCounter();
	if (counter!=NULL && Stack_Paint3D%10==0) {
		counter->SetPos(Stack_Paint3D);
		if (counter->isCanceled()) {
			Stop_Paint3D = true;
			Stack_Paint3D--;
			return num;
		}
	}

	for (yy=rb.ymin; yy<=rb.ymax; yy++) {
		sy = yy*xp.xs;
		for (xx=rb.xmin; xx<=rb.xmax; xx++) {
			cc = (int)xp.gp[sy + xx];

			if (cc>=UMAXCT && cc<MAXCT) {
 				cc -= UMAXCT;

				if (z-1>=0) {
					if (mode>=26) {
						cz = (z-1)*vp.xs*vp.ys;
						for (jj=-1; jj<=1; jj++) {
							if (yy+jj>=0 && yy+jj<vp.ys) {
								cy = cz + (yy+jj)*vp.xs;
								for (ii=-1; ii<=1; ii++) {
									if (xx+ii>=0 && xx+ii<vp.xs) {
										cx = (int)vp.gp[cy + (xx+ii)];
										if (cx<=cc+v && cx>=cc-w && cx<=mx && cx>=mn) {
											num += continuePaint3D(vp, xx+ii, yy+jj, z-1, mn, mx, v, w, mode, rbound);
										}
									}
								}
							}
						}
					}
					else if (mode>=18) {
						cz = (z-1)*vp.xs*vp.ys;
						for (jj=-1; jj<=1; jj++) {
							if (yy+jj>=0 && yy+jj<vp.ys) {
								cy = cz + (yy+jj)*vp.xs;
								for (ii=-1; ii<=1; ii++) {
									if (Xabs(ii)+Xabs(jj)<2) {
										if (xx+ii>=0 && xx+ii<vp.xs) {
											cx = (int)vp.gp[cy + (xx+ii)];
											if (cx<=cc+v && cx>=cc-w && cx<=mx && cx>=mn) {
												num += continuePaint3D(vp, xx+ii, yy+jj, z-1, mn, mx, v, w, mode, rbound);
											}
										}
									}
								}
							}
						}
					}
					else {
						cx = (int)vp.gp[(z-1)*vp.xs*vp.ys + yy*vp.xs + xx];
						if (cx<=cc+v && cx>=cc-w && cx<=mx && cx>=mn) {
							num += continuePaint3D(vp, xx, yy, z-1, mn, mx, v, w, mode, rbound);
						}
					}
				}


				if (z+1<vp.zs) {
					if (mode>=26) {
						cz = (z+1)*vp.xs*vp.ys;
						for (jj=-1; jj<=1; jj++) {
							if (yy+jj>=0 && yy+jj<vp.ys) {
								cy = cz + (yy+jj)*vp.xs;
								for (ii=-1; ii<=1; ii++) {
									if (xx+ii>=0 && xx+ii<vp.xs) {
										cx = (int)vp.gp[cy + (xx+ii)];
										if (cx<=cc+v && cx>=cc-w && cx<=mx && cx>=mn) {
											num += continuePaint3D(vp, xx+ii, yy+jj, z+1, mn, mx, v, w, mode, rbound);
										}
									}
								}
							}
						}
					}
					else if (mode>=18) {
						cz = (z+1)*vp.xs*vp.ys;
						for (jj=-1; jj<=1; jj++) {
							if (yy+jj>=0 && yy+jj<vp.ys) {
								cy = cz + (yy+jj)*vp.xs;
								for (ii=-1; ii<=1; ii++) {
									if (Xabs(ii)+Xabs(jj)<2) {
										if (xx+ii>=0 && xx+ii<vp.xs) {
											cx = (int)vp.gp[cy + (xx+ii)];
											if (cx<=cc+v && cx>=cc-w && cx<=mx && cx>=mn) {
												num += continuePaint3D(vp, xx+ii, yy+jj, z+1, mn, mx, v, w, mode, rbound);
											}
										}
									}
								}
							}
						}
					}
					else {
						cx = (int)vp.gp[(z+1)*vp.xs*vp.ys + yy*vp.xs + xx];
						if (cx<=cc+v && cx>=cc-w && cx<=mx && cx>=mn) {
							num += continuePaint3D(vp, xx, yy, z+1, mn, mx, v, w, mode, rbound);
						}
					}
				}
			}

			if (Stop_Paint3D) return num;
		}
	}

    Stack_Paint3D--;

//	DEBUG_MODE print_message("continuePaint3D: zz = %3d   stack = %5d\r", z, Stack_Paint3D);
	if (counter!=NULL && Stack_Paint3D%10==0) {
		counter->SetPos(Stack_Paint3D);
		if (counter->isCanceled()) {
			Stop_Paint3D = true;
			return num;
		}
	}

    return num;
}



/**
可変閾値（可変値優先）モード 2Dペイント

	xp の (x, y)座標から，輝度値 mn〜mx の範囲か，または，隣接点との輝度値の差が v 以下の点にUMAXCTを加える．

	隣接点の塗りつぶしの条件は，(cx<cc+v && cx>cc-v) || (cx<=mx && cx>=mn)
		cc: 現点の輝度値．cx: 隣接点の輝度値

    引数: vp   -- グラフィックデータ構造体．
          x    -- 塗りつぶしを始める点の X座標．
          y    -- 塗りつぶしを始める点の Y座標．
          v    -- 塗りつぶしを行う輝度値の差．
          mn   -- 塗りつぶされる輝度値の最小値
          mx   -- 塗りつぶされる輝度値の最大値．
          mode -- モード．8以上：平面上で 8近傍の塗りつぶし
                         その他：平面上で 4近傍の塗りつぶし

    使い方：mn〜mxは必要最低限の大きさに取って，vで微調整する．
			v==0 で通常の塗りつぶしになる．MSGraph_Paint()

	注意：輝度値に，addがプラスの場合はadd以上，または addがマイナスの場合は add+輝度値がプラスになるもの
	      があると誤作動する．
	      作業領域として add〜 を使用する．
*/
int   variablePaint(MSGraph<sWord> xp, int x, int y, int z, int mn, int mx, int v, int w, int add, int mode, RBound<int>* rbound)
{
    int  i, k, cc, cx, st, ed, num=0;

	if (x<0 || x>=xp.xs || y<0 || y>=xp.ys) return 0;

	// 左端へ
    cc = (int)xp.point(x, y);
    while(x>0) {
		cx = (int)xp.point(x-1, y);
    	if ((cx>cc+v || cx<cc-w) && (cx>mx || cx<mn)) break;
		cc = cx;
    	x--; 
    }
    st = k = x;

	// 右方向へ塗りつぶし
    while(k<xp.xs) {
    	cx = (int)xp.point(k, y);
    	if ((cx>cc+v || cx<cc-w) && (cx>mx || cx<mn)) break;
        xp.point(k, y) += add;
		if (rbound!=NULL) rbound->fusion(k, y, z);
		cc = cx;
		num++;
        k++;
    }
    ed = k - 1;


	// 上下ライン
    for (i=st; i<=ed; i++){ 
		cc = (int)xp.point(i, y) - add;
   		
		// 1ライン上へ
		if (y-1>=0 && y-1<xp.ys){ 
	    	cx = (int)xp.point(i, y-1);
            if ((cx<=cc+v && cx>=cc-w) || (cx<=mx && cx>=mn)) {
				num += variablePaint(xp, i, y-1, z, mn, mx, v, w, add, mode, rbound);
	    	}
            if (mode>=8) {   
	    		if (i-1>=0) {
		    		cx = (int)xp.point(i-1, y-1);
            	    if ((cx<=cc+v && cx>=cc-w) || (cx<=mx && cx>=mn)) {
		        		num += variablePaint(xp, i-1, y-1, z, mn, mx, v, w, add, mode, rbound);
		    		}
				}
            	if (i+1<xp.xs) {
		    		cx = (int)xp.point(i+1, y-1);
            	    if ((cx<=cc+v && cx>=cc-w) || (cx<=mx && cx>=mn)) {
		    			num += variablePaint(xp, i+1, y-1, z, mn, mx, v, w, add, mode, rbound);
					}
				}
	    	}
		}

		// 1ライン下へ
        if (y+1>=0 && y+1<xp.ys){
            cx = (int)xp.point(i, y+1);
            if ((cx<=cc+v && cx>=cc-w) || (cx<=mx && cx>=mn)) {
				num += variablePaint(xp, i, y+1, z, mn, mx, v, w, add, mode, rbound);
	    	}
            if (mode>=8) {  
	        	if (i-1>=0) {
		    		cx = (int)xp.point(i-1, y+1);
					if ((cx<=cc+v && cx>=cc-w) || (cx<=mx && cx>=mn)) {
            	    	num += variablePaint(xp, i-1, y+1, z, mn, mx, v, w, add, mode, rbound);
		    		}
				}
            	if (i+1<xp.xs) {
		    		cx = (int)xp.point(i+1, y+1);
					if ((cx<=cc+v && cx>=cc-w) || (cx<=mx && cx>=mn)) {
            	    	num += variablePaint(xp, i+1, y+1, z, mn, mx, v, w, add, mode, rbound);
		    		}
				}
	    	}
 		}
    }

    return num;
}



/**
可変閾値（可変値優先）モード 3Dペイント

	xp の (x, y, z)座標から，輝度値 mn〜mx の範囲か，または，隣接点との輝度値の差が v 以下の点にUMAXCTを加える．

	隣接点の塗りつぶしの条件は，(cx<cc+v && cx>cc-v) || (cx<=mx && cx>=mn)
		cc: 現点の輝度値．cx: 隣接点の輝度値

    引数: vp   -- グラフィックデータ構造体．
          x    -- 塗りつぶしを始める点の X座標．
          y    -- 塗りつぶしを始める点の Y座標．
          v    -- 塗りつぶしを行う輝度値の差．
          mn   -- 塗りつぶされる輝度値の最小値
          mx   -- 塗りつぶされる輝度値の最大値．
          mode -- モード．26以上：平面上で26近傍の塗りつぶし
                          その他：平面上で 6近傍の塗りつぶし

    使い方：mn〜mxは必要最低限の大きさに取って，vで微調整する．
			v==0 で通常の塗りつぶしになる．MSGraph_Paint3D()

	注意：輝度値にマイナス または UMAXCT以上のものがあると誤作動する．
	      作業領域として UMAXCT〜MAXCT を使用する．
*/
int   variablePaint3D(MSGraph<sWord> vp, int x, int y, int z, int mn, int mx, int v, int w, int mode, RBound<int>* rbound)
{
	int  ii, jj, xx, yy, cc, cx;
	int  sy, cy, cz;
	int  num = 0;
	int  mode2d = 8;
    MSGraph<sWord> xp;
	RBound<int> rb;

    if (Stop_Paint3D) return 0;

    Stack_Paint3D++;
    if (Stack_Paint3D > Stack_MaxLimit_Paint3D) {
		print_message("VARIABLE_PAINT3D: Maximum Stacks. stack = %d\n", Stack_Paint3D);
		Stop_Paint3D  = true;
		Error_Paint3D = true;
		Stack_Paint3D--;
		return 0;
    }
	Stack_Max_Paint3D = Max(Stack_Paint3D, Stack_Max_Paint3D);

	// 平面塗り
	if (mode<18) mode2d = 4;
	rb.set(vp.xs-1, 0, vp.ys-1, 0, vp.zs-1, 0);
   	xp = vp;
	xp.zs = 1;
	xp.gp = (sWord*)&vp.gp[z*vp.xs*vp.ys];
    num += variablePaint(xp, x, y, z, mn, mx, v, w, UMAXCT, mode2d, &rb);
	if (num==0) return 0;
	if (rbound!=NULL) rbound->fusion(rb);

	Volume_Paint3D += num;
	if (Volume_MaxLimit_Paint3D>0 && Volume_Paint3D>Volume_MaxLimit_Paint3D) {
		print_message("VARIABLE_PAINT3D: Maximum Volume. vol = %d\n", Volume_Paint3D);
		Stop_Paint3D  = true;
		Error_Paint3D = true;
		return num;
	}

//	DEBUG_MODE print_message("variablePaint3D: zz = %3d   stack = %5d\r", z, Stack_Paint3D);

 	// カウンタ
	CVCounter* counter = GetUsableGlobalCounter();
	if (counter!=NULL && Stack_Paint3D%10==0) {
		counter->SetPos(Stack_Paint3D);
		if (counter->isCanceled()) {
			Stop_Paint3D = true;
			Stack_Paint3D--;
			return num;
		}
	}

	for (yy=rb.ymin; yy<=rb.ymax; yy++) {
		sy = yy*xp.xs;
		for (xx=rb.xmin; xx<=rb.xmax; xx++) {
			cc = (int)xp.gp[sy + xx];

			if (cc>=UMAXCT && cc<MAXCT) {
 				cc -= UMAXCT;

	            if (z-1>=0) {
					if (mode>=26) {
						cz = (z-1)*vp.xs*vp.ys;
						for (jj=-1; jj<=1; jj++) {	// 変数の範囲のチェックを入れる
							if (yy+jj>=0 && yy+jj<vp.ys) {
								cy = cz + (yy+jj)*vp.xs;
								for (ii=-1; ii<=1; ii++) { 
									if (xx+ii>=0 && xx+ii<vp.xs) {
										cx = (int)vp.gp[cy + (xx+ii)];
										if ((cx<=cc+v && cx>=cc-w) || (cx<=mx && cx>=mn)) {
											num += variablePaint3D(vp, xx+ii, yy+jj, z-1, mn, mx, v, w, mode, rbound);
										}
									}
								}
							}
						}
					}
					else if (mode>=18) {
						cz = (z-1)*vp.xs*vp.ys;
						for (jj=-1; jj<=1; jj++) {	// 変数の範囲のチェックを入れる
							if (yy+jj>=0 && yy+jj<vp.ys) {
								cy = cz + (yy+jj)*vp.xs;
								for (ii=-1; ii<=1; ii++) { 
									if (Xabs(ii)+Xabs(jj)<2) {
										if (xx+ii>=0 && xx+ii<vp.xs) {
											cx = (int)vp.gp[cy + (xx+ii)];
											if ((cx<=cc+v && cx>=cc-w) || (cx<=mx && cx>=mn)) {
												num += variablePaint3D(vp, xx+ii, yy+jj, z-1, mn, mx, v, w, mode, rbound);
											}
										}
									}
								}
							}
						}
					}
					else {
						cx = (int)vp.gp[(z-1)*vp.xs*vp.ys + yy*vp.xs + xx];
						if ((cx<=cc+v && cx>=cc-w) || (cx<=mx && cx>=mn)) {
							num += variablePaint3D(vp, xx, yy, z-1, mn, mx, v, w, mode, rbound);
						}
					}
				}


				if (z+1<vp.zs) {
					if (mode>=26) {
						cz = (z+1)*vp.xs*vp.ys;
						for (jj=-1; jj<=1; jj++) {
							if (yy+jj>=0 && yy+jj<vp.ys) {
								cy = cz + (yy+jj)*vp.xs;
								for (ii=-1; ii<=1; ii++) {
									if (xx+ii>=0 && xx+ii<vp.xs) {
										cx = (int)vp.gp[cy + (xx+ii)];
										if ((cx<=cc+v && cx>=cc-w) || (cx<=mx && cx>=mn)) {
											num += variablePaint3D(vp, xx+ii, yy+jj, z+1, mn, mx, v, w, mode, rbound);
										}
									}
								}
							}
						}
					}
					else if (mode>=18) {
						cz = (z+1)*vp.xs*vp.ys;
						for (jj=-1; jj<=1; jj++) {
							if (yy+jj>=0 && yy+jj<vp.ys) {
								cy = cz + (yy+jj)*vp.xs;
								for (ii=-1; ii<=1; ii++) {
									if (Xabs(ii)+Xabs(jj)<2) {
										if (xx+ii>=0 && xx+ii<vp.xs) {
											cx = (int)vp.gp[cy + (xx+ii)];
											if ((cx<=cc+v && cx>=cc-w) || (cx<=mx && cx>=mn)) {
												num += variablePaint3D(vp, xx+ii, yy+jj, z+1, mn, mx, v, w, mode, rbound);
											}
										}
									}
								}
							}
						}
					}
					else {
						cx = (int)vp.gp[(z+1)*vp.xs*vp.ys + yy*vp.xs + xx];
						if ((cx<=cc+v && cx>=cc-w) || (cx<=mx && cx>=mn)) {
							num += variablePaint3D(vp, xx, yy, z+1, mn, mx, v, w, mode, rbound);
						}
					}
				}
			}

			if (Stop_Paint3D) return num;
		}
	}

    Stack_Paint3D--;

//	DEBUG_MODE print_message("variablePaint3D: zz = %3d   stack = %5d\r", z, Stack_Paint3D);
	if (counter!=NULL && Stack_Paint3D%10==0) {
		counter->SetPos(Stack_Paint3D);
		if (counter->isCanceled()) {
			Stop_Paint3D = true;
			return num;
		}
	}

    return num;
}



/**
	密度マスク

	区画の中で縁に接して，かつ zeroでない画素の数を数え，全体の画素の
	数に対して rate 以上の比率の場合，その画素パターンを返す．
	rateに満たない場合は zero で埋められたパターンを返す．
*/
MSGraph<sWord>   density_mask(MSGraph<sWord> vp, double rate, int mode)
{
	MSGraph<sWord> pp;
	int  ps = vp.xs*vp.ys;

	pp.mimicry(vp);

	if ((int)(ps*rate+0.5)==ps) {
		int  eflg = OFF;
		for (int i=0; i<ps; i++) {
			if (vp.gp[i]==vp.zero) {
				pp.state = 0;
				eflg = ON;
				break;
			}
		}
		if (eflg==OFF) {
			for (int i=0; i<ps; i++) pp.gp[i] = vp.gp[i];
			pp.state = ps;
		}
		return pp;
	}

	MSGraph<sWord> xp;
	xp.set(vp.xs+2, vp.ys+2, 1, vp.zero, vp.base, vp.RZxy);

	for (int j=0; j<vp.ys; j++) {
		for (int i=0; i<vp.xs; i++) {
			xp.gp[(j+1)*xp.xs+i+1] = vp.gp[j*vp.xs+i];
		}
	}
	set_around_MSGraph(xp, xp.zero+1);
	MSGraph_Paint(xp, 0, 0, xp.zero+1, EMAXCT-1, EMAXCT, mode);

	int nn = 0;
	for (int j=0; j<vp.ys; j++) {
		for (int i=0; i<vp.xs; i++) {
			if (xp.gp[(j+1)*xp.xs+i+1]==EMAXCT) {
				pp.gp[j*pp.xs+i] = vp.gp[j*vp.xs+i];
				nn++;
			}
		}
	}
	pp.state = nn;

	if ((int)(ps*rate+0.5)>nn) {
		pp.state = 0;
		pp.clear();
	}

	xp.free();
	return pp;
}



/**
	密度フィルター

	画像データの密度マスクを適用して返す．

	size -- マスクのサイズ
	rate -- 密度の閾値
	mode -- 近傍(4 or 8)
*/
MSGraph<sWord>   density_filter(MSGraph<sWord> vp, int size, double rate, int mode)
{
	MSGraph<sWord> pp, xp, wp;
	int hsize = size/2;

	if (hsize==0) hsize = 1;
	size = hsize*2 + 1;
	pp.set(size, size, 1, vp.zero, vp.base);
	wp.mimicry(vp);

	for (int j=hsize; j<vp.ys-hsize; j++) {
		for (int i=hsize; i<vp.xs-hsize; i++) {

			for (int m=-hsize; m<=hsize; m++) {
				int vx = (m+j)*vp.xs + i;
				int px = (m+hsize)*pp.xs + hsize;

				for (int n=-hsize; n<=hsize; n++) {
					pp.gp[px+n] = vp.gp[vx+n];
				}
			}

			xp = density_mask(pp, rate, mode);

			if (xp.state!=0) {
				for (int m=-hsize; m<=hsize; m++) {
					int wx = (m+j)*wp.xs + i;
					int xx = (m+hsize)*xp.xs + hsize;

					for (int n=-hsize; n<=hsize; n++) {
						if (xp.gp[xx+n]!=vp.zero) {
							wp.gp[wx+n] = xp.gp[xx+n];
						}
					}
				}
			}
			xp.free();
		}
	}

	pp.free();
	return wp;
}

