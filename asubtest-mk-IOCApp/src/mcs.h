#define SYNTAX_ERROR  1
#define ALL_BLANKS    2
#define TOO_LONG      3

#define MCS_GOOD    0
#define MCS_BAD     1
#define MCS_WARNING 2

/* Simulation States */

#define MCS_NONE        0
#define MCS_FULL        1
#define MCS_VSM         2
#define MCS_FAST        3

/* Trajectory calculation options */
#define NO_ADJ    0         /* No trajectory calculation */
#define VELOCITY  1         /* Velocity limitation */
#define FULL      2         /* Acceleration limit */ 

long check_string( char *, char *, int, int );
long check_numeric( char * );

/* These are limits on the position and velocity tolerances */

#define POS_ERROR_LOWER 0.0          /* Degrees */
#define POS_ERROR_UPPER 2.0          /* Degrees */
#define VEL_ERROR_LOWER 0.0          /* Degrees/s */
#define VEL_ERROR_UPPER 2.0          /* Degrees/s */

#define AZ_MAX_VEL 1.9
#define EL_MAX_VEL 0.7

#define AZ_MIN_VEL -1.9
#define EL_MIN_VEL -0.7

#define AZ_MAX_ACC 0.1
#define EL_MAX_ACC 0.04

#define AZ_MIN_ACC -0.1
#define EL_MIN_ACC -0.04

#define AZ_MIN_POS -188.25
#define AZ_MAX_POS  367.67
#define EL_MIN_POS  0.0
#define EL_MAX_POS  92.73

#ifdef TELESCOPE

/* For the Telescope */

#define ECT            32           /* Encoder Conversion Table Counts Factor      */
#define RAW_PER_VE     1.0          /* Raw Counts per Virtual Encoder Count        */
#define AZDEG_TO_I222  45.0/2.0     /* Az. Factor between deg/s and I222 cnts/msec */
#define ELDEG_TO_I222  16.875/0.75  /* El. Factor between deg/s and I222 cnts/msec */
#define AZ_I223_S      30.0         /* Az. home velocity using switch              */
#define EL_I223_S      10.0         /* El. home velocity using switch              */
#define AZ_I223_H       1.0         /* Az. home velocity using tape head           */
#define EL_I223_H       1.0         /* El. home velocity using tape head           */
#define MCDEG_TO_CNTS   533572.2667 /* Degs to encoder counts */
#define MCDEG_TO_I122   MCDEG_TO_CNTS/1000.0  /* Cr. Factor between deg/s and I122 cnts/msec */
#define VEL_CNTS_PER_DEG 29965.06616

/* Default values when using simulation mode */
#define DEF_I10          3713707
#define DEF_I108         96
#define DEF_I109         96
#define DEF_I508         80
#define DEF_I509         80

#else

/* For the stage */

#define ECT            32        /* Encoder Conversion Table Counts Factor   */
#define RAW_PER_VE     0.75/32.0 /* Raw Counts per Virtual Encoder Count/ECT */
#define AZDEG_TO_I222  0.5208    /* Factor between deg/s and I222 cnts/msec  */
#define ELDEG_TO_I222  0.5208    /* Factor between deg/s and I222 cnts/msec  */
#define AZ_I223_S      2.5       /* Az. home velocity using switch           */
#define EL_I223_S      2.5       /* El. home velocity using switch           */
#define AZ_I223_H      1.0       /* Az. home velocity using tape head        */
#define EL_I223_H      1.0       /* El. home velocity using tape head        */

#endif

/* Function Prototypes */

long check_string( char *, char *, int, int );
long check_numeric( char * );
int  mcGetSimuLevel (void) ;
void mcSimStartDatumS (long) ;
void mcSimEndDatumS (long) ;
void mcSimFollow (void) ;
void mcSimStop (void) ;

