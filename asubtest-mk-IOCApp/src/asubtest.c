#include <epicsStdlib.h>
#include <epicsStdioRedirect.h>
#include <registryFunction.h>
#include <epicsExport.h>
#include <string.h>
#include <iocsh.h> 
#include <math.h>
#include <time.h>
#include <epicsPrint.h>

#include <aSubRecord.h>

static epicsInt32 init_my_asub (aSubRecord *prec) {

    *(epicsInt32 *)prec->vala = 1;	/* first array at offset zero */
    return 0;
}

int mySubDebug = 0;
int mySubDebug1 = 0;
static epicsInt32 my_asubtx(aSubRecord *prec) {
//    epicsInt32  i, *ain, aout[10];
//    epicsInt32 sum=0;
    
//    ain = (epicsInt32 *)prec->a;
//    for (i=0; i<prec->noa; i++) {
//        sum += ain[i];
//        aout[i] = ain[i];
//
//        if(mySubDebug)
//            printf("sum: %ld\n", sum);
//    }

//    if(mySubDebug)
//        printf("Hello asubtx\n");
    //*(epicsInt32 *) prec->val = sum;
    memcpy (prec->vala, prec->a, prec->nova * sizeof(epicsInt32));

    return 0; /* process output links */
}

static epicsInt32 my_asubrx(aSubRecord *prec) {
    static epicsTimeStamp tnow, tprev;
    static double tdiff[10] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double tsum = 0.0;
    double tave = 0.0;
    double tstd = 0.0;
    static double tmin = 1e6;
    static double tmax= 0.0;
    static int first = 1, sample = 0;
    int i;

    memcpy (prec->vala, prec->a, prec->noa * sizeof(epicsInt32 ));

    epicsTimeGetCurrent(&tnow);
    if (first) {
        tprev = tnow;
        first = 0;
    }
    tdiff[sample] = epicsTimeDiffInSeconds(&tnow, &tprev);
    tprev = tnow;

    if (tdiff[sample] > tmax)
        tmax = tdiff[sample];

    if (tdiff[sample] < tmin)
        tmin = tdiff[sample];
    
    /*Run statistics for last element in array*/
    if (sample == (prec->noa)-1) {

        tsum = 0.0;
        for (i=0; i<=sample; i++) {
            tsum += tdiff[i];
            if (mySubDebug1)
                printf("tsum, %f, tdiff[%d] = %f\n, ", tsum, i, tdiff[i]);
        }

        tave = tsum/(sample+1);
        if (mySubDebug1)
            printf("tave = %f\n",  tave);


        for (i=0; i<=sample; i++) {
            tstd += (tdiff[i] - tave) * (tdiff[i] - tave);
        }
        tstd = sqrt(tstd/sample);
        sample = 0;

        if (mySubDebug)
            printf("mean: %f; max: %f, min: %f, sum: %f, std: %f\n", tave, tmax, tmin, tsum, tstd);

        *(double *) prec->valb = tsum;
        *(double *) prec->valc = tave;
        *(double *) prec->vald = tstd;
        *(double *) prec->vale = tmin;
        *(double *) prec->valf = tmax;
    }
    else {
        sample++;
    }

    *(double *) prec->valt = tdiff[sample];
    return 0; /* process output links */
}


static epicsInt32 tdiff_proc(aSubRecord *prec) {

    *(double *) prec->vala = *(double *) prec->a;
    *(double *) prec->valb = *(double *) prec->b;
    return 0; /* process output links */
}



/* -----------------------------------------------------------------*/
epicsExportAddress(int, mySubDebug);

epicsRegisterFunction(my_asubtx);
epicsRegisterFunction(my_asubrx);
epicsRegisterFunction(tdiff_proc);
epicsRegisterFunction(init_my_asub);

