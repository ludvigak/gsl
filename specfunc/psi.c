/*
 * Author: G. Jungman
 * RCS: "$Id$"
 */
#include <math.h>
#include <gsl_math.h>
#include "gsl_sf_chebyshev.h"
#include "gsl_sf_psi.h"


/* simple implementation for integer argument */
double gsl_sf_psi_i(int n)
{
  double result;
  
  if(n == 1) {
    result = -M_EULER;
  }
  else if(n <= 0) {
    /* domain error */
    result = 0.;
  }
  else {
    int k;
    result = -M_EULER;
    for(k=1; k<n; k++) {
      result += 1./k;
    }
  }
  
  return result;
}


/* Chebyshev fits from SLATEC code for psi(x)

 Series for PSI        on the interval  0.	   to  1.00000D+00
				       with weighted error   2.03E-17
					log weighted error  16.69
			      significant figures required  16.39
				   decimal places required  17.37

 Series for APSI       on the interval  0.	   to  2.50000D-01
				       with weighted error   5.54E-17
					log weighted error  16.26
			      significant figures required  14.42
				   decimal places required  16.86

*/

static double psics_data[23] = {
  -.038057080835217922,
   .491415393029387130, 
  -.056815747821244730,
   .008357821225914313,
  -.001333232857994342,
   .000220313287069308,
  -.000037040238178456,
   .000006283793654854,
  -.000001071263908506,
   .000000183128394654,
  -.000000031353509361,
   .000000005372808776,
  -.000000000921168141,
   .000000000157981265,
  -.000000000027098646,
   .000000000004648722,
  -.000000000000797527,
   .000000000000136827,
  -.000000000000023475,
   .000000000000004027,
  -.000000000000000691,
   .000000000000000118,
  -.000000000000000020
}    
static double apsics_data[16] = {    
  -.0204749044678185,
  -.0101801271534859,
   .0000559718725387,
  -.0000012917176570,
   .0000000572858606,
  -.0000000038213539,
   .0000000003397434,
  -.0000000000374838,
   .0000000000048990,
  -.0000000000007344,
   .0000000000001233,
  -.0000000000000228,
   .0000000000000045,
  -.0000000000000009,
   .0000000000000002,
  -.0000000000000000 
}    
static struct gsl_sf_ChebSeries psi_cs = {
  psics_data,
  22,
  -1, 1
}
static struct gsl_sf_ChebSeries apsi_cs = {
  apsics_data,
  15,
  -1, 1
}


double gsl_sf_psi(double x)
{
  double result;
  double y = fabs(x);
  double xbig  = 1./GSL_SQRT_MACH_EPS;       /* XBIG  = 1.0/SQRT(R1MACH(3)) */
  double dxrel = 10. * GSL_SQRT_MACH_EPS;    /* DXREL = SQRT (R1MACH(4))    */
  
  if(y >= 2.0) {
    double aux = 0.;
    if(y < xbig) aux = gsl_sf_cheb_eval(8./(y*y)-1., &apsi_cs);
    if(x < 0.) {
      result = log(y) - 0.5/x + aux - M_PI * cot(M_PI*x);
    }
    else { /* x > 0 */
      result = log(x) - 0.5/x + aux;
    }
  }
  else { /* y < 2.0 */
    if(x == 0.) {
      /* x == 0 */
      result = 0.;
    }
    else {
      int n = x;
      if(x < 0.) --n;
      y = x - n;
      --n;
      result = gsl_sf_cheb_eval(2.*y-1., psi_cs);
      if(n == 0) return result;

      n = -n;

      if(x < 0. && x+n-2 == 0.) {
      	/* x is a negative integer */
	result = 0.;
      }
      else {
	if(x < -0.5 && fabs((x-AINT(x-0.5))/x) < dxrel) {
      	/* loss of precision: x near a negative integer */
      	}

      	for(i=0; i<n; i++) {
      	  result -= 1./(x + i);
      	}
      }
    }
  }    
  
  return result;
}
