
#ifndef  _STATUS_CHECK_H
#define  _STATUS_CHECK_H


#include "Branch.h"


//
// チェックの種類
#define  NO_CHECK    -1
#define  NORMAL       0

#define  PRE_SCAN_TRACHEA  1
#define  PRE_SCAN_OBJECTS  2

#define  SSTART       3
#define  LABEL        4
#define  SPHERE_NEXT  5
#define  SPHERE_CHK   6

#define  INTER_CHECK  7
#define  INTER_PATH   8
#define  RELOOK       9

#define  THRESHOLD   10
#define  PAINT_UP    11

#define  ALL_OK      99



// 最小断面積
#define SMALLEST_BRONCHUS 0
#define SMALLEST_OBJECTS  0
#define SMALLEST_BLOOD    0


// 領域検査でのエラー情報
#define  RGN_ERR_OVLP	-401		// 領域が親領域とオーバーラップしていない．
#define  RGN_ERR_CMPLX	-402		// 領域の形が複雑すぎる．（分散度が大きすぎる）
#define  RGN_ERR_SMALL  -403		// 領域が小さすぎる．
#define  RGN_ERR_BIG    -404		// 領域が大きすぎる．
#define  RGN_ERR_EXT    -405		// 処理対処外の領域．



int     chk_region_status(RgnStat ns, RgnStat** gs,  int nn, int sm, int md); 

// 血管
void    chk_blood_region(RgnStat ns, RgnStat** gs, int nn, int md);
void    chk_blood_unit  (RgnStat ns, RgnStat** gs, int nn);
void    chk_blood_glvl  (RgnStat ns, RgnStat** gs, int nn);

// 気管支
void	chk_bronchus_region(RgnStat ns, RgnStat** gs, int nn, int md); 
void	chk_bronchus_unit  (RgnStat ns, RgnStat** gs, int nn);
void	chk_bronchus_glvl  (RgnStat ns, RgnStat** gs, int nn);

// オブジェクト
void	chk_objects_region(RgnStat ns, RgnStat** gs, int nn, int md); 
void	chk_objects_unit  (RgnStat ns, RgnStat** gs, int nn);
void	chk_objects_glvl  (RgnStat ns, RgnStat** gs, int nn);



bool	next_region_check   (RgnStat ns, RgnStat gs, double p);
int		overlap_region_check(RgnStat ns, RgnStat gs, int limit);

double	loop_region_check  (MSGraph<sWord> gx, RgnStat* gs, double rx);
bool	inter_region_check (MSGraph<sWord> gd, RgnStat* ps, RgnStat* ns, int sm);
bool	inter_duppath_check(MSGraph<sWord> gx, RgnStat* ps, RgnStat* ns);


#endif
 
