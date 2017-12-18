/* States of 20Hz data */

#define MCS_VALID   0
#define MCS_INVALID 1
#define MCS_TIMEOUT 2

/* PMAC Access */

#define MCS_GOOD 0
#define MCS_BAD  1

/* Datum Flag */

#define MCS_NOT_DATUMED 0
#define MCS_DATUMED     1

/* Axis States */

#define MCS_BRAKED          0
#define MCS_STATIONARY      1
#define MCS_SLEWING         2
#define MCS_TRACKING        3
#define MCS_APPLY_BRAKES    4
#define MCS_RELEASE_BRAKES  5
#define MCS_INTERLOCK       6

/* Trajectory calculation options */

#define NO_ADJUST      0         /* No trajectory calculation */
#define ADJUST_VEL     1         /* Velocity limitation */
#define ADJUST_ACCEL   2         /* Acceleration limit */ 


/* Virtual Encoder algorithms for the axes */

#define MCS_FDE       0
#define MCS_ALL_HEADS 1
#define MCS_N         2
#define MCS_M_N       3
#define MCS_EL_N      4

/* Virtual Encoder heads and Datum parameters */

#define MCS_NONE         0
#define MCS_HEAD_1       1
#define MCS_HEAD_2       2
#define MCS_HEAD_3       3
#define MCS_HEAD_4       4
#define MCS_HEADS_1_3    5
#define MCS_HEADS_2_4    6
#define MCS_HEADS_1_2    7
#define MCS_HEAD_1_T     8
#define MCS_HEAD_2_T     9
#define MCS_ALL_AZHEADS 10
#define MCS_ALL_ELHEADS 11
#define MCS_SWITCH      12
#define MCS_HEADS_3_4   13

/* Virtual Encoder setup words */

#define MCS_FDE_SETUP        0x000
#define MCS_HEAD1_SETUP      0x208
#define MCS_HEAD2_SETUP      0x408
#define MCS_HEAD3_SETUP      0x608
#define MCS_HEAD4_SETUP      0x808
#define MCS_HEADS1_2_SETUP   0x450
#define MCS_HEADS1_3_SETUP   0x650
#define MCS_HEADS2_4_SETUP   0x890
#define MCS_ALL_HEADS_SETUP  0x018
#define MCS_HEAD1T_SETUP     0x220
#define MCS_HEAD2T_SETUP     0x420
#define MCS_HEADS3_4_SETUP   0x8D0

/* Debug States */

/* MCS_NONE defined earlier */
#define MCS_FULL        1

/* Elevation Striker Bracket Positions */

#define MCS_OBSERVING   0
#define MCS_ENGINEERING 1

/* Elevation Tilt Switch Positions */

#define MCS_BELOW_45_DEGREES 0
#define MCS_ABOVE_45_DEGREES 1

/* Follow States */

#define MCS_IGNORING  0
#define MCS_FOLLOWING 1

/* Slew State */
#define MCS_SLEW 1

/* Health States */

/* MCS_GOOD and MCS_BAD already defined */
#define MCS_WARNING 2

/* In-Position States */

#define MCS_FALSE 0
#define MCS_TRUE  1

/* Logging States - logData SIR record */

#define MCS_LOG_OFF      0
#define MCS_LOG_PRIMED   1
#define MCS_LOG_ON       2

/* Simulation States */

/* MCS_NONE defined earlier */
/* MCS_FULL defined earlier */
#define MCS_VSM  2
#define MCS_FAST 3

/* Operational States of the MCS */

#define MCS_BOOTING      0
#define MCS_INITIALISING 1
#define MCS_RUNNING      2

/* Azimuth Cable Wraps */

#define MCS_WRAP_A 0
#define MCS_WRAP_B 1
#define MCS_WRAP_C 2

/* Look-up Tables */

#define MCS_REFERENCE 0
#define MCS_TAPE      1

/* PMAC Servo Interrupt Time units */

#define PMAC_INTERRUPT_FACTOR  8388608

/* MCS Commands */

#define MCS_NOCOMMAND    0
#define MCS_REBOOT       1
#define MCS_INIT         2
#define MCS_DATUM        3
#define MCS_TEST         4
#define MCS_DEBUG        5
#define MCS_SIMULATE     6
#define MCS_STOP         7
#define MCS_MOVE         8
#define MCS_FOLLOW       9
#define MCS_PARK        10
#define MCS_AZCONFIG    11
#define MCS_ELCONFIG    12
#define MCS_LOGDATA     13
#define MCS_TOLERANCE   14
#define MCS_CWMOVE      15
#define MCS_TESTLAN     16
#define MCS_RELOAD      17

/* Files which can be reloaded */

#define MCS_TELESCOPE_PAR	0
#define MCS_TAPEPOLYS_PAR	1
#define MCS_AXISLIMITS_PAR	2
#define MCS_SONY_PAR		3
#define MCS_LOGDATA_PAR		4
#define MCS_ALLFILES_PAR	5
#define MCS_CW1_PAR		6
#define MCS_CW2_PAR		7
#define MCS_CW3_PAR		8

/* For the pvload calls from the MCS RELOAD and INIT commands */
#define DIRPATH_LEN	64
#define MCS_NUM_FILES	 6
