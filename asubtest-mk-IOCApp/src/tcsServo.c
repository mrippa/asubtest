/*
*   FILENAME
*   tcsServo.c
*
*   FUNCTION NAME(S)
*   tcsServo - Simulates a servo mechanism
*
*/
/* *INDENT-OFF* */
/* *INDENT-ON* */

#include <math.h>
#include <float.h>
#include "tcsServo.h"
#include "tcsRandom.h"

int tcsServo ( double t, double pdt, int stop, struct ServoConfig *config, 
               struct ServoState *state, double *pt, double *vt )
/*
**
**  - - - - - -
**   S E R V O
**  - - - - - -
**
**  Simulate one telescope servo (az, el, pa).
**
**
**  Given arguments:
**     t        d       Timestamp (sec)
**     pdt      d       Demand position at time t (arcsec)
**     stop     i       TRUE to stop mechanism
**     config   struct* Servo parameters
**     state    struct* Servo state
**
**  Modified arguments:
**     pt       d*     Position achieved at time t (arcsec)
**     vt       d*     Velocity achieved at time t (arcsec/sec)
**
**  Function value:
**              i      non-zero = "in position"
*/
#define TINY 1e-10
#define HUGE 1e10
{
/* Scratch variables */
   double dt;                       /* Time since last update */
   double ve;                       /* Velocity error */
   double dv;                       /* Velocity change */
   double dvm;                      /* Maximum velocity change in the time */
   double pe;                       /* Position error */
   double s0;                       /* SQRT/PI breakpoint */
   double vi;                       /* Integrator velocity increment */
   double w;                        /* Workspace */
   int ok;                          /* In position */


   if (fabs(pdt) < FLT_MIN) 
     pdt = (pdt < 0.0) ? -1.0 * FLT_MIN : FLT_MIN;

/* Time since last update. */
   dt = t - state->told;
   if ( dt == 0.0 ) dt = 1e-3;

/* Predict telescope position and velocity. */
   ve = state->vd - *vt;
   dv = fabs ( ve );
   dvm = config->dvmax * dt;
   dv = ( dv <= dvm ) ? dv : dvm;
   if ( ve < 0.0 ) dv = -dv;
   *vt += dv;
   if ( fabs ( *vt ) >= config->vtmax ) 
      *vt = ( *vt >= 0.0 ) ? config->vtmax : -config->vtmax;
   *pt += *vt * dt;

/* Add noise */
   if (config->noise != 0.0) *pt += tcsGresid(config->noise);

/* Position error. */
   pe = *pt - pdt;

/* "In position" test. */
   if ( stop ) 
   {
      if ( *vt == 0.0 ) 
      {
         state->tout = t;
         ok = 1;
      }
      else
      {
         ok = 0;
      }
   }
   else
   {
      w = state->tout;
      if ( fabs ( pe ) > config->pok ) 
      {
         state->tout = t;
         ok = 0;
      }
      else ok = ( ( t - w ) >= config->tok ) ? 1 : 0;
   }

/* Velocity feedforward = measured tracking velocity. */
   state->vd = ( pdt - state->pdtold ) / dt;

/* Square root or linear term. */
   if ( fabs ( config->gs * config->gp ) < TINY ) {
      s0 = HUGE;
   } else {
      s0 = ( config->gs * config->gs ) / ( config->gp * config->gp );
   }
   if ( fabs ( pe ) > s0 ) {
      w = config->gs * sqrt ( fabs ( pe ) );
      state->vd -= ( pe > 0.0 ) ? w : -w;
   } else {
      state->vd -= config->gp * pe;
   }

/* Integrator term if within window. */
   if ( fabs ( pe ) > config->pi ) {
      state->accum = 0.0;
   } else {
      state->accum += pe;
      vi = config->gi * state->accum;
      if ( fabs ( vi ) > config->vtmax ) 
         vi = ( vi > 0.0 ) ? config->vtmax : -config->vtmax;
      if ( fabs ( config->gi ) > TINY ) state->accum = vi / config->gi;
      state->vd -= vi;
   }

/* Limit total demand. */
   if ( fabs ( state->vd ) > config->vtmax ) 
      state->vd = ( state->vd > 0.0 ) ? config->vtmax : -config->vtmax;
   if ( stop ) state->vd = 0.0;

/* Note latest timestamp and position demand. */
   state->told = t;
   state->pdtold = pdt;

/* Exit. */
   return ok;
}
