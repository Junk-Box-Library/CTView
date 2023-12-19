
#ifndef  _BRANCH_H
#define  _BRANCH_H

/**
	再帰的領域探索のための領域操作用基本ヘッダ
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <list>
#include <algorithm>

#include "common++.h"
#include "tools.h"

#include "ExGdata.h"
#include "Gmt.h"
#include "CT.h"


class  RgnStat;
class  RgnData;
class  Branch;
class  BrcData;


using namespace jbxl;


// 領域データのデフォルト一時ファイル名
#define  TEMP_RGN_FILENAME  ".RGN_TEMP_FILE"


//
// 探索空間へのグローバルポインタ
extern  MSGraphDisp<sWord>*	GLMPDisp;


// 配列の最大値
#define  RGN_MAXNO     30		// 分岐（子領域）の候補数の最大値
#define  NXTRGN_MAXNO  10		// 分岐数（子領域の数）の最大数
//#define  RING_BUF_SIZE 10000	// 探索用リングバッファのサイズ
#define  INTERPATH_MAX 200		// 領域補間用配列


//
// Ring Buffer
//extern int 	Search_Ring_Addpt;
//extern int	Search_Ring_Point;
//extern int	Search_Ring_Number;
//extern RgnStat	 Search_Ring_Buffer[RING_BUF_SIZE];


// 器官の識別
#define  TRACHEA	1
#define  BRONCHUS	2
#define  BLOOD		3
#define  HILUM		4
#define  OBJECTS	100


// 観測面のビューモード 　0は指定しないこと
#define  FTMODE		1		// 平面モード（実数による線形補間）
#define  WDMODE		2		// 平面モード（Word)
#define  SPMODE		3		// 球面モード
#define  XVMODE		10		// ３Ｄモード＋他のモード



//////////////////////////////////////////////////////////////////////////////////////////////////////
//　領域
//

// 領域の属性 (at)
#define  RGN_OVLP_TERM	100		// 重複領域（ループ）の枝の末端
#define  RGN_OVLP_PNT	101		// 重複領域（ループ）の枝の開始点．ループポイント
#define  RGN_DEL		103		// 削除すべき領域．
#define  RGN_FORK		104		// 枝分かれ（直前）の領域．
#define  RGN_CONTINUE	105		// 枝の末端．継続する枝は１つ．		
#define  RGN_END		106		// 枝の末端領域．継続枝無し．
#define  RGN_NORMAL		110		// ノーマル領域（枝の途中）
#define  RGN_INTER		111		// 枝間の補間領域
#define  RGN_DUMMY		200		// ダミーのリージョン．何も処理しない．


/**
グループ番号について．
		同一グループ番号(gn)内では，木番号(tn), 枝番号(bn), 領域番号(nc)は一意的である．
	　　逆に言えば，違うグループ内では，木番号，枝番号，領域番号は同じ物がある可能性がある．
	　　グループ番号は実際の探索オペレーションを表すとも言える．
  */

//
// 領域の状態を表す
//
class  RgnStat
{
public:
	// ワールド座標
    double	xx;				// ワールド座標系での領域の重心のX座標						in GV 
    double	yy;				// ワールド座標系での領域の重心のY座標						in GV 
    double	zz;				// ワールド座標系での領域の重心のZ座標						in GV 
    double	px;				// ワールド座標系での領域の単位方向ベクトル（視線）の X成分	in RL & GV 
    double	py;				// ワールド座標系での領域の単位方向ベクトル（視線）の Y成分	in RL & GV 
    double	pz;				// ワールド座標系での領域の単位方向ベクトル（視線）の Z成分	in RL & GV 
    double	ex;				// ワールド座標系での次の領域への単位方向ベクトルの X成分	in RL & GV 
    double	ey;				// ワールド座標系での次の領域への単位方向ベクトルの Y成分	in RL & GV 
    double	ez;				// ワールド座標系での次の領域への単位方向ベクトルの Z成分	in RL & GV 

	// ローカル座標
	int		os;				// 観測面の X,Yサイズ
    int		xs;				// 領域面の X(y')サイズ									in CR 
    int		ys;				// 領域面の Y(z')サイズ									in CR 
    int		sx;				// 観測面での領域面の左上隅の X座標						in CR 
    int		sy;				// 観測面での領域面の左上隅の Y座標						in CR 
    int		wx;				// 領域面での領域の重心の X座標							in CR 
    int		wy;				// 領域面での領域の重心の Y座標							in CR 
    int		ux;				// 領域面での領域の重心に最も近い領域上の点の X座標		in CR 
    int		uy;				// 領域面での領域の重心に最も近い領域上の点の Y座標		in CR 

	// 領域の２Ｄ画像属性
    int		ss;				// 面積													in CR 
    int		rr;				// 半径^2（ユークリッド距離による）						in CR 
	double	ra;				// 半径　2*ss/sr										in CR 
    double	sr;				// 外周長（斜めは√2）									in CR 
    double	su;				// 外周長 (by iseki's method)							in CR 
    double	fg;				// 分散度（円形度の逆数  by iseki's method）			in CR 
    int		dg;				// 輝度値の平均											in CR 
    int		r2;				// 重心と観測面（ローカル座標）の中心の距離の2乗		in CR 
    int		mo;				// 領域の重心の周りの統計的分散の2乗（モーメント）		in CR 
    int		vx;				// 領域のサイズ		= Max(xs, ys)						in LA 
    int		vs;				// 次の観測面のX,Yサイズ　例) vs= 2*vx+2 
	int		p1[10];			// 予備．パディング
    sWord*	gp; 			// 領域のイメージ（通常，MSGraphのデータへポイント）	in CR 

	// 木構造属性
    int		gn;				// グループ番号．メモリ内でのグループ化で使用．
    int		tn;				// 木番号												in SST
    int		bn;				// 枝番号												in LA 
    int		nc;				// 領域番号												in ST 
	int		bp;				// 親枝の枝番号　										in LA 
    int		np;				// 親領域の領域番号　									in CR 
    int		pn;				// 親領域での分岐数（姉妹領域の数）　					in LA 
    int		rn;				// 分岐の深さ（木の根からこの領域までの分岐回数）　　	in LA 
	int		nn;				// 子領域の数（分岐数）									in LA 
	int		at;				// 領域の属性

	// 状態
	bool	st;				// 木の一番最初の領域か?                                 
	bool	sb;				// 枝の一番最初の領域か?                                 
    bool	ip;				// 領域間の補間領域か?									in IP 
    bool	ib;				// 枝間の補間領域か?									in IP 
	bool	nr;				// 適当な領域か？　true: OK, false: NG
	bool	lt;				// 領域を含む枝はループしているか                      

	// 閾値
    int		mh;				// これ以上の輝度値は無視（固定）
    int		mx;				// 閾値探索の上限（領域により変化，mhを超えない）
    int		mm;				// 閾値													in PU 
    int		mn;				// 閾値探索の加減（領域により変化，mlを下回らない）		in CR 
    int		ml;				// これ以下の輝度値は無視（固定）
	int     zr;				// 処理上ゼロとみなす輝度値 < ml

	// 重複
	int		un;				// 枝の開始点からその領域までの総領域数．                         
    int		on;				// 枝の開始点からその領域まで重複領域数．                         
    int		ob;				// 重複領域が重なっている相手の枝番号．親枝優先．0:無し．                      
   	bool	op;				// この領域は重複しているか？ 
    double	ov;				// 領域の重複度                         

    int		nx[NXTRGN_MAXNO];// 子領域の領域番号（ポインタ）　			             
	int     pt[20];			// 予備．パディング

public:
	RgnStat() { init();}
	~RgnStat(){}	// 通常 gp はフリーしない方がよい．他（MSGraph等）で使っている可能性があるので

	void  init() { bzero((char*)this, sizeof(RgnStat));}

	void  free() { if(!isNull()) freeNull(gp); init();}
	
	bool  isNull() {
		if (gp==NULL) return true; 
		return false;
	}
};



//
// 全領域データ
//
class  RgnData
{
public:
    int				no;		// 領域総数
    RBound<int>		rb;		// 領域範囲
    RgnStat*		rs;		// 領域データの配列 

public:
	RgnData() { init();}
	~RgnData(){ free();}


	void  init() { no=0; rb.init(); /*ms.init();*/ rs=NULL;}

	void  free() { 
		for (int i=0; i<no; i++) rs[i].free(); 
		delete [] rs;
		init();
	}

	int	  next(int id);			// 同じ枝内で rs[id] の次の領域のインデックスを返す．
	int	  end (int id);			// 同じ枝内での最後の領域のインデックスを返す．

	void  well_formed();		// トポロジー的に完全な状態に修正する．
	bool  well_form_check();	// トポロジー的に完全な状態かチェックする．デバッグ用．
	void  find_loop_point();	// ループ判定処理を行う．

	BrcData* make_branch();		// RgnData から BrcDataを生成する．
	MSGraph<sWord> make_MSGraph(int mode=4);		// RgnData から BrcDataを生成する．
};



//////////////////////////////////////////////////////////////////////////////////////////////////////
//  枝
//

// 枝の属性
#define  BRC_NORMAL	500
#define	 BRC_END	501
#define	 BRC_DUMMY	502
#define  BRC_LOOP	503		// LOOPの枝間を繋ぐ枝 
#define  BRC_INTER	504		// 枝間を補間した枝


typedef std::list<Vector<double>*>   Vector_List;


//
// 枝データクラス
//
class  Branch 
{
public:
    char	name[20];		// 枝の名前（解剖学上名前があれば...）
    int		kd;				// 枝の種類 
    int		rl;	 			// 体の中での大まかな位置（ジオメトリ）LEFT, CENTER or RIGHT 

	// 他のデータのインデックス
    int		id;				// ID．枝の開始点の領域が格納されている RgnData.rs[]のインデックス
	int		pd;				// 親枝が格納されている BrcData.br[]のインデックス．木の根っこの場合は-1．

    int		gn;				// グループ番号
	int     tn;				// 木番号
    int		bn;				// 枝番号
    int		bp;				// 親枝の番号
    int		ob;				// LOOPしている場合の次の枝の番号

	int		vn;				// 枝に含まれる領域の数
	int		rn;				// 枝の深さ
    int		nn;				// 分岐数（子枝の数）
	int     pn;				// 親の分岐数（姉妹枝の数）
    int		at;      		// 枝の属性
    int		ss, se;			// 断面積（開始点と終点） 
    double   fg, fe;			// 分散度（開始点と終点）

    Vector<double>	vs;		// 開始点座標ベクトル
    Vector<double>	ve;		// 終点座標ベクトル
	Vector_List*	lv;		// 座標のリストへのポインタ

public:
	Branch() { init();}
	~Branch(){ free();}

	void  init() { bzero((char*)this, sizeof(Branch)); lv=new Vector_List();}

	void  clear_vector() { 
		Vector<double>* vc;
		Vector_List::iterator ivct=lv->begin();
		while (ivct!=lv->end()) {
			vc = *ivct;
			ivct = lv->erase(ivct);
			delete(vc);
		}
		vs.init();
		ve.init();
		vn = 0;
	}

	void  free() { 
		clear_vector();
		delete(lv);
		bzero((char*)this, sizeof(Branch));
	}

};



//
// 全枝データ
//
class  BrcData 
{
public:
    int			no;			// 枝の総数
    RBound<int>	rb;			// 領域範囲
    Branch*		br;			// 枝データの配列へのポインタ


public:
	BrcData() { init();}
	~BrcData(){ free();}


	void  init() { no=0; rb.init(); br=NULL;}
	void  free() { delete [] br; init();}
};




//////////////////////////////////////////////////////////////////////////////////////////
//

#define  DUPRGN_MAXNO  100


class  DupBrc
{
public:
    int		 no;				// 総数
    int		 bn[DUPRGN_MAXNO];	// 枝番号が入る
    long int mg[DUPRGN_MAXNO];	// 重複点の数

public:
	DupBrc() { init();}
	~DupBrc(){}

	void  init() { no=0; for(int i=0; i<DUPRGN_MAXNO; i++) mg[i]=bn[i]=0;}
	void  set(int n);
	int   get_maxbrc();
};


extern  DupBrc  dupBranch;




/**
class  RgnStatRingBuffer 

	機能：探索用リングバッファの先頭に，領域のデータのコピーを加える．
	      グラフィックデータはコピーされないため，バッファに格納後，外部で
		  グラフィックデータを破棄しないこと．



	リングバッファ                 addPoint
                                      ↓
		-------------------------------------------
		|   |   | P | C | P | C | P | C |   |   |      P: Parent Region,    C: Child Region
		-------------------------------------------
				 ↑
		       getPoint 
*/

/////////////////////////////////////////////////////////////////////////////////////////////
class  RgnStatRingBuffer 
{
protected:
	int bufferSize;
    int dataNum;
    int getPoint;
	int addPoint;
    RgnStat* buffer;		// バッファ本体へのポインタ

public:

    RgnStatRingBuffer(int n=100) {
        bufferSize = n;
        buffer   = (RgnStat*)malloc(n*sizeof(RgnStat));
		dataNum  = 0;
		getPoint = 0;
		addPoint = 0;
    }

	~RgnStatRingBuffer() {
		free(buffer);
	}

    void  addData(RgnStat ns) {
		if (dataNum==bufferSize) {
			DEBUG_MODE print_message("addData: 一杯のリングに addしようとした!!\n");
			return;
		}
		dataNum++;
        buffer[addPoint] = ns;
        addPoint++;
        if (addPoint>=bufferSize) addPoint -= bufferSize;
    }

    RgnStat getData() {
		RgnStat ns;
		if (dataNum==0) {
			DEBUG_MODE print_message("getData: 空のリングから getしようとした!!\n");
			ns.init();
			return ns;
		}
		dataNum--;
		ns = buffer[getPoint];
		getPoint++;
		if (getPoint>=bufferSize) getPoint -= bufferSize;
		return ns;
   	}

    void clearBuffer()  { getPoint = addPoint = dataNum  = 0;}
    int getBufferSize() { return bufferSize;}
    int getDataNum()    { return dataNum;}
};




//////////////////////////////////////////////////////////////////////////////////////////////////////
//

MSGraph<sWord>  pre_paint_region(RgnStat ns, MSGraph<sWord> wp, int sm, int xx=-1, int yy=-1);
MSGraph<sWord>  pre_cut_region(RgnStat, MSGraph<sWord>, int);
void  cut_color (RgnStat, MSGraph<sWord>, int);
int	  cut_region(RgnStat, MSGraph<sWord>, MSGraph<sWord>, RgnStat**, MSGraph<sWord>**); //, int);

int	   view_wide(RgnStat* n);
int    rsorting(RgnStat**, MSGraph<sWord>**, int);
int    nsorting(RgnStat**, MSGraph<sWord>**, int);

void   free_data(RgnStat** gs, MSGraph<sWord>** vr, int nn, int mm);
void   get_world_center (RgnStat ns, RgnStat *gs, double rs);
void   get_direct_vector(RgnStat ns, RgnStat *gs);

MSGraph<sWord>  fviewport(MSGraph<sWord> gd, RgnStat ns, double rs);
MSGraph<sWord>  wviewport(MSGraph<sWord> gd, RgnStat ns, double rs);
MSGraph<sWord>  pviewport(MSGraph<sWord> gd, RgnStat ns, double rs, double st);
MSGraph<sWord>  xviewport(MSGraph<sWord> gd, RgnStat ns, double rs, int vm, int nz=1);


void   Local2World_RS(MSGraph<sWord> gd, RgnStat ns, int mode=0);

inline int  get_color_from_bn(int bn) { return (UMAXCT - bn*10);}
inline int  get_bn_from_color(int cl) { return (UMAXCT - cl)/10;}


void   bi_paint(MSGraph<sWord> fvp, MSGraph<sWord> bvp, int x, int y, int mn, int mx, int c, int m, int id);


//void  add_search_ring(RgnStat  pn, RgnStat  ns);
//void  get_search_ring(RgnStat* pn, RgnStat* ns); 

RBound<int>  check_rgn_world(RgnStat ns, RBound<int> rb);

void   println_RgnStat (RgnStat ns);


#endif
