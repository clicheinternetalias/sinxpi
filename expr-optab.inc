/* Operators, functions, etc.
 */
#ifndef COMMA
#define COMMA /* */
#endif

/* ********************************************************************** */
/* ********************************************************************** */

#ifdef SYMBOL
/* SYMBOL(ENUM, PREC, TOKEN, ARGS, DOC, EVAL) */

/* Identifiers: Variables, Constants, Functions */

/* Variables */

SYMBOL(OP_X,        0, "x",        0, "[0..1]",      x) COMMA

/* Constants */

SYMBOL(OP_E,        0, "E",        0, "(2.718...)",   M_E) COMMA
SYMBOL(OP_EULER,    0, "EULER",    0, "(0.577...)",   M_EULER) COMMA
SYMBOL(OP_GAMMA,    0, "GAMMA",    0, "(0.577...)",   M_GAMMA) COMMA
SYMBOL(OP_GOLDEN,   0, "GOLDEN",   0, "(1.618...)",   M_GOLDEN) COMMA
SYMBOL(OP_IGOLDEN,  0, "IGOLDEN",  0, "(0.618...)",   M_IGOLDEN) COMMA
SYMBOL(OP_INF,      0, "INFINITY", 0, "",             INFINITY) COMMA
SYMBOL(OP_LN2,      0, "LN2",      0, "(0.693...)",   M_LN2) COMMA
SYMBOL(OP_LN10,     0, "LN10",     0, "(2.302...)",   M_LN10) COMMA
SYMBOL(OP_LOG2E,    0, "LOG2E",    0, "(1.442...)",   M_LOG2E) COMMA
SYMBOL(OP_LOG10E,   0, "LOG10E",   0, "(0.434...)",   M_LOG10E) COMMA
SYMBOL(OP_MAGIC,    0, "MAGIC",    0, "(0.955...)",   M_MAGIC) COMMA
SYMBOL(OP_NAN,      0, "NAN",      0, "",             NAN) COMMA
SYMBOL(OP_PHI,      0, "PHI",      0, "(1.618...)",   M_PHI) COMMA
SYMBOL(OP_PI,       0, "PI",       0, "(3.141...)",   M_PI) COMMA
SYMBOL(OP_PI1,      0, "PI1",      0, "(0.318...)",   M_1_PI) COMMA
SYMBOL(OP_PI2,      0, "PI2",      0, "(0.636...)",   M_2_PI) COMMA
SYMBOL(OP_PI14,     0, "PI14",     0, "(0.785...)",   M_PI1_4) COMMA
SYMBOL(OP_PI12,     0, "PI12",     0, "(1.570...)",   M_PI1_2) COMMA
SYMBOL(OP_PI34,     0, "PI34",     0, "(2.356...)",   M_PI3_4) COMMA
SYMBOL(OP_PLASTIC,  0, "PLASTIC",  0, "(1.324...)",   M_PLASTIC) COMMA
SYMBOL(OP_SILVER,   0, "SILVER",   0, "(2.414...)",   M_SILVER) COMMA
SYMBOL(OP_SQRT12,   0, "SQRT12",   0, "(0.707...)",   M_SQRT1_2) COMMA
SYMBOL(OP_SQRT2,    0, "SQRT2",    0, "(1.414...)",   M_SQRT2) COMMA
SYMBOL(OP_SQRT3,    0, "SQRT3",    0, "(1.732...)",   M_SQRT3) COMMA
SYMBOL(OP_SQRTPI2,  0, "SQRTPI2",  0, "(1.128...)",   M_2_SQRTPI) COMMA
SYMBOL(OP_TAU,      0, "TAU",      0, "(6.283...)",   M_TAU) COMMA

/* Functions */

SYMBOL(OP_ABS,      0, "abs",      1, "(v)",          fabs(aa)) COMMA
SYMBOL(OP_CBRT,     0, "cbrt",     1, "(v)",          cbrt(aa)) COMMA
SYMBOL(OP_CEIL,     0, "ceil",     1, "(v)",          ceil(aa)) COMMA
SYMBOL(OP_CLAMP,    0, "clamp",    3, "(v,min,max)",  aa < bb ? bb : aa > cc ? cc : aa) COMMA
SYMBOL(OP_D2R,      0, "d2r",      1, "(degree)",     aa * M_DEG_TO_RAD) COMMA
SYMBOL(OP_DIFF,     0, "diff",     2, "(a,b)",        fdim(aa, bb)) COMMA
SYMBOL(OP_ERF,      0, "erf",      1, "(v)",          erf(aa)) COMMA
SYMBOL(OP_EXP,      0, "exp",      1, "(v)",          exp(aa)) COMMA /* e ** aa */
SYMBOL(OP_EXP1M,    0, "exp1m",    1, "(v)",          expm1(aa)) COMMA /* (e ** aa) - 1 */
SYMBOL(OP_FLOOR,    0, "floor",    1, "(v)",          floor(aa)) COMMA
SYMBOL(OP_TGAMMA,   0, "gamma",    1, "(v)",          tgamma(aa)) COMMA
SYMBOL(OP_HYPOT,    0, "hypot",    2, "(x,y)",        hypot(aa, bb)) COMMA
SYMBOL(OP_ISEVEN,   0, "iseven",   1, "(v)",          fmod(aa, 2.0) == 0.0) COMMA
SYMBOL(OP_ISFINITE, 0, "isfinite", 1, "(v)",          !!isfinite(aa)) COMMA
SYMBOL(OP_ISINF,    0, "isinf",    1, "(v)",          !!isinf(aa)) COMMA
SYMBOL(OP_ISNAN,    0, "isnan",    1, "(v)",          !!isnan(aa)) COMMA
SYMBOL(OP_ISODD,    0, "isodd",    1, "(v)",          fmod(aa, 2.0) == 1.0) COMMA
SYMBOL(OP_J0,       0, "j0",       1, "(v)",          j0(aa)) COMMA
SYMBOL(OP_J1,       0, "j1",       1, "(v)",          j1(aa)) COMMA
SYMBOL(OP_JN,       0, "jn",       2, "(n,v)",        jn((int)aa, bb)) COMMA
SYMBOL(OP_LERP,     0, "lerp",     3, "(t,min,max)",  bb + aa * (cc - bb)) COMMA
SYMBOL(OP_LGAMMA,   0, "lgamma",   1, "(v)",          lgamma(aa)) COMMA
SYMBOL(OP_LOG2,     0, "log2",     1, "(v)",          log2(aa)) COMMA /* base 2 */
SYMBOL(OP_LOG10,    0, "log10",    1, "(v)",          log10(aa)) COMMA /* base 10 */
SYMBOL(OP_LN,       0, "ln",       1, "(v)",          log(aa)) COMMA /* base e */
SYMBOL(OP_LN1P,     0, "ln1p",     1, "(v)",          log1p(aa)) COMMA /* base e (aa + 1) */
SYMBOL(OP_LOG,      0, "log",      2, "(b,v)",        log(aa) / log(bb)) COMMA /* base any */
SYMBOL(OP_MAX,      0, "max",      2, "(a,b)",        fmax(aa, bb)) COMMA
SYMBOL(OP_MIN,      0, "min",      2, "(a,b)",        fmin(aa, bb)) COMMA
SYMBOL(OP_ORDERED,  0, "ordered",  3, "(a,b,c)",      fmax(aa, bb) == fmin(bb, cc)) COMMA /* a <= b <= c */
SYMBOL(OP_NQUAD,    0, "nquad",    3, "(a,b,c)",      nquadratic(aa, bb, cc)) COMMA
SYMBOL(OP_POW,      0, "pow",      2, "(v,e)",        pow(aa, bb)) COMMA
SYMBOL(OP_QUAD,     0, "quad",     3, "(a,b,c)",      quadratic(aa, bb, cc)) COMMA
SYMBOL(OP_R2D,      0, "r2d",      1, "(radian)",     aa * M_RAD_TO_DEG) COMMA
SYMBOL(OP_ROOT,     0, "root",     2, "(v,r)",        pow(aa, 1 / bb)) COMMA
SYMBOL(OP_ROUND,    0, "round",    1, "(v)",          round(aa)) COMMA
SYMBOL(OP_SIGN,     0, "sign",     1, "(v)",          copysign(1.0, aa)) COMMA
SYMBOL(OP_SQRT,     0, "sqrt",     1, "(v)",          sqrt(aa)) COMMA
SYMBOL(OP_SQUARE,   0, "square",   1, "(v)",          aa * aa) COMMA
SYMBOL(OP_TRIWAVE,  0, "triwave",  1, "(v)",          trianglewave(aa)) COMMA
SYMBOL(OP_UNLERP,   0, "unlerp",   3, "(v,min,max)",  (aa - bb) / (cc - bb)) COMMA /* (mid, min, max) */
SYMBOL(OP_Y0,       0, "y0",       1, "(v)",          y0(aa)) COMMA
SYMBOL(OP_Y1,       0, "y1",       1, "(v)",          y1(aa)) COMMA
SYMBOL(OP_YN,       0, "yn",       2, "(n,v)",        yn((int)aa, bb)) COMMA

SYMBOL(OP_SINC,     0, "sinc",     1, "(r)",          aa ? sin(aa) / aa : 1.0) COMMA /* sine cardinal */
SYMBOL(OP_COSC,     0, "cosc",     1, "(r)",          aa ? cos(aa) / aa : 1.0) COMMA
SYMBOL(OP_TANC,     0, "tanc",     1, "(r)",          aa ? tan(aa) / aa : 1.0) COMMA
SYMBOL(OP_SINK,     0, "sink",     1, "(r)",          log(fabs(aa ? sin(aa) / aa : 1.0))) COMMA /* kilroy curve */
SYMBOL(OP_COSK,     0, "cosk",     1, "(r)",          log(fabs(aa ? cos(aa) / aa : 1.0))) COMMA
SYMBOL(OP_TANK,     0, "tank",     1, "(r)",          log(fabs(aa ? tan(aa) / aa : 1.0))) COMMA

SYMBOL(OP_SIN,      0, "sin",      1, "(r)",          sin(aa)) COMMA
SYMBOL(OP_COS,      0, "cos",      1, "(r)",          cos(aa)) COMMA
SYMBOL(OP_TAN,      0, "tan",      1, "(r)",          tan(aa)) COMMA
SYMBOL(OP_CSC,      0, "csc",      1, "(r)",          1 / sin(aa)) COMMA
SYMBOL(OP_SEC,      0, "sec",      1, "(r)",          1 / cos(aa)) COMMA
SYMBOL(OP_COT,      0, "cot",      1, "(r)",          1 / tan(aa)) COMMA
SYMBOL(OP_ASIN,     0, "asin",     1, "(r)",          asin(aa)) COMMA
SYMBOL(OP_ACOS,     0, "acos",     1, "(r)",          acos(aa)) COMMA
SYMBOL(OP_ATAN,     0, "atan",     1, "(r)",          atan(aa)) COMMA
SYMBOL(OP_ATAN2,    0, "atan2",    2, "(y,x)",        atan2(aa, bb)) COMMA
SYMBOL(OP_ACSC,     0, "acsc",     1, "(r)",          asin(1 / aa)) COMMA
SYMBOL(OP_ASEC,     0, "asec",     1, "(r)",          acos(1 / aa)) COMMA
SYMBOL(OP_ACOT,     0, "acot",     1, "(r)",          atan(1 / aa)) COMMA
SYMBOL(OP_SINH,     0, "sinh",     1, "(r)",          sinh(aa)) COMMA
SYMBOL(OP_COSH,     0, "cosh",     1, "(r)",          cosh(aa)) COMMA
SYMBOL(OP_TANH,     0, "tanh",     1, "(r)",          tanh(aa)) COMMA
SYMBOL(OP_CSCH,     0, "csch",     1, "(r)",          1 / sinh(aa)) COMMA
SYMBOL(OP_SECH,     0, "sech",     1, "(r)",          1 / cosh(aa)) COMMA
SYMBOL(OP_COTH,     0, "coth",     1, "(r)",          1 / tanh(aa)) COMMA
SYMBOL(OP_ASINH,    0, "asinh",    1, "(r)",          asinh(aa)) COMMA
SYMBOL(OP_ACOSH,    0, "acosh",    1, "(r)",          acosh(aa)) COMMA
SYMBOL(OP_ATANH,    0, "atanh",    1, "(r)",          atanh(aa)) COMMA
SYMBOL(OP_ACSCH,    0, "acsch",    1, "(r)",          asinh(1 / aa)) COMMA
SYMBOL(OP_ASECH,    0, "asech",    1, "(r)",          acosh(1 / aa)) COMMA
SYMBOL(OP_ACOTH,    0, "acoth",    1, "(r)",          atanh(1 / aa)) COMMA

/* Operators: Tokens, Non-Tokens */

/* Tokens */

SYMBOL(OP_COMMA,    0, ",",        0, NULL,           0.0) COMMA
SYMBOL(OP_COLON,    0, ":",        0, NULL,           0.0) COMMA
SYMBOL(OP_CLOSE,    0, ")",        0, NULL,           0.0) COMMA
SYMBOL(OP_OPEN,     0, "(",        0, ") sub-expr",   0.0) COMMA
SYMBOL(OP_LOGNOT,   0, "!",        1, "log-not",      !aa) COMMA
SYMBOL(OP_BITNOT,   0, "~",        1, "bit-not",      ~(int)(aa)) COMMA

SYMBOL(OP_MUL,     12, "*",        2, "mul",          aa * bb) COMMA
SYMBOL(OP_DIV,     12, "/",        2, "div",          aa / bb) COMMA
SYMBOL(OP_IDIV,    12, "//",       2, "idiv",         (long int)aa / (long int)bb) COMMA
SYMBOL(OP_MOD,     12, "%",        2, "mod",          fmod(aa, bb)) COMMA
SYMBOL(OP_IMOD,    12, "%%",       2, "imod",         (long int)aa % (long int)bb) COMMA
SYMBOL(OP_ADD,     11, "+",        2, "add",          aa + bb) COMMA
SYMBOL(OP_SUB,     11, "-",        2, "sub",          aa - bb) COMMA
SYMBOL(OP_SHL,     10, "<<",       2, "shl",          (long int)(aa) << ((unsigned)(bb) & 0x1F)) COMMA
SYMBOL(OP_SHR,     10, ">>",       2, "shr",          (long int)(aa) >> ((unsigned)(bb) & 0x1F)) COMMA
SYMBOL(OP_USHR,    10, ">>>",      2, "ushr",         (unsigned long)(aa) >> ((unsigned)(bb) & 0x1F)) COMMA
SYMBOL(OP_BITAND,   9, "&",        2, "bit-and",      (long int)(aa) & (long int)(bb)) COMMA
SYMBOL(OP_BITXOR,   8, "^",        2, "bit-xor",      (long int)(aa) | (long int)(bb)) COMMA
SYMBOL(OP_BITOR,    7, "|",        2, "bit-or",       (long int)(aa) ^ (long int)(bb)) COMMA
SYMBOL(OP_LT,       6, "<",        2, "lt",           aa < bb) COMMA
SYMBOL(OP_GT,       6, ">",        2, "gt",           aa > bb) COMMA
SYMBOL(OP_LE,       6, "<=",       2, "le",           aa <= bb) COMMA
SYMBOL(OP_GE,       6, ">=",       2, "ge",           aa >= bb) COMMA
SYMBOL(OP_APPROXLE, 6, "~<=",      2, "approx. le",   aa <= bb || approx(aa, bb)) COMMA
SYMBOL(OP_APPROXGE, 6, "~>=",      2, "approx. ge",   aa >= bb || approx(aa, bb)) COMMA
SYMBOL(OP_EQ,       5, "==",       2, "eq",           aa == bb) COMMA
SYMBOL(OP_NE,       5, "!=",       2, "ne",           aa != bb) COMMA
SYMBOL(OP_APPROXEQ, 5, "~==",      2, "approx. eq",   approx(aa, bb)) COMMA
SYMBOL(OP_APPROXNE, 5, "~!=",      2, "approx. ne",   !approx(aa, bb)) COMMA
SYMBOL(OP_LOGAND,   4, "&&",       2, "log-and",      !aa ? aa : bb) COMMA
SYMBOL(OP_LOGOR,    3, "||",       2, "log-or",       !!aa ? aa : bb) COMMA
SYMBOL(OP_COAL,     2, "??",       2, "coalesce",     !isnan(aa) ? aa : bb) COMMA
SYMBOL(OP_COND,     1, "?",        3, ": ternary",    aa ? bb : cc) COMMA

/* Non-Tokens */

SYMBOL(OP_POS,      0, "+u",       1, NULL,           +aa) COMMA
SYMBOL(OP_NEG,      0, "-u",       1, "neg",          -aa) COMMA
SYMBOL(OP_NUMBER,   0, "number",   0, NULL,           zz) COMMA
SYMBOL(OP_EOF,      0, "end",      0, NULL,           0.0)

#ifdef LIMIT
COMMA
#endif
#endif
#ifdef LIMIT

/* Sub-Ranges */

LIMIT(_OP_IDENT_MIN, OP_X) COMMA
LIMIT(_OP_IDENT_MAX, OP_ACOTH) COMMA

LIMIT(_OP_VAR_MIN, OP_X) COMMA
LIMIT(_OP_VAR_MAX, OP_X) COMMA

LIMIT(_OP_CONST_MIN, OP_E) COMMA
LIMIT(_OP_CONST_MAX, OP_TAU) COMMA

LIMIT(_OP_FUNC_MIN, OP_D2R) COMMA
LIMIT(_OP_FUNC_MAX, OP_ACOTH) COMMA

LIMIT(_OP_OPER_MIN, OP_COMMA) COMMA
LIMIT(_OP_OPER_MAX, OP_COND) COMMA

LIMIT(_OP_MIN, OP_X) COMMA
LIMIT(_OP_MAX, OP_EOF)

#endif

/* ********************************************************************** */
/* ********************************************************************** */

#undef SYMBOL
#undef LIMIT
#undef COMMA
#undef aa
#undef bb
#undef cc
#undef zz
