
/*
ＣＴファイル入出力関数ヘッダ  
 
    CTファイルへの入出力をサポートする．
    また，ファイル入出力の統合関数もサポート

    依存関係
        #include  "Gio.h"
        #include  "Branch.h"
  
    注：カウンタの使用宣言個所が不満！！（特に書き込み関数）
*/

#ifndef __CTRGNIO_H_
#define __CTRGNIO_H_


#include  "Gio.h"
#include  "Branch.h"

#include  <sys/stat.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//  for 領域データ
//
RgnData* readRgnFile (char* fn, bool cnt=false);
int      writeRgnFile(char* fn, RgnData** pRgnData, int no, bool cnt=false);

RgnData* readRgnSLFile (char* fn, bool cnt=false);
FILE*    wOpenRgnSLFile(char* fn, unsigned int no=0);
void     writeRgnData(FILE* fp, RgnStat ns);

/**/

#endif

