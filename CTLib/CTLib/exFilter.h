
#ifndef  _FILTER_H_EX
#define  _FILTER_H_EX


#include "Gmt.h"
#include "Branch.h"


extern int    FilterMode;


MSGraph<sWord>	xfilter(RgnStat ns, MSGraph<sWord> xp);


/*
MSGraph<sWord>	fx_mask(MSGraph<sWord> xp, FMask mask);
void			fi_mask(MSGraph<sWord> xp, FMask mask);
MSGraph<sWord>	curvef(MSGraph<sWord> xp, int m);
*/

#endif

 
