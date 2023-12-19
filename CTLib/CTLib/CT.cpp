
/**
    CT操作用プログラム
*/

#include  "CT.h"

static int  Stack_Paint3D = 0;
static bool Stop_Paint3D  = false;


/**
int    get_fPoint(MSGraph<sWord> gd, double xx, double yy, double zz)

    機能：各座標（double）の輝度値を返す．補間法は線形補間．Z方向の歪補正あり．
    　　　補間法の制限によりZ=（Z方向の最大値）の平面の値は取れない（範囲外となる）．
    　　　座標値が範囲外の場合は gd->zero を返す．
*/
int    get_fPoint(MSGraph<sWord> gd, double xx, double yy, double zz)
{
    int    ix, iy, iz, ax, ay, az;
    int    ia, ib, ic, id, im, in, io, ip;
    double fa, fb, fc, fd, fm, fn, fo, fp;
    double fx, fy, fz;
    int    ret;

    ret = gd.zero;
    zz = (double)(zz*gd.RZxy);  // (ex. *0.4/2.0) 

    ix = (int)(xx) - gd.rbound.xmin;
    iy = (int)(yy) - gd.rbound.ymin;
    iz = (int)(zz) - gd.rbound.zmin;
    ax = ix + 1;
    ay = iy + 1;
    az = iz + 1;
    fx = xx - ix - gd.rbound.xmin;
    fy = yy - iy - gd.rbound.ymin;
    fz = zz - iz - gd.rbound.zmin;

    if (ix>=0&&ax<gd.xs&&iy>=0&&ay<gd.ys&&iz>=0&&az<gd.zs){ 
        ia = gd.gp[iz*gd.xs*gd.ys + iy*gd.xs + ix];
        ib = gd.gp[iz*gd.xs*gd.ys + iy*gd.xs + ax];
        ic = gd.gp[iz*gd.xs*gd.ys + ay*gd.xs + ix];
        id = gd.gp[iz*gd.xs*gd.ys + ay*gd.xs + ax];
        im = gd.gp[az*gd.xs*gd.ys + iy*gd.xs + ix];
        in = gd.gp[az*gd.xs*gd.ys + iy*gd.xs + ax];
        io = gd.gp[az*gd.xs*gd.ys + ay*gd.xs + ix];
        ip = gd.gp[az*gd.xs*gd.ys + ay*gd.xs + ax];
        fa = ia*(1.f-fx)*(1.f-fy)*(1.f-fz);
        fb = ib*     fx *(1.f-fy)*(1.f-fz);
        fc = ic*(1.f-fx)*     fy *(1.f-fz);
        fd = id*     fx *     fy *(1.f-fz);
        fm = im*(1.f-fx)*(1.f-fy)*     fz ;
        fn = in*     fx *(1.f-fy)*     fz ;
        fo = io*(1.f-fx)*     fy *     fz ;
        fp = ip*     fx *     fy *     fz ;
        ret = (int)(fa + fb + fc + fd + fm + fn + fo + fp + 0.5);
    }
    return ret;
}

