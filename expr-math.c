
#include "expr-math.h"
#include <math.h>

double
quadratic(double a, double b, double c)
{
  double d = b * b - 4 * a * c;
  return (a == 0)
    ? ( (b == 0 && c == 0) ? NAN : -(c / b) )
    : ( (d < 0) ? NAN : (-b + sqrt(d)) / (2 * a) );
}

double
nquadratic(double a, double b, double c)
{
  double d = b * b - 4 * a * c;
  return (a == 0)
    ? ( (b == 0 && c == 0) ? NAN : -(c / b) )
    : ( (d < 0) ? NAN : (-b - sqrt(d)) / (2 * a) );
}

double
trianglewave(double x)
{
  double frac, tmp;
  frac = modf(x, &tmp) * 2.0;
  return frac <= 1.0 ? frac : 2.0 - frac;
}

int
approx(double a, double b)
{
#define CUTOFF .9999999 /* 15-17 significant digits; stop halfway */
  double da = fabs(a);
  double db = fabs(b);
  int samesigns = (da == a) == (db == b); /* both pos or neg */
  if (da > db) { double tmp = da; da = db; db = tmp; }
  return samesigns && da >= (db * CUTOFF);
}

#ifdef TEST
#include <stdio.h>

void
test_approx(void)
{
  struct {
    int rv;
    double a;
    double b;
  } tests[] = {
    { 0, 1, 1.1 },
    { 1, 1, 1.00000005 },
    { 0, -1, 1.00000005 },
    { 0, 1, -1.00000005 },
    { 1, -1, -1.00000005 },
    { 0, .000000001, .000000005 },
    { 0, -.000000001, .000000005 },
    { 0, .000000001, -.000000005 },
    { 0, -.000000001, -.000000005 },
    { 0, 1, 2 },
    { 0, 0.5, 0.6 },
    { 0, 0.000005, 0.000006 },
    { 0, 1e100, 2e100 },
    { 1, .9999999999e0, 1e0 },
    { 1, .9999999999e1, 1e1 },
    { 1, .9999999999e10, 1e10 },
    { 1, .9999999999e20, 1e20 },
    { 1, .9999999999e30, 1e30 },
    { 1, .9999999999e31, 1e31 },
    { 1, .9999999999e32, 1e32 },
    { 1, .9999999999e33, 1e33 },
    { 1, .9999999999e50, 1e50 },
    { 1, .9999999999e62, 1e62 },
    { 1, .9999999999e63, 1e63 },
    { 1, .9999999999e64, 1e64 },
    { 1, .9999999999e65, 1e65 },
    { 1, .9999999999e100, 1e100 },
    { 1, .9999999999e200, 1e200 },
    { 1, .9999999999e300, 1e300 },
    { 1, .9999999999e-1, 1e-1 },
    { 1, .9999999999e-2, 1e-2 },
    { 1, .9999999999e-10, 1e-10 },
    { 1, .9999999999e-20, 1e-20 },
    { 1, .9999999999e-30, 1e-30 },
    { 1, .9999999999e-40, 1e-40 },
    { 1, .9999999999e-100, 1e-100 },
    { 1, .9999999999e-200, 1e-200 },
    { 1, .9999999999e-300, 1e-300 },
    { 0, 2e0, 1e0 },
    { 0, 2e1, 1e1 },
    { 0, 2e10, 1e10 },
    { 0, 2e20, 1e20 },
    { 0, 2e30, 1e30 },
    { 0, 2e31, 1e31 },
    { 0, 2e32, 1e32 },
    { 0, 2e33, 1e33 },
    { 0, 2e50, 1e50 },
    { 0, 2e62, 1e62 },
    { 0, 2e63, 1e63 },
    { 0, 2e64, 1e64 },
    { 0, 2e65, 1e65 },
    { 0, 2e100, 1e100 },
    { 0, 2e200, 1e200 },
    { 0, 2e300, 1e300 },
    { 0, 2e-1, 1e-1 },
    { 0, 2e-2, 1e-2 },
    { 0, 2e-10, 1e-10 },
    { 0, 2e-20, 1e-20 },
    { 0, 2e-30, 1e-30 },
    { 0, 2e-40, 1e-40 },
    { 0, 2e-100, 1e-100 },
    { 0, 2e-200, 1e-200 },
    { 0, 2e-300, 1e-300 },
    { -1, 0.0, 0.0 }
  };
  size_t i;
  int rv;
  for (i = 0; tests[i].rv >= 0; ++i) {
    if (tests[i].a == tests[i].b)
      printf("approx failed: malformed test %d: %15.15g == %15.15g\n",
             (int)i, tests[i].a, tests[i].b);
    rv = approx(tests[i].a, tests[i].b);
    if (rv != tests[i].rv)
      printf("approx failed: %15.15g %15.15g -> %d should be %d (test %d) # %15.15g %15.15g\n",
             tests[i].a, tests[i].b, rv, tests[i].rv, (int)i,
             tests[i].a * CUTOFF, tests[i].b * CUTOFF);
  }
}

int
main(void)
{
  test_approx();
  printf("math done\n");
  return 0;
}


#endif
