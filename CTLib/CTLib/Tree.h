
#ifndef  _TREE_H
#define  _TREE_H


/**
	再帰的領域探索ヘッダ



  */

#include "Branch.h"
#include "CTRGNio.h"
#include "StatusCheck.h"
#include "exFilter.h"



#define  STOP_SEARCH_DEPTH_OF_BRANCH  15	// 探索を行う，枝の深さの最大値

#define  BRANCH_OVLP_CHECK    0.6f
#define  BRANCH_OVLP_UNIT     0.01f


extern  int		Tree_No;
extern  int		Region_No;
extern  int		Branch_No;
extern  bool	Inter_Path;
extern  bool	Sphere_Start;

extern  int		SavedRegionNum;
extern  bool	isCanceled;

extern  MSGraph<sWord>*		GLMPgd;
extern  MSGraph<sWord>*		GLMPgx;

extern RgnStatRingBuffer*  pSearchRing;
extern RgnStatRingBuffer*  pInterpRing;

//
MSGraph<sWord>* RecursiveRegionSearch(MSGraph<sWord> gd, MSGraph<sWord> gx, int sm, FILE* fp=NULL);


MSGraph<sWord>  look_forward(MSGraph<sWord> gd, RgnStat ns, double rs, int vm, int nz=1);  
MSGraph<sWord>  look_forward(MSGraph<sWord> gd, RgnStat ns, int vm);

int   pre_scan_top_plane(MSGraph<sWord> gd, RgnStat ns, RgnStat** gs, MSGraph<sWord>** vr, int sm);
int   get_region(RgnStat ns, MSGraph<sWord> xp, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm, int cm);
int   get_region_paint(RgnStat ns, MSGraph<sWord> xp, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm, int cm, int xx=-1, int yy=-1);

void  inter_region_path(MSGraph<sWord> gd, MSGraph<sWord> gx, RgnStat* ps, RgnStat* ns, int sm, FILE* fp);
void  inter_branch_path(MSGraph<sWord> gd, MSGraph<sWord> gx, RgnStat* ps, RgnStat* ns, int sm, FILE* fp);

// 
int   start_search_tree(MSGraph<sWord> gd, MSGraph<sWord> gx, double rs, int sm, FILE* fp=NULL);
void  search_tree(MSGraph<sWord> gd, MSGraph<sWord> gx, RgnStat* ns, double rs, int sm, FILE* fp=NULL);
int   labeling(MSGraph<sWord> gd, MSGraph<sWord> gx, RgnStat* ns, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm);

int   search_region         (MSGraph<sWord> gd, RgnStat ns, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm, int cm);
int   search_region_with_thd(MSGraph<sWord> gd, RgnStat ns, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm, int cm);

void  relook_region         (MSGraph<sWord> gd, RgnStat ns, RgnStat** cs, MSGraph<sWord>** vz, int sm);
void  relook_region_with_thd(MSGraph<sWord> gd, RgnStat ns, RgnStat** cs, MSGraph<sWord>** vz, int sm);

int   sphere_next_region(MSGraph<sWord> gd, MSGraph<sWord> gx, RgnStat ns, RgnStat** gs, MSGraph<sWord>** vr, double rs, int sm);
int   sphere_next_vector(MSGraph<sWord> gd, RgnStat ns, Vector<double>** vz, double rx, int sm);
 
void  sphere_back_clean(MSGraph<sWord> fvp, MSGraph<sWord> bvp);

void  output_RgnStat(MSGraph<sWord> gx, RgnStat gs, bool sw, FILE* fp);



#endif
 
