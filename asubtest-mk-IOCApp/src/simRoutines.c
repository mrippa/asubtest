/*
*   FILENAME
*   simRoutines.c 
*
*   FUNCTION NAME(S)
*   mcCWPmacSimulate - simulate the behaviour of the cable wrap
*   mcGetSimuLevel   - fetch current simulation level 
*   mcInitSimMode    - set the simulation mode at initialisation time
*   mcPmacSimulate   - simulate PMAC card when none are present
*   mcSetSimuLevel   - set simulation level 
*   mcSimStartDatumS - trigger a simulated Start datum command using Sony switch
*   mcSimEndDatumS   - trigger a simulated Stop datum command using Sony switch
*   mcSimFollow      - trigger follow mode in the simulator
*   mcSimMove        - trigger a simulated move command
*   mcSimStop        - trigger a simulated stop command
*   mcSimulationMode - manipulate simulation mode 
*
*/
/* *INDENT-OFF* */
/*
 *
 */
/* *INDENT-ON* */


#include <epicsStdlib.h>
#include <epicsStdioRedirect.h>
#include <epicsExport.h>
#include <registryFunction.h>
#include <string.h>
#include <iocsh.h> 
#include <math.h>
#include <time.h>
#include <epicsPrint.h>

#include <dbEvent.h>
#include <dbDefs.h>
#include <dbCommon.h>
#include <recSup.h>
#include <subRecord.h>
#include <genSubRecord.h>
#include <sirRecord.h>
//#include <mcscad.h>
#include <mcs.h>
//#include <drvPmac.h>

//#include <cadRecord.h>
//#include <cad.h>
//#include <menuCarstates.h> 
#include <timeLib.h>

#include <tcsServo.h>

#define SIM_STOPPED 0
#define SIM_MOVING  1
#define SIM_FOLLOWING 2

#define ACTIVATE_MOTOR       0x00800000
#define ASSIGN_CS            0x00800000
#define SET_NEG_LIMIT        0x00400000
#define CLEAR_NEG_LIMIT     ~0x00400000
#define SET_POS_LIMIT        0x00200000
#define CLEAR_POS_LIMIT     ~0x00200000
#define SET_OPEN_LOOP        0x00040000
#define CLEAR_OPEN_LOOP     ~0x00040000
#define SET_DEFTIME_MOVE     0x00020000
#define CLEAR_DEFTIME_MOVE  ~0x00020000
#define AMP_ENABLED          0x00004000
#define SET_VELOCITY_ZERO    0x00002000
#define CLEAR_VELOCITY_ZERO ~0x00002000
#define HOME_COMPLETE        0x00000400
#define SET_IN_POSITION      0x00000001
#define CLEAR_IN_POSITION   ~0x00000001

#define PMAC_POS_LIMIT      270.0*3600.0
#define PMAC_NEG_LIMIT     -270.0*3600.0

#define WRAP_POS_LIMIT        5.0*3600.0
#define WRAP_NEG_LIMIT       -5.0*3600.0

#define TWO_POWER_24        16777216

int pvload( char *, char *, int, int );
void pvloadTask( int, int );

/*static char cwFiles[4][16] =
  { "CWPOS1", "CWPOS2", "CWPOS3", "END" }; */

static int intSimulate = 0 ;        /* Internal simulation flag */
static int simDatumCommand = 0 ;    /* Flag for a simulated datum command */
static int simDatumAzS = 0 ;    /* Flag for a simulated Az datum command using 
                                   sony switch */
static int simDatumElS = 0 ;    /* Flag for a simulated El datum command using 
                                   sony switch */
static int simEndDatumAzS = 0 ; /* Flag for a simulated Az End datum command 
                                   using sony switch */
static int simEndDatumElS = 0 ; /* Flag for a simulated El End datum command 
                                   using sony switch */

static double paTargetCnts ;          /* Demand position for moves (arcsec) */
static int stateMCS = SIM_STOPPED ;   /* Mount is initially stopped */


/*+
 *   Function name:
 *   mcCWPmacSimulate
 *
 *   Purpose:
 *   Simulate the Cass Wrap PMAC card when one isn't available
 *
 *   Desmciption:
 *   This routine is a specialisation of the mcPmacSimulate routine for
 *   the wrap. 
 *
 *   Invocation:
 *   mcCWPmacSimulate(pgsub)
 *
 *   Parameters: (">" input, "!" modified, "<" output)  
 *      (!)   pgsub   (struct genSubRecord *)  Pointer to gensub structure
 *
 *   Function value:
 *   (<)  status  (long)  Return status, 0 = OK
 * 
 *   Epics inputs:
 *   a => Servo noise (arcsec)
 *   b => Current maximum velocity (deg/s)
 *   c => Follow demand (counts)
 *   d => countsPerDeg 
 *   f => servoFreq
 *
 *   Epics outputs:
 *   valb => current position in encoder counts
 *   valc => PMAC demand position in encoder counts 
 *   vald => current velocity in encoder counts 
 *   vale => PMAC demand velocity in encoder counts
 *   valf => PMAC following error
 *   valg => Motor status word 1 for mount
 *   valh => Motor status word 2 for mount
 *
 *-
 */

long mcCWPmacSimulate (struct genSubRecord *pgsub) 
{

  static int first = TRUE ;               /* Flag to initialise routine */

  static double pa ;                      /* Current position angle */
  static double pav ;                     /* Current velocity */
  static double paerr ;                   /* PA error */

  static struct ServoState servoState ;
  static struct ServoConfig servoConfig ;

  double now ;                            /* Current time (TAI) */
  int    inPosition ;                     /* In position flag */
  double pademand ;                       /* Current demand (arcsec) */
  static double paTarget ;                /* Target posn (arcsecs) */
  double countsPerDeg ;                   /* Conversion for input/output */
  double servoFreq ;                      /* Servo frequency */
  static int motorStat1 = 0 ;             /* Motor status word */
  static int motorStat2 = 0 ;             /* Motor status word */

  static int  gotTime0 = 0 ;

/* Initialise the servo on the first pass. Note all positions are in arcsec
*  and velocities in arcsec/sec
*/
  if (first) {
    motorStat1 |= SET_OPEN_LOOP ;
    servoConfig.dvmax = 360.0 ;
    servoConfig.gs    = 20.0  ;
    servoConfig.gp    = 4.0 ;            /* In the tor we use 5.0 here */
    servoConfig.gi    = 0.5 ;
    servoConfig.pi    = 1.0 ;
    servoConfig.pok   = 0.2 ; 
    servoConfig.tok   = 0.1 ;
    servoState.pdtold = 0.0 ;             /* In simulation always start at 0 */
    timeNow (&(servoState.told) ) ;
    servoState.told -= 0.05 ;
    servoState.tout = servoState.told ;
    servoState.vd   = 0.0 ;
    servoState.accum = 0.0 ;
    pa = servoState.pdtold ;
    pav = 0.0 ;
    paTarget = pa ;
    first = FALSE ;
  }

/* Fetch input data */
  servoConfig.noise = *(double *)pgsub->a ;
  servoConfig.vtmax = *(double *)pgsub->b * 3600.0 ;
  pademand          = *(double *)pgsub->c * 3600.0 / 
                       (MCDEG_TO_CNTS * ECT * DEF_I108) ; /* Convert to " */
  countsPerDeg      = *(double *)pgsub->d ;
  servoFreq         = *(double *)pgsub->f ;

/* Send demand to servo */
  timeNow (&now) ;

/* Time is often not available for the first few calls to this routine so
*  loop until a sensible value is read 
*/
  if (!gotTime0) {
    if (now > 1000.0) {
      servoState.told = now - 0.05 ;
      servoState.tout = servoState.told ;
      gotTime0 = 1 ;
    }
  } 
    
  if (stateMCS == SIM_MOVING) {

    motorStat2 &= CLEAR_IN_POSITION ;
    motorStat1 &= CLEAR_VELOCITY_ZERO ;
    motorStat1 |= SET_DEFTIME_MOVE ;
    paTarget = pademand ;
    inPosition = tcsServo (now, paTarget, 0, &servoConfig, &servoState,
         &pa, &pav ) ;
    paerr = paTarget - pa ;

  } else if (stateMCS == SIM_FOLLOWING) {

    motorStat1 &= CLEAR_DEFTIME_MOVE ;
    motorStat1 &= CLEAR_VELOCITY_ZERO ;
    paTarget = pademand ;
    inPosition = tcsServo (now, paTarget, 0, &servoConfig, &servoState,
         &pa, &pav ) ;
    paerr = paTarget - pa ;
    if (inPosition) {
      motorStat2 |= SET_IN_POSITION ;
    } else {
      motorStat2 &= CLEAR_IN_POSITION ;
    }

  } else {
    paTarget = pademand ;
    inPosition = tcsServo (now, paTarget, 1, &servoConfig, &servoState,
         &pa, &pav ) ;
    paerr = paTarget - pa ;
    if (inPosition) {
      motorStat1 &= CLEAR_DEFTIME_MOVE ;
      motorStat2 |= SET_IN_POSITION ;
      motorStat1 |= SET_VELOCITY_ZERO ;
    }
  }
    
  motorStat1 |= ACTIVATE_MOTOR ;
  motorStat2 |= ASSIGN_CS ;

  if (pa >= WRAP_POS_LIMIT)
    motorStat1 |= SET_POS_LIMIT ;
  else
    motorStat1 &= CLEAR_POS_LIMIT ;

  if (pa <= WRAP_NEG_LIMIT)
    motorStat1 |= SET_NEG_LIMIT ;
  else
    motorStat1 &= CLEAR_NEG_LIMIT ;

/* Output data for EPICS */
  *(double *)pgsub->valb = pa * countsPerDeg * ECT * DEF_I508 / 3600.0 ;
  *(double *)pgsub->valc = paTarget * countsPerDeg * ECT * DEF_I508 / 3600.0 ;
  *(double *)pgsub->vald = pav * ECT * DEF_I509 *VEL_CNTS_PER_DEG /
                           (servoFreq * 3600.0) ;
  *(double *)pgsub->vale = servoState.vd * ECT * DEF_I509 *VEL_CNTS_PER_DEG /
                           (servoFreq * 3600.0) ;
  *(double *)pgsub->valf = paerr * countsPerDeg * ECT * DEF_I508 / 3600.0 ;
  *(long *)pgsub->valg = (long)motorStat1 ;
  *(long *)pgsub->valh = (long)motorStat2 ;

  return 0 ;

}

/*+
 *   Function name:
 *   mcGetSimuLevel
 *
 *   Purpose:
 *   Fetch the current simulation level of the Mount
 *
 *   Desmciption:
 *   Simply return the simulation level flag
 *
 *   Invocation:
 *   simLevel = mcGetSimuLevel()
 *
 *   Parameters: (">" input, "!" modified, "<" output)  
 *   None
 *
 *   Function value:
 *   (<)  simLevel  (int) Simulation level 0 = No simulation 
 * 
 *-
 */

int mcGetSimuLevel (void)
{

  return intSimulate ;

}

/*+
 *   Function name:
 *   mcInitSimMode
 *
 *   Purpose:
 *   Set the simulation mode at initialisation time
 *
 *   Desmciption:
 *   EPICS records have a field SIMM that determines whether they operate
 *   in simulation mode or not. This routine sets this mode at initialisation
 *   time
 *
 *   Invocation:
 *   mcInitSimMode(pgsub)
 *
 *   Parameters: (">" input, "!" modified, "<" output)  
 *      (!)    pgsub    (struct genSubRecord *)  Pointer to genSub structure
 *
 *   Epics inputs:
 *   a => demand simulation level
 *
 *   Epics outputs:
 *   vala => simulation level
 *   valb => current simulation mode
 *
 *   Function value:
 *   (<)  status  (long)  Return status, 0 = OK
 * 
 *-
 */

long mcInitSimMode (struct genSubRecord *pgsub) 
{

  long demandSimLevel ;

  demandSimLevel = *(long *)pgsub->a ;

  if (mcGetSimuLevel()) {

/* Force the simulation mode to YES and check that the level is valid. If
*  it isn't then set it to FULL
*/

    *(long *)pgsub->valb = 1 ;
    if (demandSimLevel < MCS_FULL || demandSimLevel > MCS_FAST)
      demandSimLevel = MCS_FULL ;
    *(long *)pgsub->vala = demandSimLevel ;
  } else {

/* If not simulating then set the mode to No and force the level to be
*  NONE irrespective of what the demand level is
*/
    *(long *)pgsub->valb = 0 ;
    *(long *)pgsub->vala = MCS_NONE ;
  } 

  return 0 ;
}


/*+
 *   Function name:
 *   mcPmacSimulate
 *
 *   Purpose:
 *   Simulate a PMAC card when one isn't available
 *
 *   Desmciption:
 *   This routine forms the core of the simulation that is run when a
 *   PMAC card isn't available. It runs a realistic servo so that slewing
 *   and tracking of the Mount is possible.
 *
 *   Invocation:
 *   mcPmacSimulate(pgsub)
 *
 *   Parameters: (">" input, "!" modified, "<" output)  
 *      (!)   pgsub   (struct genSubRecord *)  Pointer to gensub structure
 *
 *   Function value:
 *   (<)  status  (long)  Return status, 0 = OK
 * 
 *   Epics inputs:
 *   a => Servo noise (arcsec)
 *   b => Current maximum velocity (deg/s)
 *   c => Follow demand (counts)
 *   d => countsPerDeg 
 *   f => servoFreq
 *
 *   Epics outputs:
 *   vala => Datum flag 0 = not datumed, 1 = datumed
 *   valb => current position in encoder counts
 *   valc => PMAC demand position in encoder counts 
 *   vald => current velocity in encoder counts 
 *   vale => PMAC demand velocity in encoder counts
 *   valf => PMAC following error
 *   valg => Motor status word 1 for axis
 *   valh => Motor status word 2 for axis
 *   vali => Servo counter
 *
 *-
 */

long mcPmacSimulate (struct genSubRecord *pgsub) 
{

  static int counter = 0 ;                /* Counter for timing datum */
  static int simDatumFlag = 0 ;           /* Flag for whether datumed */
  static int first = TRUE ;               /* Flag to initialise routine */

  static double pa ;                      /* Current position angle */
  static double pav ;                     /* Current velocity */
  static double paerr ;                   /* PA error */
  static int lastStateMCS ;               /* Previous mount state */

  static struct ServoState servoState ;
  static struct ServoConfig servoConfig ;

  double now ;                            /* Current time (TAI) */
  int    inPosition ;                     /* In position flag */
  double pademand ;                       /* Current demand (arcsec) */
  static double paTarget ;                /* Target posn (arcsecs) */
  double countsPerDeg ;                   /* Conversion for input/output */
  double servoFreq ;                      /* Servo frequency */
  static int motorStat1 = 0 ;             /* Motor status word */
  static int motorStat2 = 0 ;             /* Motor status word */

  static long servoCounter ;              /* Servo counter */
  static double time0 ;
  static int  gotTime0 = 0 ;

/* Initialise the servo on the first pass. Note all positions are in arcsec
*  and velocities in arcsec/sec
*/
  if (first) {
    motorStat1 |= SET_OPEN_LOOP ;
    servoConfig.dvmax = 360.0 ;
    servoConfig.gs    = 20.0  ;
    servoConfig.gp    = 5.0 ;
    servoConfig.gi    = 0.5 ;
    servoConfig.pi    = 1.0 ;
    servoConfig.pok   = 0.2 ; 
    servoConfig.tok   = 0.1 ;
    servoState.pdtold = 0.0 ;             /* In simulation always start at 0 */
    timeNow (&(servoState.told) ) ;
    servoState.told -= 0.05 ;
    servoState.tout = servoState.told ;
    servoState.vd   = 0.0 ;
    servoState.accum = 0.0 ;
    pa = servoState.pdtold ;
    pav = 0.0 ;
    paTarget = pa ;
    lastStateMCS = stateMCS ;
    first = FALSE ;
  }

/* Fetch input data */
  servoConfig.noise = *(double *)pgsub->a ;
  servoConfig.vtmax = *(double *)pgsub->b * 3600.0 ;
  pademand          = *(double *)pgsub->c ;
  countsPerDeg      = *(double *)pgsub->d ;
  servoFreq         = *(double *)pgsub->f ;

/* Send demand to servo */
  timeNow (&now) ;

/* Time is often not available for the first few calls to this routine so
*  loop until a sensible value is read 
*/
  if (!gotTime0) {
    if (now > 1000.0) {
      time0 = now ;
      servoState.told = now - 0.05 ;
      servoState.tout = servoState.told ;
      gotTime0 = 1 ;
    }
  } 
    

/* Compute the servo counter */
  servoCounter = (long)((now - time0) * servoFreq)%TWO_POWER_24 ;
  
  if (stateMCS == SIM_MOVING) {

    motorStat2 &= CLEAR_IN_POSITION ;
    motorStat1 &= CLEAR_VELOCITY_ZERO ;
    motorStat1 |= SET_DEFTIME_MOVE ;
    paTarget = paTargetCnts * 3600.0 / countsPerDeg ;
    inPosition = tcsServo (now, paTarget, 0, &servoConfig, &servoState,
         &pa, &pav ) ;
    paerr = paTarget - pa ;
    if (inPosition)
      stateMCS = SIM_STOPPED ;

  } else if (stateMCS == SIM_FOLLOWING) {

    motorStat1 &= CLEAR_DEFTIME_MOVE ;
    motorStat1 &= CLEAR_VELOCITY_ZERO ;
    paTarget = pademand * 3600.0 / countsPerDeg ;
    inPosition = tcsServo (now, paTarget, 0, &servoConfig, &servoState,
         &pa, &pav ) ;
    paerr = paTarget - pa ;
    if (inPosition) {
      motorStat2 |= SET_IN_POSITION ;
    } else {
      motorStat2 &= CLEAR_IN_POSITION ;
    }

  } else {
    if (lastStateMCS != SIM_STOPPED) {
      paTarget = pa ;
    }
    inPosition = tcsServo (now, paTarget, 1, &servoConfig, &servoState,
         &pa, &pav ) ;
    paerr = paTarget - pa ;
    if (inPosition) {
      motorStat1 &= CLEAR_DEFTIME_MOVE ;
      motorStat2 |= SET_IN_POSITION ;
      motorStat1 |= SET_VELOCITY_ZERO ;
    }
  }
    
  lastStateMCS = stateMCS ;

/* Simulate the receipt of a datum command in azimuth, using the Sony switch. 
*  Assuming this record is being scanned at 20Hz, the datum will take 5 secs.
*/
  if (simDatumAzS) {
    counter++  ;  
    simDatumFlag = 0 ;
    motorStat2 &= CLEAR_IN_POSITION ;
    motorStat2 |= AMP_ENABLED ;
    motorStat1 &= CLEAR_OPEN_LOOP ;
    motorStat1 &= CLEAR_VELOCITY_ZERO ;
  }

/* Simulate the receipt of a datum command in azimuth, using the Sony switch. 
*  Assuming this record is being scanned at 20Hz, the datum will take 5 secs.
*/
  if (simEndDatumAzS) {
    if (counter == 100) {
      simDatumAzS = 0 ;
      simDatumFlag = 1 ;
      counter = 0 ;
      motorStat2 = motorStat2 | SET_IN_POSITION | HOME_COMPLETE ;
      motorStat1 |= SET_VELOCITY_ZERO ;
    }
  }

/* Simulate the receipt of a datum command in azimuth, using the Sony switch. 
*  Assuming this record is being scanned at 20Hz, the datum will take 5 secs.
*/
  if (simDatumElS) {
    counter++  ;  
    simDatumFlag = 0 ;
    motorStat2 &= CLEAR_IN_POSITION ;
    motorStat2 |= AMP_ENABLED ;
    motorStat1 &= CLEAR_OPEN_LOOP ;
    motorStat1 &= CLEAR_VELOCITY_ZERO ;
  }

/* Simulate the receipt of a datum command in azimuth, using the Sony switch. 
*  Assuming this record is being scanned at 20Hz, the datum will take 5 secs.
*/
  if (simEndDatumElS) {
    if (counter == 100) {
      simDatumAzS = 0 ;
      simDatumFlag = 1 ;
      counter = 0 ;
      motorStat2 = motorStat2 | SET_IN_POSITION | HOME_COMPLETE ;
      motorStat1 |= SET_VELOCITY_ZERO ;
    }
  }

/* Simulate the receipt of a datum command. Assuming this record is being
*  scanned at 20Hz, the datum will take 5 secs.
*/
  if (simDatumCommand) {
    counter++  ;  
    simDatumFlag = 0 ;
    motorStat2 &= CLEAR_IN_POSITION ;
    motorStat2 |= AMP_ENABLED ;
    motorStat1 &= CLEAR_OPEN_LOOP ;
    motorStat1 &= CLEAR_VELOCITY_ZERO ;
    if (counter == 100) {
      simDatumCommand = 0 ;
      simDatumFlag = 1 ;
      counter = 0 ;
      motorStat2 = motorStat2 | SET_IN_POSITION | HOME_COMPLETE ;
      motorStat1 |= SET_VELOCITY_ZERO ;
    }
  }

  motorStat1 |= ACTIVATE_MOTOR ;
  motorStat2 |= ASSIGN_CS ;

  if (pa >= PMAC_POS_LIMIT)
    motorStat1 |= SET_POS_LIMIT ;
  else
    motorStat1 &= CLEAR_POS_LIMIT ;

  if (pa <= PMAC_NEG_LIMIT)
    motorStat1 |= SET_NEG_LIMIT ;
  else
    motorStat1 &= CLEAR_NEG_LIMIT ;

/* Output data for EPICS */
  *(long *)pgsub->vala = simDatumFlag ;
  *(double *)pgsub->valb = pa * countsPerDeg * ECT * DEF_I108 / 3600.0 ;
  *(double *)pgsub->valc = paTarget * countsPerDeg * ECT * DEF_I108 / 3600.0 ;
  *(double *)pgsub->vald = pav * ECT * DEF_I109 *VEL_CNTS_PER_DEG /
                           (servoFreq * 3600.0) ;
  *(double *)pgsub->vale = servoState.vd * ECT * DEF_I109 *VEL_CNTS_PER_DEG /
                           (servoFreq * 3600.0) ;
  *(double *)pgsub->valf = paerr * countsPerDeg * ECT * DEF_I108 / 3600.0 ;
  *(long *)pgsub->valg = (long)motorStat1 ;
  *(long *)pgsub->valh = (long)motorStat2 ;
  *(double *)pgsub->vali = (double)servoCounter ;
  *(double *)pgsub->valj = time0 ;

  return 0 ;

}

/*+
 *   Function name:
 *   mcSetSimuLevel
 *
 *   Purpose:
 *   Set the simulation level of the Mount control system
 *
 *   Desmciption:
 *   Store the specified simulation level for future use. Note that at
 *   present there are no grades of simulation. All levels greater than 0
 *   are treated the same. If a negative number is entered then this is
 *   converted to a positive one. To specify no simulation the level should
 *   be set to 0.
 *
 *   Invocation:
 *   mcSetSimuLevel(simLevel)
 *
 *   Parameters: (">" input, "!" modified, "<" output)  
 *   (>)  simLevel  (int)  Desired simulation level
 *
 *   Function value:
 *   None
 * 
 *-
 */

void mcSetSimuLevel (int simLevel)
{

  if (simLevel < 0) simLevel = -simLevel ;
  intSimulate = simLevel ;

}

/*+
 *   Function name:
 *   mcSimDatum
 *
 *   Purpose:
 *   Trigger a simulated datum command
 *
 *   Desmciption:
 *   This routine simply sets a flag to trigger the start of a simulated
 *   datum command
 *
 *   Invocation:
 *   mcSimDatum(card)
 *
 *   Parameters: (">" input, "!" modified, "<" output)  
 *   None
 *
 *   Function value:
 *   None
 * 
 *-
 */

void mcSimStartDatumS (long card)
{
    if (card == 0)
        simDatumAzS = 1;
    else
        simDatumElS =1;
}

void mcSimEndDatumS (long card)
{
    if (card == 0)
        simEndDatumAzS = 1;
    else
        simEndDatumElS =1;
}

/*+
 *   Function name:
 *   mcSimFollow
 *
 *   Purpose:
 *   Trigger follow mode in the PMAC simulator
 *
 *   Desmciption:
 *   This routine simply sets the simulator state to following
 *
 *   Invocation:
 *   mcSimFollow()
 *
 *   Parameters: (">" input, "!" modified, "<" output)  
 *   None
 *
 *   Function value:
 *   None
 * 
 *-
 */

void mcSimFollow (void)
{

  stateMCS = SIM_FOLLOWING ; 

}
/*+
 *   Function name:
 *   mcSimMove
 *
 *   Purpose:
 *   Trigger a simulated move command
 *
 *   Desmciption:
 *   This routine triggers a move in the PMAC simulator. It first checks
 *   the input string to make sure it is a valid PMAC command and then
 *   extracts the demanded position. The position is converted from counts
 *   to arcsec and then passed to the simulator
 *
 *   Invocation:
 *   mcSimMove(command, errmsg)
 *
 *   Parameters: (">" input, "!" modified, "<" output)  
 *   (>)     command   (char *)   Command string
 *   (<)     errmsg    (char *)   Error message
 *
 *   Function value:
 *   None
 * 
 *-
 */

void mcSimMove (char *command, char *errmsg)
{

/* Initialise error message */
  errmsg = NULL ;

  errlogPrintf ("command string is %s\n", command) ;

/* Decode the command string */
  if (strlen(command) < 4) {
    errlogPrintf ("Nothing to do\n") ;
    return ;                              /* Nothing to do */
  } else if ( command[3] == '-') {
    errlogPrintf ("Driving to negative limit\n") ;
    paTargetCnts = -270.0 * MCDEG_TO_CNTS ;       /* Drive to minimum limit */
    stateMCS = SIM_MOVING ; 
  } else if ( command[3] == '+') {
    errlogPrintf ("Driving to positive limit\n") ;
    paTargetCnts = 270.0 * MCDEG_TO_CNTS ;        /* Drive to maximum limit */
    stateMCS = SIM_MOVING ; 
  } else {

/* Try to read a demand position from the command string */
    paTargetCnts = atof (&command[4]) ;
    errlogPrintf ("paTargetCnts = %f\n", paTargetCnts ) ;

    stateMCS = SIM_MOVING ; 

  }
}

/*+
 *   Function name:
 *   mcSimStop
 *
 *   Purpose:
 *   Trigger a simulated stop command
 *
 *   Desmciption:
 *   This routine triggers a stop in the PMAC simulator. It  does this by
 *   simply setting the simulator state to stopped.
 *
 *   Invocation:
 *   mcSimStop()
 *
 *   Parameters: (">" input, "!" modified, "<" output)  
 *   None
 *
 *   Function value:
 *   None
 * 
 *-
 */

void mcSimStop (void)
{

  stateMCS = SIM_STOPPED ; 

}


/*+
 *   Function name:
 *   mcSimulationMode
 *
 *   Purpose:
 *   Update the simulation level of the Mount software
 *
 *   Desmciption:
 *   This routine is used in conjunction with the mcInitSimMode routine
 *   to update the simulation level of the Mount software. Note it
 *   only adjusts the simulation level not the simulation mode. The
 *   simulation mode is set at boot time and can't be changed without
 *   stopping and restarting the software.
 *
 *   Invocation:
 *   mcSimulationMode(pgsub)
 *
 *   Parameters: (">" input, "!" modified, "<" output)  
 *      (!)    pgsub    (struct genSubRecord *)  Pointer to genSub structure
 *
 *
 *   Epics inputs:
 *   a => demand simulation level
 *
 *   Epics outputs:
 *   vala => output simulation level
 *   valb => current simulation mode
 *
 *   Function value:
 *   (<)  status  (long)  Return status, 0 = OK
 * 
 *-
 */

long mcSimulationMode (struct genSubRecord *pgsub) 
{

  long demandSimLevel ;

  demandSimLevel = *(long *)pgsub->a ;

  if (mcGetSimuLevel()) {

    if (demandSimLevel < MCS_FULL || demandSimLevel > MCS_FAST)
      demandSimLevel = MCS_FULL ;
    *(long *)pgsub->vala = demandSimLevel ;

    /* AWE: put a YES or NO on VALC to be used to point record SIML links to.*/
    strcpy( (char *)pgsub->valc, "YES");

  } else {

    *(long *)pgsub->vala = MCS_NONE ;
    strcpy( (char *)pgsub->valc, "NO");
  } 

  return 0 ;
}

epicsRegisterFunction(mcPmacSimulate);
epicsRegisterFunction(mcSetSimuLevel);
epicsRegisterFunction(mcSimulationMode);
epicsRegisterFunction(mcInitSimMode);

static const iocshArg mcSetSimuLevelArg0 = {"Simulation level", iocshArgInt };
static const iocshArg *mcSetSimuLevelArgs[] = { &mcSetSimuLevelArg0 };
static const iocshFuncDef mcSetSimuLevelFuncDef = 
         {"mcSetSimuLevel", 1, mcSetSimuLevelArgs};
static void mcSetSimuLevelCallFunc(const iocshArgBuf *args)
{
   mcSetSimuLevel(args[0].ival);
}

static void mcRegisterMiscCommands(void)
{
   static int firstTime = 1;
   if(firstTime) {
      iocshRegister(&mcSetSimuLevelFuncDef, mcSetSimuLevelCallFunc);
   }
}
epicsExportRegistrar(mcRegisterMiscCommands);


