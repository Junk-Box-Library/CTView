
#ifndef  _CT_APP_LIB_H_
#define  _CT_APP_LIB_H_


/**
CT アプリケーションライブラリ ヘッダ


*/




#include "CT.h"


#define  STACK_MAX_PAINT3D  10000

#define  AND_PAINT		1
#define  OR_PAINT		2


extern bool Error_Paint3D;				// スタックオーバーや体積上限により停止
extern bool Stop_Paint3D;				// キャンセルやスタックオーバーにより停止
extern int  Stack_MaxLimit_Paint3D;		// スタックの最大制限数（3D呼び出しの場合のみ）
extern int  Stack_Max_Paint3D;			// 使用したスタックの最大数

extern int  Volume_Paint3D;
extern int  Volume_MaxLimit_Paint3D;	// 検出体積の上限（3D呼び出しの場合のみ）<=0 なら制限なし



// リージョングローイング
MSGraph<sWord>*  RegionGrowing(MSGraph<sWord> vp, Vector<int> pnt, Vector<int> thd, 
									int method=OR_PAINT, int mode=18, RBound<int>* rb=NULL, int* size=NULL, int ecnt=FALSE);

int   andPaint3D(MSGraph<sWord> vp, int x, int y, int z, int mn, int mx, int v, int w, int mode, RBound<int>* rbound);
int   orPaint3D (MSGraph<sWord> vp, int x, int y, int z, int mn, int mx, int v, int w, int mode, RBound<int>* rbound);


int   continuePaint(MSGraph<sWord> xp, int x, int y, int z, int mn, int mx, int v, int w, int add, int mode, RBound<int>* rb);
int   variablePaint(MSGraph<sWord> xp, int x, int y, int z, int mn, int mx, int v, int w, int add, int mode, RBound<int>* rb);
int   continuePaint3D(MSGraph<sWord> vp, int x, int y, int z, int mn, int mx, int v, int w, int mode, RBound<int>* rb);
int   variablePaint3D(MSGraph<sWord> vp, int x, int y, int z, int mn, int mx, int v, int w, int mode, RBound<int>* rb);




MSGraph<sWord>   density_mask(MSGraph<sWord> vp, float rate=1.0, int mode=8);
MSGraph<sWord>   density_filter(MSGraph<sWord> vp, int size, float rate=1.0, int mode=8);



#endif
 
