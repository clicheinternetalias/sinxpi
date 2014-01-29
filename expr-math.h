
#ifndef EXPR_MATH_H
#define EXPR_MATH_H 1

#ifndef M_E
#define M_E           2.7182818284590452354 /* e */
#endif
#ifndef M_EULER
#define M_EULER       0.5772156649015328606 /* Euler-Mascheroni */
#endif
#ifndef M_GAMMA
#define M_GAMMA       0.5772156649015328606 /* Euler-Mascheroni */
#endif
#ifndef M_GOLDEN
#define M_GOLDEN      1.6180339887498948482 /* golden ratio */
#endif
#ifndef M_IGOLDEN
#define M_IGOLDEN     0.6180339887498948482 /* inverse golden ratio */
#endif
#ifndef M_LOG2E
#define M_LOG2E       1.4426950408889634074 /* log_2(e) */
#endif
#ifndef M_LOG10E
#define M_LOG10E      0.43429448190325182765 /* log_10(e) */
#endif
#ifndef M_LN2
#define M_LN2         0.69314718055994530942 /* log_e(2) */
#endif
#ifndef M_LN10
#define M_LN10        2.30258509299404568402 /* log_e(10) */
#endif
#ifndef M_MAGIC
#define M_MAGIC       0.95531661812450927816 /* magic angle */
#endif
#ifndef M_PHI
#define M_PHI         1.6180339887498948482 /* golden ratio */
#endif
#ifndef M_PI
#define M_PI          3.14159265358979323846 /* tau / 2 */
#endif
#ifndef M_1_PI
#define M_1_PI        0.31830988618379067154 /* 1/pi */
#endif
#ifndef M_2_PI
#define M_2_PI        0.63661977236758134308 /* 2/pi */
#endif
#ifndef M_PI1_4
#define M_PI1_4       0.78539816339744830962 /* pi/4 */
#endif
#ifndef M_PI1_2
#define M_PI1_2       1.57079632679489661923 /* pi/2 */
#endif
#ifndef M_PI3_4
#define M_PI3_4       2.35619449019234492884 /* 3 * pi/4 */
#endif
#ifndef M_PLASTIC
#define M_PLASTIC     1.32471795724474602596 /* x ** 3 = x + 1 */
#endif
#ifndef M_SILVER
#define M_SILVER      2.4142135623730950488 /* silver ratio */
#endif
#ifndef M_2_SQRTPI
#define M_2_SQRTPI    1.12837916709551257390 /* 2/sqrt(pi) */
#endif
#ifndef M_SQRT2
#define M_SQRT2       1.41421356237309504880 /* sqrt(2) */
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2     0.70710678118654752440 /* 1/sqrt(2) */
#endif
#ifndef M_SQRT3
#define M_SQRT3       1.73205080756887729352 /* sqrt(3) */
#endif
#ifndef M_TAU
#define M_TAU         6.28318530717958647693 /* better than pi */
#endif
#ifndef M_RAD_TO_DEG
#define M_RAD_TO_DEG  57.2957795130823208768 /* 180/pi, 360/tau */
#endif
#ifndef M_DEG_TO_RAD
#define M_DEG_TO_RAD  0.01745329251994329577 /* pi/180, tau/360 */
#endif

/** Calculate the first root of the quadratic formula.
 *
 * @param a The quadratic coefficient.
 * @param b The linear coefficient.
 * @param c The free term.
 * @return The positive root.
 */
extern double quadratic(double a, double b, double c);

/** Calculate the second root of the quadratic formula.
 *
 * @param a The quadratic coefficient.
 * @param b The linear coefficient.
 * @param c The free term.
 * @return The negative root.
 */
extern double nquadratic(double a, double b, double c);

/** Produce the y value for a triangular wave.
 *
 * @li Domain: (-Infinity, Infinity)
 * @li Co-domain: [0,1]
 * @li Period: 1
 * @li Specific values:
 *   0.0 -> 0
 *   0.5 -> 1
 *   1.0 -> 0
 *
 * @param x The x coordinate.
 * @return The y coordinate.
 */
extern double trianglewave(double x);

/** Test for approximate equality.
 *
 * Test (min / max) for closeness to 1 (i.e., what fraction of
 * 'max' is 'min'?). And since (min / max >= cutoff) is equivalent
 * to (min >= cutoff * max), we can avoid division. (I'm reminded
 * of a young linear interpolation...)
 *
 * The best way to craft the cutoff constant is to subtract epsilon
 * from 1.0, with epsilon defined in terms of the smallest value
 * with exponent 0 (i.e., the ULP of 1.0). Here, we fake it.
 *
 * @note .99999... approaches 1. Thou shall not Proliferate thy Nines
 * lest thou incur the Confusion of the Internal strtod() of the Dread
 * Compiler (Blessed be Her Grammar).
 *
 * @note The approximation threshold scales. As the values approach
 * zero, so does the threshold. In other words, a positive can never
 * approximately equal a negative.
 *
 * @note This has not been thoroughly tested. Approximate function
 * is approximate.
 *
 * @param a,b The values to compare for approximate equality.
 * @return Non-zero if approximately equal, zero if not.
 */
extern int approx(double a, double b);

#endif /* EXPR_MATH_H */
