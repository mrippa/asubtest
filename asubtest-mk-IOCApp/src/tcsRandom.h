/*
*   FILENAME
*   tcsRandom.h
*
*   DESCRIPTION
*   Header file for tcsRandom.c which contains ex slalib routines used
*   to generate random numbers etc.
*
*/
/* *INDENT-OFF* */
/*
 * $Log: tcsRandom.h,v $
 * Revision 1.1  2001/08/20 16:54:55  gemvx
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
 * Revision 1.1  2000/03/07 09:03:45  cjm
 * Header file for tcsRandom.c
 *
 * Revision 1.1.1.1  1998/11/08 00:21:13  epics
 * Version 1-2 of the TCS as transferred from DRAL
 *
 * Revision 1.1  1997/02/10 17:10:57  pbt
 * Recreate in tcs subdirectory
 *
 * Revision 1.1  1996/05/23 20:33:42  cjm
 * Initial version
 *
 */
/* *INDENT-ON* */

#ifndef TCSRANDOMH
#define TCSRANDOMH

float tcsRandom( float seed) ;

float tcsGresid( float s ) ;

#endif
