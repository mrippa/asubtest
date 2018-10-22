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
#define NSAMPLES 200

static epicsFloat64 tdiff[NSAMPLES];
static epicsInt32 init_my_asub (aSubRecord *prec) {

    *(epicsInt32 *)prec->vala = 1;	/* first array at offset zero */
    return 0;
}

static epicsInt32 init_asubrx (aSubRecord *prec) {

    int i;

    *(epicsInt32 *)prec->vala = 1;	/* first array at offset zero */

    for(i=0; i<NSAMPLES; i++)
        tdiff[i] = 0.0;

    return 0;
}

int mySubDebug = 0;
int mySubDebug1 = 0;
static epicsInt32 my_asubtx(aSubRecord *prec) {
    
    memcpy (prec->vala, prec->a, prec->nova * sizeof(epicsInt32));

    return 0; /* process output links */
}

static epicsInt32 my_asubrx(aSubRecord *prec) {
    static epicsTimeStamp tnow, tprev;
    epicsFloat64 tsum = 0.0;
    epicsFloat64 tave = 0.0;
    epicsFloat64 tstd = 0.0;
    static epicsFloat64 tmin = 1e6;
    static epicsFloat64 tmax= 0.0;
    static int first = 1, sample = 0;
    int i;
    epicsFloat64 output[5] = {0.0, 0.0, 0.0, 0.0, 0.0};

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

    if (tdiff[sample] >= 0.0 && tdiff[sample] < tmin)
        tmin = tdiff[sample];
    
    /*Run statistics for last element in array*/
    if (sample == NSAMPLES-1) {

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
        tstd = sqrt(tstd/(sample+1));
        sample = 0;

        if (mySubDebug)
            printf("mean: %f; max: %f, min: %f, sum: %f, std: %f\n", tave, tmax, tmin, tsum, tstd);

        output[0] = tsum;
        output[1] = tave;
        output[2] = tstd;
        output[3] = tmin;
        output[4] = tmax;

        memcpy (prec->valb, output, sizeof(output) * sizeof(epicsFloat64 ));
        tmin = 1e6;
        tmax = 0.0;

        *(epicsFloat64 *) prec->valt = tdiff[sample];
        return 0; /* process output links */
 
    }
    else {
        sample++;
    }

    return -1; /* DON'T process output links */
}

static epicsInt32 tdiff_proc(aSubRecord *prec) {

    memcpy (prec->vala, prec->a, prec->noa * sizeof(epicsFloat64 ));
    return 0; /* process output links */
}



/* -----------------------------------------------------------------*/
epicsExportAddress(int, mySubDebug);

epicsRegisterFunction(my_asubtx);
epicsRegisterFunction(my_asubrx);
epicsRegisterFunction(tdiff_proc);
epicsRegisterFunction(init_my_asub);
epicsRegisterFunction(init_asubrx);

