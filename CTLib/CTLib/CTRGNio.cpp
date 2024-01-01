/*
CTデータ用ファイル入出力関数  
                                   
  ヘッダ
      #include "CTRGNio.h"
*/

#include  "common++.h"
#include  "tools++.h"
#include  "ClassBox.h"
#include  "CTRGNio.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 領域ファイルサポート
//

RgnData*  readRgnFile(char* fn, bool cnt) 
{
    RgnData* pRgnData = readRgnSLFile(fn, cnt);
    if (pRgnData==NULL) return NULL;

    pRgnData->well_formed();
    pRgnData->find_loop_point();
    pRgnData->well_form_check();

    return  pRgnData;
}


int   writeRgnFile(char* fn, RgnData** pRgnData, int no, bool cnt) 
{
    int  i, j;
    CVCounter* counter = NULL;

    // 領域の数をカウント
    unsigned int nn = 0;
    for (i=0; i<no; i++) {
        for (j=0; j<pRgnData[i]->no; j++) nn++;
    }

    FILE* fp = wOpenRgnSLFile(fn, nn);
    if (fp==NULL) return -1;

    if (nn>=10 && cnt) {
        counter = GetUsableGlobalCounter();
        if (counter!=NULL) {
            counter->SetTitle("領域ファイル書き込み中");
            counter->SetMax(nn/10);
            counter->SetPos(0);
        }
    }

    nn = 0;
    for (i=0; i<no; i++) {
        for (j=0; j<pRgnData[i]->no; j++) {
            writeRgnData(fp, pRgnData[i]->rs[j]);
            nn++;
            
            if (counter!=NULL && nn%10==0) {
                counter->StepIt();
                if (counter->isCanceled()) {    // キャンセル
                    return nn;
                }
            }
        }
    }

    // ファイルの最後に領域数を入れる．
    unsigned int nx = htonl(nn);
    fwrite(&nx, 4, 1, fp); 
    fclose(fp);

    if (counter!=NULL) counter->PutFill();
    return nn;
}


FILE*   wOpenRgnSLFile(char* fn, unsigned int no)
{
    FILE*    fp;
    CmnHead  hd;

    if ((fp=fopen(fn,"wb"))==NULL) {
        DEBUG_MODE print_message("WOPEN_RGN_SLFILE: ファイルオープンエラー!!");
        return fp;
    }

    bzero((char*)&hd, sizeof(CmnHead));
    hd.kind  = CT_RGN_SL;
    hd.depth = 16;
    hd.bsize = no;
    hd.lsize = 0;
    hd.buf   = NULL;
    hd.grptr = NULL;

    hton_st(&hd, 4);
    fwrite(&hd, sizeof(CmnHead), 1, fp);

    return  fp;
}


void  writeRgnData(FILE* fp, RgnStat ns)
{
    int   nsize, gsize;
    sWord* gp;

    nsize = sizeof(RgnStat);
    gsize = ns.xs*ns.ys*sizeof(sWord);
    gp = ns.gp;

    ns.gp = NULL;
    hton_st(&ns, 4);
    hton_ar(gp, gsize);
    fwrite(&ns, nsize, 1, fp);
    fwrite(gp,  gsize, 1, fp);

    return;
}


/**
RgnData*  readRgnSLFile(char* fn, bool cnt)

    機能：RgnStat ns の画像データをファイルから読み込む.
    　　　各スライス（観測面）の画像データはワールド座標系に展開へ展開される．
    　　　また，各スライス（観測面）の画像データは保持されたままとなる．

    引数：
        fn : データファイル名．  
 */
RgnData*  readRgnSLFile(char* fn, bool cnt)
{
    unsigned int nn, lsize;
    int      rsize;
    FILE*    fp;
    CmnHead  hd;
    RgnStat  ns, *np;
    RgnData* rd;
    CVCounter* counter = NULL;

    unsigned int fsz = file_size(fn);
    if (fsz==0) {
        DEBUG_MODE print_message("READRGNSLFILE: ファイルオープンエラー1!! %s", fn);
        return NULL;
    }
    if ((fp=fopen(fn,"rb"))==NULL) {
        DEBUG_MODE print_message("READRGNSLFILE: ファイルオープンエラー2!! %s", fn);
        return NULL;
    }

    // ヘッダ読み込み
    fread(&hd, sizeof(CmnHead), 1, fp);
    ntoh_st(&hd, 4);
    if(!checkBit(hd.kind, CT_RGN_SL)) {
        DEBUG_MODE print_message("READRGNSLFILE: ファイルタイプが合っていない．File Type = %04x", hd.kind);
        fclose(fp);
        return NULL;
    }
    
    if (hd.bsize>0) nn = hd.bsize;
    else {
        fseek(fp, (long int)fsz-4, 0);
        fread(&nn, 4, 1, fp);
        nn = ntohl(nn);    //    領域数
    }
    DEBUG_MODE print_message("readRgnSLFile: 領域数は %d", nn);

    // カウンタ
    if (nn>=10 && cnt) {
        counter = GetUsableGlobalCounter();
        if (counter!=NULL) {
            counter->SetTitle("領域ファイル読み込み中");
            counter->SetMax(nn*2/10);
            counter->SetPos(0);
        }
    }

    rsize = sizeof(RgnStat);
    RBound<int> rb(SINTMAX, SINTMIN, SINTMAX, SINTMIN, SINTMAX, SINTMIN);
     
    fseek(fp, sizeof(CmnHead), 0);
    int i;
       for (i=0; i<(int)nn; i++) {
        fread(&ns, rsize, 1, fp);
        ntoh_st(&ns, 4);
        lsize = ns.xs*ns.ys*sizeof(sWord);
        ns.gp = (sWord*)malloc(lsize);
        if (ns.gp==NULL) {
            DEBUG_MODE print_message("READRGNSLFILE: メモリ不足1． NS.GP, %d Byte.", lsize);
            fclose(fp);
            return NULL;
        }

        fread(ns.gp, lsize, 1, fp);
        ntoh_ar(ns.gp, lsize);
        rb = check_rgn_world(ns, rb);
        ns.free();

        if (counter!=NULL && i%10==0) {
            counter->StepIt();
            if (counter->isCanceled()) {    // キャンセル
                fclose(fp);
                return NULL;
            }
        }
    }

    rb.xmax += 1;
    rb.ymax += 1;
    rb.zmax += 1;
    rb.xmin -= 1;
    rb.ymin -= 1;
    rb.zmin -= 1;

    np  = new RgnStat[nn]();
    if (np==NULL) {
        DEBUG_MODE print_message("READRGNSLFILE: メモリ不足．NP");
        fclose(fp);
        return NULL;
    }

    fseek(fp, sizeof(CmnHead), 0);
    for (i=0; i<(int)nn; i++) {
        fread(&ns, rsize, 1, fp);
        ntoh_st(&ns, 4);
        np[i] = ns;
        lsize = ns.xs*ns.ys*sizeof(sWord);
        np[i].gp = ns.gp = (sWord*)malloc(lsize);
        if (ns.gp==NULL) {
            DEBUG_MODE print_message("READ_RGN_SLFILE: メモリ不足2． NS.GP, %d Byte.", lsize);
            fclose(fp);
            delete [] np;
            return NULL;
        }

        fread(ns.gp, lsize, 1, fp);
        ntoh_ar(ns.gp, lsize);

        if (counter!=NULL && i%10==0) {
            counter->StepIt();
            if (counter->isCanceled()) {    // キャンセル
                fclose(fp);
                delete [] np;
                return NULL;
            }
        }
    }

    fclose(fp);

    rd = new RgnData();
    rd->no = nn;
    rd->rb = rb;
    rd->rs = np;

    if (counter!=NULL) counter->PutFill();

    return rd;
}

