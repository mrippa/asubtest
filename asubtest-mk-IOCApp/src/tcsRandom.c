/*
*   FILENAME
*   tcsRandom.c
*
*   FUNCTION NAME(S)
*   tcsRandom - generate a pseudo-random real number in range 0 <= x < 1
*   tcsGresid - generate a pseudo-random normal deviate
*
*/
/* *INDENT-OFF* */
/*
 * $Log: tcsRandom.c,v $
 * Revision 1.2  2001/10/22 17:54:48  gemvx
 * *** empty log message ***
 *
 * Revision 1.1  2001/08/20 16:54:54  gemvx
 *
 * In this version only the "Double Buffer" option is allowed, in
 * order to avoid misunderstanding in the installation.
 * Modified Files:
 *  	src/misc/miscRoutines.c src/par/telescope.par
 *  	src/par/telescopeCP.par src/par/telescopeMK.par
 *  	src/sir/Makefile.Vx startup/.cvsignore startup/mv167st.vws
 *  	startup/mvstage.vws startup/startupCP.vws
 *
 * Added files are related to PMAC simulator.
 *
 * Besides that some modifications in the health tree were made.
 *
 *  	capfast/mcMiscHealth.sym capfast/pmacSimulator.sch
 *  	capfast/pmacSimulator.sym dl/Diag_dram.adl dl/mcHealthTree.adl
 *  	dl/mcMiscHealth.adl dl/mcMotorHealth.adl dl/mcTimeHealth.adl
 *  	dl/tstamp.adl src/misc/healthRoutines.c src/misc/simRoutines.c
 *  	src/misc/tcsRandom.c src/misc/tcsRandom.h src/misc/tcsServo.c
 *  	src/misc/tcsServo.h src/sir/mcHealthSIR.c
 *
 * Revision 1.1  2000/03/07 09:06:02  cjm
 * Noise generator copied from TCS
 *
 * Revision 1.1.1.1  1998/11/08 00:21:13  epics
 * Version 1-2 of the TCS as transferred from DRAL
 *
 * Revision 1.1  1997/02/10 17:10:56  pbt
 * Recreate in tcs subdirectory
 *
 * Revision 1.1  1996/05/23 20:34:48  cjm
 * Initial version
 *
 */
/* *INDENT-ON* */


#include "tcsRandom.h"
#include "slalib.h"
#include "slamac.h"
#include <stdlib.h>
#include <limits.h>
#ifndef RAND_MAX /* To fix problem with SunOS */
#define RAND_MAX 2147483647
#endif

/*+
 *   Function name:
 *   tcsRandom
 *
 *   Purpose:
 *   Generate pseudo-random real number in the range 0 <= x < 1
 *
 *   Description:
 *   Given an initial seed value, which is used to initialize a random
 *   number generator, the function returns a pseudo-random real number
 *   in the range 0 <= x < 1.
 *   This routine is an exact copy of slaRandom, a routine developed 
 *   by P. T. Wallace as part of the slalib library but now no longer
 *   included. 
 *
 *   Invocation:
 *   x = tcsRandom (seed)
 *
 *   Parameters: (">" input, "!" modified, "<" output)  
 *      (>)     seed    float      Seed value for random number generator
 *
 *   Function value:
 *   Real number in range 0 <= x < 1
 *-
 */


float tcsRandom ( float seed )

{
   double as;
   unsigned int is;
   static int ftf = 1;

   if ( ftf )
   {
      ftf = 0;
      as = fabs ( (double) seed );
      is = (unsigned int) gmax ( 1.0, as );
      srand ( gmin ( UINT_MAX, is ) );
   }
   return (float) rand() / (float) RAND_MAX;
}


/*+
 *   Function name:
 *   tcsGresid
 *
 *   Purpose:
 *   Generate pseudo-random normal deviate ( = "Gaussian residual")
 *
 *   Description:
 *   Generates a pseudo-random normal deviate.
 *   The results of many calls to this routine will be normally
 *   distributed with mean zero and standard deviation s.
 *
 *   The Box-Muller algorithm is used.  This is described in
 *   Numerical Recipes, Section 7.2.
 *
 *   This routine is an exact copy of slaGresid, a routine developed 
 *   by P. T. Wallace as part of the slalib library but now no longer
 *   included. 
 *
 *   Invocation:
 *   x = tcsGresid (s)
 *
 *   Parameters: (">" input, "!" modified, "<" output)  
 *      (>)     s       float      Standard deviation
 *
 *   Function value:
 *   Normal deviate
 * 
 *-
 */

float tcsGresid ( float s )

#define TINY 1e-30

{
   static float seed = 1;
   static int first = TRUE;
   static float x, y;
   float w;
   double rsq;


/* Second normal deviate of the pair available? */
   if ( first ) {

   /* No - generate random x,y in unit-radius circle */
      do {
         x = ( 2.0f * tcsRandom ( seed ) ) - 1.0f;
         y = ( 2.0f * tcsRandom ( seed ) ) - 1.0f;
         rsq = (double) ( x * x + y * y );
      } while ( rsq >= 1.0 );

   /* Box-Muller transformation, generating two deviates */
      w = ( rsq > TINY ) ? (float) sqrt ( - 2.0 * log ( rsq ) / rsq ) : 0.0f ;
      x *= w;
      y *= w;

   /* Set flag to indicate availability of next deviate */
      first = FALSE;

   } else {

   /* Return second deviate of the pair & reset flag */
      x = y;
      first = TRUE;
   }

/* Scale the deviate by the required standard deviation */
   return x * s;
}
