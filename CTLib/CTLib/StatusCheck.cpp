
#include "StatusCheck.h"
#include "Tree.h"



///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
int    chk_region_status(RgnStat ns, RgnStat** gs, int nn, int sm, int md)
{
    if (nn==0)        return nn; 
    if (md==NO_CHECK) return nn; 

	int i;
    for (i=0; i<nn; i++) gs[i]->nr = true;
    if (md==ALL_OK) return nn;

    sm = Xabs(sm); 
    //ns.fg = Max(ns.fg, 1.0);

    if      (sm==BRONCHUS) chk_bronchus_region(ns, gs, nn, md);
    else if (sm==OBJECTS)  chk_objects_region (ns, gs, nn, md);
    else if (sm==BLOOD)    chk_blood_region   (ns, gs, nn, md);
    else {
		DEBUG_MODE print_message("CHK_REGION: エラー. 定義されていない対象モード = %d\n", sm);
		exit(1);
    }

	int k=0;
    for (i=0; i<nn; i++)  if (gs[i]->nr) k++;
    return k;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
void    chk_blood_region(RgnStat ns, RgnStat** gs, int nn, int md)
{
    int  i;

    if (md==NORMAL) { 
		for (i=0; i<nn; i++) {
			gs[i]->nr = inter_region_check(*GLMPgd, &ns, gs[i], BLOOD);
			//gs[i]->nr = next_region_check(ns, *gs[i], 10.);
			//if (!gs[i]->nr) gs[i]->at = RGN_ERR_OVLP;
		}
        chk_blood_unit(ns, gs, nn);
		chk_blood_glvl(ns, gs, nn);
	}

	else if (md==RELOOK) { 
		for (i=0; i<nn; i++) {
			if (i!=0) {
				gs[i]->nr = false;
				gs[i]->at = RGN_ERR_EXT;
			}
	    	//if (i==0) {
	        //	gs[i]->nr = next_region_check(ns, *gs[i], 50.);
    	    	//if (gs[i]->ss>ns.ss    ||gs[i]->ss<ns.ss/1.5) gs[i]->nr = false;
    	    	//if (gs[i]->fg>ns.fg+0.5||gs[i]->fg<ns.fg-0.5) gs[i]->nr = false;
                //if (gs[i]->ss<=SMALLEST_BLOOD) gs[i]->nr = false;
	    	//}
	    	//else gs[i]->nr = false;
		}
    }
    

    else if (md==INTER_PATH) {
		for (i=0; i<nn; i++) {
	    	if (i==0) {
				gs[i]->nr = inter_region_check(*GLMPgd, &ns, gs[i], BLOOD);
				//if (gs[i]->nr) gs[i]->nr = inter_duppath_check(*GLMPgx, &ns, gs[i]);

	        	//gs[i]->nr = next_region_check(ns, *gs[i], 0.01);
				//if (!gs[i]->nr) gs[i]->at = RGN_ERR_OVLP;
    	    	//if (gs[i]->ss>ns.ss*2||gs[i]->ss<ns.ss/3) gs[i]->nr = false;
    	    	//if (gs[i]->fg>ns.fg+1||gs[i]->fg<ns.fg-1) gs[i]->nr = false;
	    	}
			else {
				gs[i]->nr = false;
				gs[i]->at = RGN_ERR_EXT;
			}
		}
    }
    

/*	else if (md==INTER_CHECK) {
		for (i=0; i<nn; i++) {
			gs[i]->nr = next_region_check(ns, *gs[i], 0.01);
			if (!gs[i]->nr) gs[i]->at = RGN_ERR_OVLP;
		}
    }
*/

	else if (md==SPHERE_NEXT) {
   		for (i=0; i<nn; i++) {
			gs[i]->nr = inter_region_check(*GLMPgd, &ns, gs[i], BLOOD);
			if (gs[i]->nr) gs[i]->nr = inter_duppath_check(*GLMPgx, &ns, gs[i]);
   		}
		//chk_blood_unit(ns, gs, nn);
        //chk_blood_glvl(ns, gs, nn);
    }
   
	
	else if (md==SPHERE_CHK) {
   		for (i=0; i<nn; i++) {
	    	//if (gs[i]->fg>2.5) gs[i]->nr = false;
	    	//if (gs[i]->ss>ns.ss*2.0) gs[i]->nr = false;
			gs[i]->nr = inter_region_check(*GLMPgd, &ns, gs[i], BLOOD);
			if (gs[i]->nr) gs[i]->nr = inter_duppath_check(*GLMPgx, &ns, gs[i]);
   		}
        //chk_blood_unit(ns, gs, nn);
        //chk_blood_glvl(ns, gs, nn);

		/*if (ns.ss>200) {
	    	ss = 0;
	    	for (i=0; i<nn; i++) {
	   			if (gs[i]->nr==true) ss = ss + gs[i]->ss;
	    	}
	    	if (ss<ns.ss/2.0) {
	   			for (i=0; i<nn; i++) gs[i]->nr = false;
	    	}
		}*/
    }


/*
	else if (md==LABEL) {
		for (i=0; i<nn; i++) gs[i]->nr = next_region_check(ns, *gs[i], 30.);
        chk_blood_unit(ns, gs, nn);
        chk_blood_glvl(ns, gs, nn);
    }
    
	else if (md==PAINT_UP) {
		gs[0]->nr = false;
        if ((ns.ss/2.0<gs[0]->ss && ns.ss*2.0>gs[0]->ss) &&
			(gs[0]->fg<1.0+ns.fg && gs[0]->fg<3.0) ) gs[0]->nr = true;
    }
    
    
	else if (md==SPHERE_NEXT) {
        chk_blood_unit(ns, gs, nn);
        chk_blood_glvl(ns, gs, nn);
    }
    
	else if (md==SPHERE_CHK) {
   		for (i=0; i<nn; i++) {
	    	if (gs[i]->fg>2.5) gs[i]->nr = false;
	    	if (gs[i]->ss>ns.ss*2.0) gs[i]->nr = false;
   		}
        chk_blood_unit(ns, gs, nn);
        chk_blood_glvl(ns, gs, nn);

		if (ns.ss>200) {
	    	ss = 0;
	    	for (i=0; i<nn; i++) {
	   			if (gs[i]->nr==true) ss = ss + gs[i]->ss;
	    	}
	    	if (ss<ns.ss/2.0) {
	   			for (i=0; i<nn; i++) gs[i]->nr = false;
	    	}
		}
    }
	
    else if (md==INTER_P) {
		for (i=0; i<nn; i++) {
	    	if (i==0) {
	        	gs[i]->nr = next_region_check(ns, *gs[i], 0.01);
    	    	if (gs[i]->ss>ns.ss*2||gs[i]->ss<ns.ss/3) gs[i]->nr = false;
    	    	if (gs[i]->fg>ns.fg+1||gs[i]->fg<ns.fg-1) gs[i]->nr = false;
	    	}
	    	else gs[i]->nr = false;
		}
    }
	
    else if (md==THRESHOLD) {
		for (i=0; i<nn; i++) gs[i]->nr = next_region_check(ns, *gs[i], 30.);
        chk_blood_unit(ns, gs, nn);
    }
*/
	
    else {
		DEBUG_MODE print_message("CHK_BLOOD_REGION: 警告　定義されていない検査モード = %d\n",md);
    }
}



/**
	機能：一個一個の血管領域のチェック．

*/
void    chk_blood_unit(RgnStat ns, RgnStat** gs, int nn)
{
    for (int i=0; i<nn; i++) {
		if (gs[i]->nr) {
			if (ns.ss>100 && gs[i]->ss>ns.ss*2) {
				gs[i]->nr = false;
				gs[i]->at = RGN_ERR_BIG;
			}

			if (gs[i]->fg>3.0 || gs[i]->fg>ns.fg*2.) {
				gs[i]->nr = false;
				gs[i]->at = RGN_ERR_CMPLX;
			}
		}
		
        //if (gx->ss<ns.ss/20 || gx->ss>ns.ss*5) gs[i]->nr = false;
		//if (gx->fg>4.0 && ns.ss<200) gs[i]->nr = false;
		//if (gx->ss>1500) gs[i]->nr = false;
		//if (gx->ss<=SMALLEST_BLOOD) gs[i]->nr = false;
		//if (gs[i]->ss<20 && gs[i]->fg>1.8) gs[i]->nr = false;
    }
}



/**
	機能：同一の観測面で探し出した血管領域の全体としてのチェック．

*/
void    chk_blood_glvl(RgnStat ns, RgnStat** gs, int nn)
{
    int    ss;
	double fg;
	int    i;

	ss = 0;
	fg = 0.0;
	for (i=0; i<nn; i++) {
        if (gs[i]->nr) {
			ss = ss + gs[i]->ss;
			fg = fg + gs[i]->fg;
		}
    }

    if (ns.ss>100 && ss>ns.ss*5) {
        for (i=0; i<nn; i++) {
			if (gs[i]->nr) {
				gs[i]->nr = false;
				gs[i]->at = RGN_ERR_BIG;
			}
		}
    }

    if (fg>ns.fg*2.) {
        for (i=0; i<nn; i++) {
			if (gs[i]->nr) {
				gs[i]->nr = false;
				gs[i]->at = RGN_ERR_CMPLX;
			}
		}
    }

    return;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
//

void    chk_bronchus_region(RgnStat ns, RgnStat** gs, int nn, int md)
{
    int  i;

    if (md==NORMAL) { 
		for (i=0; i<nn; i++) {
			gs[i]->nr = inter_region_check(*GLMPgd, &ns, gs[i], BRONCHUS);
			//gs[i]->nr = next_region_check(ns, *gs[i], 5.);
			//if (!gs[i]->nr) gs[i]->at = RGN_ERR_OVLP;
		}
        //chk_bronchus_unit(ns, gs, nn);
        //chk_bronchus_glvl(ns, gs, nn);
	}

	else if (md==RELOOK) { 
		for (i=0; i<nn; i++) {
			if (i!=0) {
				gs[i]->nr = false;
				gs[i]->at = RGN_ERR_EXT;
			}
		}
    }
 
    else if (md==INTER_PATH) {
		for (i=0; i<nn; i++) {
	    	if (i==0) {
				gs[i]->nr = inter_region_check(*GLMPgd, &ns, gs[i], BRONCHUS);
				//if (gs[i]->nr) gs[i]->nr = inter_duppath_check(*GLMPgx, &ns, gs[i]);
	        	//gs[i]->nr = next_region_check(ns, *gs[i], 0.01);
				//if (!gs[i]->nr) gs[i]->at = RGN_ERR_OVLP;
    	    	//if (gs[i]->ss>ns.ss*2||gs[i]->ss<ns.ss/3) gs[i]->nr = false;
    	    	//if (gs[i]->fg>ns.fg+1||gs[i]->fg<ns.fg-1) gs[i]->nr = false;
	    	}
			else {
				gs[i]->nr = false;
				gs[i]->at = RGN_ERR_EXT;
			}
		}
    }
   
/*	else if (md==INTER_CHECK) {
		for (i=0; i<nn; i++) {
			//gs[i]->nr = next_region_check(ns, *gs[i], 0.01);
			//if (!gs[i]->nr) gs[i]->at = RGN_ERR_OVLP;
		}
    }
*/	
	else if (md==SPHERE_NEXT) {
   		for (i=0; i<nn; i++) {
			gs[i]->nr = inter_region_check(*GLMPgd, &ns, gs[i], BRONCHUS);
			if (gs[i]->nr) gs[i]->nr = inter_duppath_check(*GLMPgx, &ns, gs[i]);
   		}
        //chk_objects_unit(ns, gs, nn);
        //chk_objects_glvl(ns, gs, nn);
    }

	else if (md==SPHERE_CHK) {
   		for (i=0; i<nn; i++) {
	    	//if (gs[i]->fg>2.5) gs[i]->nr = false;
	    	//if (gs[i]->ss>ns.ss*2.0) gs[i]->nr = false;
			gs[i]->nr = inter_region_check(*GLMPgd, &ns, gs[i], BRONCHUS);
			if (gs[i]->nr) gs[i]->nr = inter_duppath_check(*GLMPgx, &ns, gs[i]);
   		}
        //chk_bronchus_unit(ns, gs, nn);
        //chk_bronchus_glvl(ns, gs, nn);

		/*if (ns.ss>100) {
	    	ss = 0;
	    	for (i=0; i<nn; i++) {
	   			if (gs[i]->nr==true) ss = ss + gs[i]->ss;
	    	}
	    	if (ss<ns.ss/2.0 || ss>ns.ss*2) {
	   			for (i=0; i<nn; i++) gs[i]->nr = false;
	    	}
		}*/
    }

/*
	if (md==NORMAL) {
		for (i=0; i<nn; i++) gs[i]->nr = next_region_check(ns, *gs[i], 30.);
        //chk_bronchus_unit(ns, gs, nn);
        //chk_bronchus_glvl(ns, gs, nn);
    }


	else if (md==RELOOK) { 
		for (i=0; i<nn; i++) {
	    	if (i==0) {
	        	//gs[i]->nr = next_region_check(ns, *gs[i], 30.);
    	    	//if (gs[i]->ss>ns.ss    ||gs[i]->ss<ns.ss/1.5) gs[i]->nr = false;
    	    	//if (gs[i]->fg>ns.fg+0.5||gs[i]->fg<ns.fg-0.5) gs[i]->nr = false;
                //if (gs[i]->ss<=SMALLEST_BRONCHUS) gs[i]->nr = false;
	    	}
	    	else gs[i]->nr = false;
		}
    }
    
/*
    else if (md==PRE_SCAN_TRACHEA) { 
        for (i=0; i<nn; i++) {
	    	gs[i]->nr = true;
            if (gs[i]->fg>2.5)  gs[i]->nr = false;
            //if (gs[i]->ss*RZxy*RZxy<100 || gs[i]->ss*RZxy*RZxy>2000) gs[i]->nr=false;
            //fprintf(stderr,"%3d   %5.3f %5d %5d -> %d\n",
            //         i, gs[i]->fg, gs[i]->ss, (int)(gs[i]->ss*RZxy*RZxy), gs[i]->nr);
		}
    }
    
    else if (md==LABEL) {
		for (i=0; i<nn; i++) gs[i]->nr = next_region_check(ns, *gs[i], 5.);
        chk_objects_unit(ns, gs, nn);
        chk_objects_glvl(ns, gs, nn);
    }

    else if (md==PAINT_UP) {
		gs[0]->nr = false;
        if ((ns.ss/2.0<gs[0]->ss&&ns.ss*2.0>gs[0]->ss) &&
			(gs[0]->fg<1.0+ns.fg&&gs[0]->fg<3.0) ) gs[0]->nr = true;
    }

    
	else if (md==SPHERE_NEXT) {
        chk_objects_unit(ns, gs, nn);
        chk_objects_glvl(ns, gs, nn);
    }
    
	else if (md==SPHERE_CHK) {
   		for (i=0; i<nn; i++) {
	    	if (gs[i]->fg>2.5) gs[i]->nr = false;
	    	if (gs[i]->ss>ns.ss*2.0) gs[i]->nr = false;
   		}
        chk_bronchus_unit(ns, gs, nn);
        chk_bronchus_glvl(ns, gs, nn);

		if (ns.ss>100) {
	    	ss = 0;
	    	for (i=0; i<nn; i++) {
	   			if (gs[i]->nr==true) ss = ss + gs[i]->ss;
	    	}
	    	if (ss<ns.ss/2.0 || ss>ns.ss*2) {
	   			for (i=0; i<nn; i++) gs[i]->nr = false;
	    	}
		}
    }
    
	else if (md==THRESHOLD) {
		for (i=0; i<nn; i++) gs[i]->nr = next_region_check(ns, *gs[i], 5.);
        chk_bronchus_unit(ns, gs, nn);
    }
*/	
    else {
		DEBUG_MODE print_message("CHK_BRONCHUS_REGION: 警告　定義されていない検査モード = %d\n",md);
    }
}



void    chk_bronchus_unit(RgnStat ns, RgnStat** gs, int nn)
{
    int    i;
    //double cf, dd;
	
    for (i=0; i<nn; i++) {
        if (ns.ss>50 && (gs[i]->ss>ns.ss*2.0 || gs[i]->ss<ns.ss/10)){
            gs[i]->nr = false;
        }

        if ((gs[i]->ss>ns.ss*2.0 || gs[i]->ss<ns.ss/10)){
            gs[i]->nr = false;
        }

        if (gs[i]->fg>ns.fg+1.0 || gs[i]->fg<0.5){
            gs[i]->nr = false;
        }

        if (ns.ss<=100 && (gs[i]->fg>2.0||gs[i]->fg>ns.fg+1.0)) {
            gs[i]->nr = false;
        }


//        if (gs[i]->ss<50 && gs[i]->fg>2.0) {
////	    	gs[i]->nr = false;
 //       }


        if (gs[i]->ss<=SMALLEST_BRONCHUS) {
            gs[i]->nr = false;
        }
    }
    return;
}



void    chk_bronchus_glvl(RgnStat ns, RgnStat** gs, int nn)
{
    int  i, ss;
    double  fg;

    for (fg=0,i=0; i<nn; i++) {
        if (gs[i]->nr==true) {
            fg += Max(1.f, gs[i]->fg);
            if (i!=0 && fg>ns.fg+2.f) {
                fg -= Max(1.f, gs[i]->fg);
                gs[i]->nr = false;
            }
        }
    }

    for (ss=0,i=0; i<nn; i++) {
        if (gs[i]->nr==true) {
            ss += gs[i]->ss;
        }
    }

    if (ns.ss>ss*5 || ns.ss*2.0<ss) {
        for (i=0; i<nn; i++) gs[i]->nr = false;
    }

    for (i=0; i<nn; i++) {
        if (gs[i]->ss<20 && gs[i]->fg>1.8) gs[i]->nr = false;
    }

    return;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//

void    chk_objects_region(RgnStat ns, RgnStat** gs, int nn, int md)
{
	int  i;

    if (md==NORMAL) { 
		for (i=0; i<nn; i++) {
			gs[i]->nr = next_region_check(ns, *gs[i], 10.);
			if (!gs[i]->nr) gs[i]->at = RGN_ERR_OVLP;
		}
        //chk_bronchus_unit(ns, gs, nn);
        //chk_bronchus_glvl(ns, gs, nn);
	}

	else if (md==RELOOK) { 
		for (i=0; i<nn; i++) {
			if (i!=0) {
				gs[i]->nr = false;
				gs[i]->at = RGN_ERR_EXT;
			}
		}
    }
/*
    if (md==NORMAL) { 
        for (i=0; i<nn; i++) {
	    	gs[i]->nr = true;
	    	if (gs[i]->ss>15) gs[i]->nr = next_region_check(ns, *gs[i], 5.);
            if (gs[i]->ss<=SMALLEST_OBJECTS) gs[i]->nr = false;
		}
    }

    else if (md==PRE_SCAN_OBJECTS) { 
        for (i=0; i<nn; i++) {
	    	gs[i]->nr = true;
            //if (gs[i]->ss*RZxy*RZxy<30 || gs[i]->ss*RZxy*RZxy>3000) gs[i]->nr=false;
            //fprintf(stderr,"%3d   %5.3f %5d %5d -> %d\n",
            //            i,gs[i]->fg,gs[i]->ss,(int)(gs[i]->ss*RZxy*RZxy),gs[i]->nr);
		}
    }

    else if (md==LABEL) {
        chk_objects_unit(ns, gs, nn);
        chk_objects_glvl(ns, gs, nn);
    }

    else if (md==SPHERE_NEXT) {
        chk_objects_unit(ns, gs, nn);
        chk_objects_glvl(ns, gs, nn);
    }

    else if (md==SPHERE_CHK) {
        chk_objects_unit(ns, gs, nn);
        chk_objects_glvl(ns, gs, nn);
    }

    else if (md==INTER_P) { 
    	for (i=0; i<nn; i++) {
	    	if (gs[i]->ss>ns.ss*2  || gs[i]->ss<ns.ss/2.) gs[i]->nr = false;
	    	if (gs[i]->fg>ns.fg+1. || gs[i]->fg<ns.fg-1.) gs[i]->nr = false;
		}
    }

    else if (md==RELOOK) {
        for (i=0; i<nn; i++) {
            if (i==0) {
				gs[i]->nr = next_region_check(ns, *gs[i], 30.);
                //if (gs[i]->ss>ns.ss    ||gs[i]->ss<ns.ss/1.5) gs[i]->nr = false;
                //if (gs[i]->fg>ns.fg+0.5||gs[i]->fg<ns.fg-0.5) gs[i]->nr = false;
                //if (gs[i]->ss<=SMALLEST_OBJECTS) gs[i]->nr = false;
            }
            else gs[i]->nr = false;
        }
    }

    else if (md==THRESHOLD) {
		for (i=0; i<nn; i++) gs[i]->nr = next_region_check(ns, *gs[i], 10.);
        chk_objects_unit(ns, gs, nn);
    }
*/
    else {
		DEBUG_MODE print_message("CHK_OBJECTS_REGION: 警告　定義されていない検査モード = %d\n",md);
    }

}



void    chk_objects_unit(RgnStat ns, RgnStat** gs, int nn)
{
	int  i;

    for (i=0; i<nn; i++) {
        if (ns.ss>100 && (gs[i]->ss>ns.ss*2.0 || gs[i]->ss<ns.ss/10)){
            gs[i]->nr = false;
        }

        if (gs[i]->fg>ns.fg+2.0 || gs[i]->fg<0.5){
            gs[i]->nr = false;
        }

        if (ns.ss<=100 && (gs[i]->fg>2.5||gs[i]->fg>ns.fg+1.0)) {
            gs[i]->nr = false;
        }

        if (gs[i]->ss<50 && gs[i]->fg>2.0) {
	    	gs[i]->nr = false;
        }

        if (gs[i]->ss<=SMALLEST_OBJECTS) {
            gs[i]->nr = false;
        }
    }
    return;
}



void    chk_objects_glvl(RgnStat ns, RgnStat** gs, int nn)
{
    int  i, ss;
    double  fg;

    for (fg=0,i=0; i<nn; i++) {
        if (gs[i]->nr==true) {
            fg += Max(1.f, gs[i]->fg);
            if (i!=0 && fg>ns.fg+2.f) {
                fg -= Max(1.f, gs[i]->fg);
                gs[i]->nr = false;
            }
        }
    }

    for (ss=0,i=0; i<nn; i++) {
        if (gs[i]->nr==true) {
            ss += gs[i]->ss;
        }
    }

    if (ns.ss>ss*5 || ns.ss*2.0<ss) {
        for (i=0; i<nn; i++) gs[i]->nr = false;
    }

    for (i=0; i<nn; i++) {
        if (gs[i]->ss<20 && gs[i]->fg>1.8) gs[i]->nr = false;
    }

    return;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//

bool  inter_region_check(MSGraph<sWord> gd, RgnStat* ps, RgnStat* ns, int sm)
{
    int     nn, mm, pm;
	double  xx, yy, zz;
	Vector<double>  dd, dr;

	dr.set(ns->xx-ps->xx, ns->yy-ps->yy, ns->zz-ps->zz);
    nn = ((int)dr.n + 1)*2;
	//nn = (int)(dr.n*2) + 1;
	dd = dr/nn;

    if (Xabs(sm)==BRONCHUS) mm = Max(ns->mm, ps->mm);
	else                    mm = Min(ns->mm, ps->mm);

    for (int i=1; i<=nn-1; i++) {
		xx = ps->xx + dd.x*i;
		yy = ps->yy + dd.y*i;
		zz = ps->zz + dd.z*i;
		pm = get_fPoint(gd, xx, yy, zz);

		if (Xabs(sm)==BRONCHUS) {
			if (pm<ps->ml || pm>mm) return false;
		}
		else if (Xabs(sm)==BLOOD) {
			if (pm>ps->mh || pm<mm) return false;
		}
		else {
			if (pm>ps->mh || pm<ps->ml) return false;
		}
	}

    return  true;
}



bool  inter_duppath_check(MSGraph<sWord> gx, RgnStat* ps, RgnStat* ns)
{
    int     nx=0;
	Vector<double>  dd, dr;


/*	dr.set(ns->xx-ps->xx, ns->yy-ps->yy, ns->zz-ps->zz);
    nn = ((int)dr.n + 1)*2;
	dd = dr/nn;

    for (int i=1; i<=nn-1; i++) {
		xx = ps->xx + dd.x*i;
		yy = ps->yy + dd.y*i;
		zz = ps->zz + dd.z*i;
		pm = get_fPoint(gx, xx, yy, zz);
		if (pm!=gx.zero) nx++;
	}

	
	double rate = (double)nx/(nn-1);
	if (rate>0.6) return false;	// 経路は抽出済み
*/
	return  true;
}



bool    next_region_check(RgnStat ns, RgnStat gs, double p)
{
    int  m, c;

    m = (int)(gs.ss*p/100.);
    c = overlap_region_check(ns, gs, m);

    if (c>m) return true;
    else     return false;
}



int    overlap_region_check(RgnStat ns, RgnStat gs, int limit)
{
    int  x, y, xx, yy, cnt, sx, sy;
    int  co, cp, cx;
	bool bf;

    sx = gs.sx - (ns.vs - ns.xs)/2; 
    sy = gs.sy - (ns.vs - ns.ys)/2;

    bf = false;
    for (cnt=0,y=1; y<gs.ys-1; y++){
 	    co = y*gs.xs;
    	for (x=1; x<gs.xs-1; x++){
 	    	cp = co + x;
	    	if (gs.gp[cp]!=gs.zr){
	    		xx = sx + x;
	    		yy = sy + y;
	    		if (xx>=1 && yy>=1 && xx<ns.xs-1 && yy<ns.ys-1){
		    		cx = yy*ns.xs + xx;
		    		if (ns.gp[cx]!=ns.zr){
						cnt++;
						if (cnt>limit) {
			    			bf = true;
			    			break;
						}
		    		}
				}
	    	}
		}
		if (bf) break;
    } 

    return cnt;
}




/**
double  loop_region_check(MSGraph<sWord> gx, RgnStat* gs, double rx) 

	機能：gsが既に抽出した親枝の領域とどれくらい重なっているかチェックする．
		　重なりが rx以上の場合は gs->lp に true を設定．
		　gs->u3 にも重複領域の相手の枝番号が入る．
	
	引数：	gx : グローバルな抽出結果空間
			gs : 領域データへのポインタ
			rx : gsがループであるとみなす割合．(0<rx<1)

	戻り値： 重複度

*/
double  loop_region_check(MSGraph<sWord> gx, RgnStat* gs, double rx) 
{
	int		i, j, cc, cj, ss;   
	int		sx, bn;

    Vector<double>  vx;
    double cst, snt, csf, snf;
    double py, pz, xx, yy, zz; 

    vx.set(gs->px, gs->py, gs->pz);
    ToPola(vx, cst, snt, csf, snf);

	bn = 0;
    sx = 0;
    for (j=0; j<gs->ys; j++) {
		pz = (double)(gs->wy - j);
		cj = gs->xs*j;
		for (i=0; i<gs->xs; i++) {
			cc = gs->gp[cj + i];
			if (cc!=gs->zr) {
				py = (double)gs->wx - i;
				xx = - py*snf - pz*cst*csf + gs->xx;
				yy =   py*csf - pz*cst*snf + gs->yy;
				zz =            pz*snt     + gs->zz;
				ss = gx.get_vPoint(xx, yy, zz);
				if (ss!=gx.zero) {
					bn = get_bn_from_color(ss);
					if (bn>0) {
						dupBranch.set(bn);
						sx++;
					}
				}
			}
		}
	}


//	gs->ob = 0;
    gs->ov = ((double)sx)/gs->ss;
//	gs->ov = 0;
    if (gs->ov>=rx) {
		gs->op = true;
		gs->on++;
		gs->ob = bn;		// 相手の枝（複数ある場合はそのうちの一つ）
	}
    else {
		gs->op = false;
	}
    
	return gs->ov;
}


