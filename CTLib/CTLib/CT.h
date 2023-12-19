
#ifndef  __CT_H_
#define  __CT_H_

/**

CT操作用ヘッダ


*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "tools++.h"
#include "Graph.h"


using namespace jbxl;



#define  MINCT	-30000
#define  UMINCT	-20000
#define  EMINCT  -5000
#define  EMAXCT  10000
#define  UMAXCT  20000	// UMAXCT > EMAXCT - EMINCT
#define  MAXCT   32000
/**

       〜 MINCT		未使用
MINCT  〜 UMINCT	このプログラムが使用
UINCT  〜 EMINCT	このプログラムが使用
EMINCT 〜 EMAXCT	実質的な CT値の範囲（探索の上限）（+4096 されている場合もあるので）
EMAXCT 〜 UMAXCT	このプログラムが使用
UMAXCT 〜 MAXCT		このプログラムが使用
MAXCT  〜			未使用（システムが使用）
*/



#define  FORWARD   0
#define  BACKWARD  1


using namespace jbxl;



int		 get_fPoint(MSGraph<sWord> gd, double xx, double yy, double zz);

#define  set_around_space(vp)  set_around_MSGraph((vp),  0)





#endif
 
