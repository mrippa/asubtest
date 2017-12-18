/*
*   FILENAME
*   tcsServo.h
*
*   DESCRIPTION
*   Header file for defining structures and functions used by the tcs
*   servo simulator
*
*/
/* *INDENT-OFF* */
/* *INDENT-ON* */

#ifndef SERVO_H
#define SERVO_H

struct ServoConfig
{
   double dvmax;     /* Acceleration performance (arcsec/sec/sec) */
   double vtmax;     /* Speed performance (arcsec/sec)            */
   double gs;        /* Gain: SQRT                                */
   double gp;        /* Gain: proportional                        */
   double gi;        /* Gain: integrator                          */
   double pi;        /* Anti-windup (integrator window)           */
   double pok;       /* "Near enough" how close                   */
   double tok;       /* "Near enough" how long                    */
   double noise;     /* Noise                                     */
};

struct ServoState
{
   double told;             /* Previous timestamp */
   double pdtold;           /* Previous demand position */
   double vd;               /* Velocity demand */
   double tout;             /* Time when last out of position */
   double accum;            /* Integrator */
};

int tcsServo ( double t, double pdt, int stop, struct ServoConfig*, 
               struct ServoState*, double *pt, double *vt );
#endif

