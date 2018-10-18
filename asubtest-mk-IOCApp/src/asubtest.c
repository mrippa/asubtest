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
//    epicsInt32 i, *a;
//    epicsInt32  sum=0;
//    
//    a = (epicsInt32 *)prec->a;
//    for (i=0; i<prec->noa; i++) {
//        sum += a[i];
//
//        if(mySubDebug)
//            printf("sum: %ld\n", sum);
//    }

    //*(epicsInt32 *) prec->val = sum;
    memcpy (prec->vala, prec->a, prec->noa * sizeof(epicsInt32 ));

    return 0; /* process output links */
}

/* -----------------------------------------------------------------*/
epicsExportAddress(int, mySubDebug);

epicsRegisterFunction(my_asubtx);
epicsRegisterFunction(my_asubrx);
epicsRegisterFunction(init_my_asub);

