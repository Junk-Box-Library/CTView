
#ifndef  __CTLIB_H_
#define  __CTLIB_H_


/**
CT(Computer Tomography) ライブリ

    Graphライブラリに依存

    変更履歴
        '04 11/22  CT.cpp   continuePaint3D(), variablePaint3D() の範囲外参照を修正
        '05  1/15  βバージョンリリース（1.0）
*/


#include  "GraphLib.h"

#include  "CT.h"

#include  "Branch.h"
#include  "CTRGNio.h"
#include  "exFilter.h"
#include  "StatusCheck.h"

#include  "Tree.h"
#include  "Threshold.h"
#include  "SearchStart.h"


#define  CTLIB_VERSION      "vβ1.2 '05 1/19" 
#define  CTLIB_NAME         "CTLib"
#define  CTLIB_COPYRIGHT    "by Fumi.Iseki (C)  1999"

#endif 
