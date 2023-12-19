
#include  "exFilter.h"


int  FilterMode = 0;


/**
    とりあえず書いた．
*/
MSGraph<sWord>  xfilter(RgnStat ns, MSGraph<sWord> xp)
{
    MSGraph<sWord> vp;
    copy_MSGraph(xp, vp);
    return vp;
}


/*
MSGraph<sWord>  fx_mask(MSGraph<sWord> xp, FMask mask)
{
    int   i, j, x, y, cx, cy, lsize;
    int   xx, yy, cp, cw, ux;
    int   kc, xc, xs, ps;
    int   ms, nf, nx, nx2, anx;
    int   pm, zc, z, zz, mz, sw;
    float   dd, dg, df, fx, dz;
    MSGraph<sWord> vp, na;

    if ((xp.zs>1&&mask.mode==2)||(xp.zs<=1&&mask.mode>2)) {
        DEBUG_MODE print_message("FX_MASK: Error: mismach mask dimension ");
        DEBUG_MODE print_message("%d %d",xp.zs,mask.mode);
        vp.xs = vp.ys = vp.zs = 0;
        vp.gp = NULL;
        return vp;
    }

    nf = mask.nfact;
    ms = mask.msize;
    if (mask.mode==2) {
        sw = 0;
        pm = ms*ms;
        mz = 0;
    }
    else {
        sw = 1;
        pm = ms*ms*ms;
        mz = Min(ms, xp.zs-2);
    }

    kc = pm/2;
    zc = mz/2;
    xc = ms/2;

    xs = xp.xs;
    ps = xp.xs*xp.ys;
    vp.xs = xp.xs;
    vp.ys = xp.ys;
    vp.zs = 1;
    vp.gp = (sWord*)malloc(vp.xs*vp.ys*vp.zs*sizeof(sWord));
    for (i=0; i<ps; i++) vp.gp[i] = 0;

    na = Nabra(xp);
    z  = xp.zs/2;
    for(y=xc; y<xp.ys-xc; y++) 
    for(x=xc; x<xp.xs-xc; x++) {
        cx  = z*ps + y*xs + x;
           nx  = na.gp[cx-ps*sw];
        nx2 = nx*nx;
        anx = Xabs(nx);
        dd  = 0.0;
        dz  = 0.0;
        for (zz=-zc; zz<=zc; zz++)
        for (yy=-xc; yy<=xc; yy++)
        for (xx=-xc; xx<=xc; xx++) {
            cp = kc + xx + yy*ms + zz*ms*ms;
            cw = cx + xx + yy*xs + zz*ps;

            if (cw==cx) df = 1.0;
            else df = exp(-((float)(na.gp[cw-ps*sw]-nx)+1.)/(nx2+1.));
            ux = xp.gp[cw];
            dg = mask.imask[cp];
            dd = dd + ux*dg*df;
            dz = dz + df*dg;
        }
        
        if (dz!=0.0) vp.gp[cx-z*ps] = (sWord)(dd/dz);
        else         vp.gp[cx-z*ps] = (sWord)(dd);
//        else         vp.gp[cx-z*ps] = (sWord)(dd.nf);
    }

    free(na.gp);

    return vp;
}


void   fi_mask(MSGraph<sWord> xp, FMask mask)
{
    int   i, j, x, y, cx, cy, lsize;
    int   xx, yy, cp, cw, ux;
    int   kc, xc, xs, ps;
    int   ms, nf, nx, nx2, anx;
    float   dd, dg, df, fx, dz;
    MSGraph<sWord> vp, na;

    ms = mask.msize;
    nf = mask.nfact;
    kc = ms*ms/2;
    xc = ms/2;
    xs = xp.xs;
    ps = xp.xs*xp.ys;
    vp.xs = xp.xs;
    vp.ys = xp.ys;
    vp.zs = xp.zs;
    lsize = vp.xs*vp.ys*vp.zs*sizeof(sWord);
    vp.gp = (sWord*)malloc(lsize);
    for (i=0; i<ps; i++) vp.gp[i] = 0;

    na = Nabra(xp);
    for(y=xc; y<xp.ys-xc; y++) 
    for(x=xc; x<xp.xs-xc; x++) {
        cx  = y*xs + x;
        nx  = na.gp[cx];
        nx2 = nx*nx;
        anx = Xabs(nx);
        dd  = 0.0;
        dz  = 0.0;
        for (yy=-xc; yy<=xc; yy++)
        for (xx=-xc; xx<=xc; xx++) {
            cp = kc + xx + yy*ms;
            cw = cx + xx + yy*xs;

            if      (cw==cx) df = 1.0;
            else df = exp(-((float)(na.gp[cw]-nx)*(na.gp[cw]-nx)+1.)/(nx2+1.));

            ux = xp.gp[cw];
            dg = mask.imask[cp];
            dd = dd + ux*dg*df;
        }
        vp.gp[cx] = (sWord)(dd/nf);
    }

    for (i=0; i<ps; i++) xp.gp[i] = vp.gp[i];
    free(vp.gp);
    free(na.gp);

    return;
}


MSGraph<sWord>  curvef(MSGraph<sWord> xp, int m) 
{
    MSGraph<sWord>  vp;

    if      (m==1) vp = WSCurve(xp, PIT,  3596);
    else if (m==2) vp = WSCurve(xp, PEAK, 4096);

    return vp;
}


#define EX 50

MSGraph<sWord>  xfilter(RgnStat ns, MSGraph<sWord> xp)
{
    int  i, j, sm, ex, ms, ma, rr, fm;
    WSGraph  vp, wp, zp;

    vp = edge_enhance(xp, 0);
    return vp;

    fm = FilterMode - 1;

    if (fm==1) {
        ex = -EX;
        ms = ns.mn;
    }
    else {
        ex = EX;
        ms = EMAXCT - ns.mn;
    }

    vp = make_WSGraph(xp.xs, xp.ys, 1);
    wp = curvef(xp, fm);
    zp = euclid_distance(wp, &rr, 1);

    for (i=0; i<wp.xs*wp.ys; i++) {
        if (wp.gp[i]!=0 && xp.gp[i]>ms) vp.gp[i] = xp.gp[i] + ex;
        else                            vp.gp[i] = xp.gp[i];
    }

    free(wp.gp);
    free(zp.gp);

    return vp;
}
*/

