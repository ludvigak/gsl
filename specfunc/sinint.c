#include <math.h>
#include <gsl_math.h>
#include <gsl_errno.h>
#include "gsl_sf_chebyshev.h"
#include "gsl_sf_expint.h"


/*-*-*-*-*-*-*-*-*-*-*-* Private Section *-*-*-*-*-*-*-*-*-*-*-*/

/* based on SLATEC r9sifg.f, W. Fullerton */

/*
 series for f1   on the interval  2.00000e-02 to  6.25000e-02
					with weighted error   2.82e-17
					 log weighted error  16.55
			       significant figures required  15.36
				    decimal places required  17.20
*/
static double f1_data[20] = {
   -0.1191081969051363610,
   -0.0247823144996236248,
    0.0011910281453357821,
   -0.0000927027714388562,
    0.0000093373141568271,
   -0.0000011058287820557,
    0.0000001464772071460,
   -0.0000000210694496288,
    0.0000000032293492367,
   -0.0000000005206529618,
    0.0000000000874878885,
   -0.0000000000152176187,
    0.0000000000027257192,
   -0.0000000000005007053,
    0.0000000000000940241,
   -0.0000000000000180014,
    0.0000000000000035063,
   -0.0000000000000006935,
    0.0000000000000001391,
   -0.0000000000000000282
};
static struct gsl_sf_ChebSeries f1_cs = {
  f1_data,
  19,
  -1, 1,
  (double *)0,
  (double *)0
};

/*

 series for f2   on the interval  0.00000e+00 to  2.00000e-02
					with weighted error   4.32e-17
					 log weighted error  16.36
			       significant figures required  14.75
				    decimal places required  17.10
*/
static double f2_data[29] = {
   -0.0348409253897013234,
   -0.0166842205677959686,
    0.0006752901241237738,
   -0.0000535066622544701,
    0.0000062693421779007,
   -0.0000009526638801991,
    0.0000001745629224251,
   -0.0000000368795403065,
    0.0000000087202677705,
   -0.0000000022601970392,
    0.0000000006324624977,
   -0.0000000001888911889,
    0.0000000000596774674,
   -0.0000000000198044313,
    0.0000000000068641396,
   -0.0000000000024731020,
    0.0000000000009226360,
   -0.0000000000003552364,
    0.0000000000001407606,
   -0.0000000000000572623,
    0.0000000000000238654,
   -0.0000000000000101714,
    0.0000000000000044259,
   -0.0000000000000019634,
    0.0000000000000008868,
   -0.0000000000000004074,
    0.0000000000000001901,
   -0.0000000000000000900,
    0.0000000000000000432
};
static struct gsl_sf_ChebSeries f2_cs = {
  f2_data,
  28,
  -1, 1,
  (double *)0,
  (double *)0
};

/*

 series for g1   on the interval  2.00000e-02 to  6.25000e-02
					with weighted error   5.48e-17
					 log weighted error  16.26
			       significant figures required  15.47
				    decimal places required  16.92
*/
static double g1_data[21] = {
   -0.3040578798253495954,
   -0.0566890984597120588,
    0.0039046158173275644,
   -0.0003746075959202261,
    0.0000435431556559844,
   -0.0000057417294453025,
    0.0000008282552104503,
   -0.0000001278245892595,
    0.0000000207978352949,
   -0.0000000035313205922,
    0.0000000006210824236,
   -0.0000000001125215474,
    0.0000000000209088918,
   -0.0000000000039715832,
    0.0000000000007690431,
   -0.0000000000001514697,
    0.0000000000000302892,
   -0.0000000000000061400,
    0.0000000000000012601,
   -0.0000000000000002615,
    0.0000000000000000548
};
static struct gsl_sf_ChebSeries g1_cs = {
  g1_data,
  20,
  -1, 1,
  (double *)0,
  (double *)0
};

/*

 series for g2   on the interval  0.00000e+00 to  2.00000e-02
					with weighted error   5.01e-17
					 log weighted error  16.30
			       significant figures required  15.12
				    decimal places required  17.07
*/
static double g2_data[34] = {
   -0.0967329367532432218,
   -0.0452077907957459871,
    0.0028190005352706523,
   -0.0002899167740759160,
    0.0000407444664601121,
   -0.0000071056382192354,
    0.0000014534723163019,
   -0.0000003364116512503,
    0.0000000859774367886,
   -0.0000000238437656302,
    0.0000000070831906340,
   -0.0000000022318068154,
    0.0000000007401087359,
   -0.0000000002567171162,
    0.0000000000926707021,
   -0.0000000000346693311,
    0.0000000000133950573,
   -0.0000000000053290754,
    0.0000000000021775312,
   -0.0000000000009118621,
    0.0000000000003905864,
   -0.0000000000001708459,
    0.0000000000000762015,
   -0.0000000000000346151,
    0.0000000000000159996,
   -0.0000000000000075213,
    0.0000000000000035970,
   -0.0000000000000017530,
    0.0000000000000008738,
   -0.0000000000000004487,
    0.0000000000000002397,
   -0.0000000000000001347,
    0.0000000000000000801,
   -0.0000000000000000501
};
static struct gsl_sf_ChebSeries g2_cs = {
  g2_data,
  33,
  -1, 1,
  (double *)0,
  (double *)0
};


/* x >= 4.0 */
static void fg_asymp(const double x, double * f, double * g)
{
  /*
      xbig = sqrt (1.0/r1mach(3))
      xmaxf = exp (amin1(-alog(r1mach(1)), alog(r1mach(2))) - 0.01)
      xmaxg = 1.0/sqrt(r1mach(1))
      xbnd = sqrt(50.0)
  */
  const double xbig  = 1./GSL_SQRT_MACH_EPS;
  const double xmaxf = 1./DBL_MIN;
  const double xmaxg = 1./GSL_SQRT_DBL_MIN;
  const double xbnd  = 7.07106781187;

  const double x2 = x*x;

  if(x <= xbnd) {
    *f = (1.0 + gsl_sf_cheb_eval((1.0/x2-0.04125)/0.02125, &f1_cs))/x;
    *g = (1.0 + gsl_sf_cheb_eval((1.0/x2-0.04125)/0.02125, &g1_cs))/x2;
  }
  else if(x <= xbig) {
    *f = (1.0 + gsl_sf_cheb_eval(100./x2-1., &f2_cs))/x;
    *g = (1.0 + gsl_sf_cheb_eval(100./x2-1., &g2_cs))/x2;
  }
  else {
    *f = (x < xmaxf ? 1./x  : 0.);
    *g = (x < xmaxg ? 1./x2 : 0. );
  }
  return;
}


/* based on SLATEC si.f, W. Fullerton

 series for si   on the interval  0.00000e+00 to  1.60000e+01
					with weighted error   1.22e-17
					 log weighted error  16.91
			       significant figures required  16.37
				    decimal places required  17.45
*/

static double si_data[12] = {
  -0.1315646598184841929,
  -0.2776578526973601892,
   0.0354414054866659180,
  -0.0025631631447933978,
   0.0001162365390497009,
  -0.0000035904327241606,
   0.0000000802342123706,
  -0.0000000013562997693,
   0.0000000000179440722,
  -0.0000000000001908387,
   0.0000000000000016670,
  -0.0000000000000000122
};

static struct gsl_sf_ChebSeries si_cs = {
  si_data,
  11,
  -1, 1,
  (double *)0,
  (double *)0
};


/*-*-*-*-*-*-*-*-*-*-*-* (semi)Private Implementations *-*-*-*-*-*-*-*-*-*-*-*/

/* checked OK [GJ] */
int gsl_sf_Si_impl(const double x, double * result)
{
  double ax   = fabs(x);
  
  if(ax < GSL_SQRT_MACH_EPS) {
    *result = x;
    return GSL_SUCCESS;
  }
  else if(ax <= 4.0) {
    *result = x * (0.75 + gsl_sf_cheb_eval((x*x-8.0)*0.125, &si_cs));
    return GSL_SUCCESS;
  }
  else {
    double f, g;
    fg_asymp(ax, &f, &g);
    *result = 0.5 * M_PI - f*cos(ax) - g*sin(ax);
    if(x < 0.) *result = - *result;
    return GSL_SUCCESS;
  }
}


/*
 series for ci   on the interval  0.00000e+00 to  1.60000e+01
					with weighted error   1.94e-18
					 log weighted error  17.71
			       significant figures required  17.74
				    decimal places required  18.27
*/
static double ci_data[13] = {
   -0.34004281856055363156,
   -1.03302166401177456807,
    0.19388222659917082877,
   -0.01918260436019865894,
    0.00110789252584784967,
   -0.00004157234558247209,
    0.00000109278524300229,
   -0.00000002123285954183,
    0.00000000031733482164,
   -0.00000000000376141548,
    0.00000000000003622653,
   -0.00000000000000028912,
    0.00000000000000000194
};
static struct gsl_sf_ChebSeries ci_cs = {
  ci_data,
  12,
  -1, 1,
  (double *)0,
  (double *)0
};

/* checked OK [GJ] */
int gsl_sf_Ci_impl(const double x, double * result)
{
  const double xsml = GSL_SQRT_DBL_MIN;
  
  if(x <= 0.0) {
    return GSL_EDOM;
  }
  else if(x <= 4.) {
    double y = -1.0;
    if (x > xsml) y = (x*x-8.0)*0.125;
    *result = log(x) - 0.5 + gsl_sf_cheb_eval(y, &ci_cs);
    return GSL_SUCCESS;
  }
  else {
    double f, g;
    fg_asymp(x, &f, &g);
    *result = f*sin(x) - g*cos(x);
    return GSL_SUCCESS;
  }
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions w/ Error Handling *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_Si_e(const double x, double * result)
{
  int status = gsl_sf_Si_impl(x, result);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_sf_Si_e", status);
  }
  return status;
}

int gsl_sf_Ci_e(const double x, double * result)
{
  int status = gsl_sf_Ci_impl(x, result);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_sf_Ci_e", status);
  }
  return status;
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*-*/

double gsl_sf_Si(double x)
{
  double y;
  int status = gsl_sf_Si_impl(x, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("gsl_sf_Si_e", status);
  }
  return y;
}

double gsl_sf_Ci(double x)
{
  double y;
  int status = gsl_sf_Ci_impl(x, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("gsl_sf_Ci_e", status);
  }
  return y;
}
