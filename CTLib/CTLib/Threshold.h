
#ifndef  _S_THRESOLD_H
#define  _S_THRESOLD_H


#include "Tree.h"


#define  THD_BLOOD_VAR_RENGE	1
#define  THD_BRONCHUS_VAR_RENGE	1


// 条件はこれでいいの？  ss:全子領域の面積の合計　fg:全子領域の分散度の合計

//#define  THD_BLOOD_TOO_SMALL    (ss>ns.ss*3  || fg>ns.fg*5.0)	// 血管の閾値を上げる
//#define  THD_BLOOD_TOO_BIG	  (ss<ns.fg*0.2|| fg<ns.fg*0.3)	// 血管の閾値を下げる

#define  THD_BLOOD_TOO_SMALL   (ss>ns.ss*2  || fg>ns.fg*2.0)	// 血管の閾値を上げる
#define  THD_BLOOD_TOO_BIG     (ss<ns.fg*0.5|| fg<ns.fg*0.5)	// 血管の閾値を下げる


//#define  THD_BRONCHUS_TOO_BIG   (ss>ns.ss*3  || fg>ns.fg*5.0)	// 気管支の閾値を下げる
//#define  THD_BRONCHUS_TOO_SMALL (ss<ns.fg*0.2|| fg<ns.fg*0.3)	// 気管支の閾値を上げる

#define  THD_BRONCHUS_TOO_BIG	  (ss>ns.ss*2  || fg>ns.fg*2.0)	// 気管支の閾値を下げる
#define  THD_BRONCHUS_TOO_SMALL (ss<ns.fg*0.5|| fg<ns.fg*0.5)	// 気管支の閾値を上げる


int   get_region_with_thd(RgnStat ns, MSGraph<sWord> vp, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm, int cm);
int   search_thd_bronchus(RgnStat, MSGraph<sWord>, RgnStat**, MSGraph<sWord>**, double, int);
int   search_thd_blood   (RgnStat, MSGraph<sWord>, RgnStat**, MSGraph<sWord>**, double, int);


void  search_most_thd      (RgnStat ns, MSGraph<sWord> vp, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm, double rate=0.1);
void  paint_up_thd_bronchus(RgnStat ns, MSGraph<sWord> vp, RgnStat** gs, MSGraph<sWord>** vr, double rs, double rate);
void  paint_down_thd_blood (RgnStat ns, MSGraph<sWord> vp, RgnStat** gs, MSGraph<sWord>** vr, double rs, double rate);

int   get_region_paint_first(RgnStat ns, MSGraph<sWord> xp, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm, int cm, double fg=1.0, int xx=-1, int yy=-1);


//int   search_thd_blood2(RgnStat,MSGraph<sWord>,MSGraph<sWord>,RgnStat**,MSGraph<sWord>**,int);

//void  paint_up_bronchus(RgnStat,MSGraph<sWord>,MSGraph<sWord>,RgnStat*);
//void  paint_up_blood(RgnStat, MSGraph<sWord>, MSGraph<sWord>, MSGraph<sWord>, RgnStat*, int);


//#define  get_region_with_thd(ns, vp, gs, vr, sm, cm)   search_thd(ns, vp, gs, vr, sm, cm)

#endif
 
