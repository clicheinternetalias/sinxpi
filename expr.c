/* expr.c
 * Tokenize/Parse/Optimize/Evaluate
 */
#include "expr.h"
#include "expr-math.h"
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef TEST
#define NDEBUG 1
#endif
#include <assert.h>

/* ********************************************************************** */
/* Data Types */
/* ********************************************************************** */

/* Used as index into opinfo.
 */
typedef enum expr_oper_e {
#define COMMA ,
#define SYMBOL(ENUM,PREC,TOK,ARGC,DOC,EVAL)  ENUM
#define LIMIT(NAME,VAL)  NAME = VAL
#include "expr-optab.inc"
} expr_oper_t;

/* Missing minimums are zero.
 * Omitted because of compiler warnings about comparisons always being true.
 */
#define op_isVar(OP)    (                         (OP) <= _OP_VAR_MAX  )
#define op_isConst(OP)  ((OP) >= _OP_CONST_MIN && (OP) <= _OP_CONST_MAX)
#define op_isFunc(OP)   ((OP) >= _OP_FUNC_MIN  && (OP) <= _OP_FUNC_MAX )
#define op_isOper(OP)   ((OP) >= _OP_OPER_MIN  && (OP) <= _OP_OPER_MAX )

static struct expr_opinfo_s {
  char * name; /* source token or descriptive name */
  int    argc; /* function argument count and operator operand count */
  int    prec; /* operator precedence */
} expr_opinfo[] = {
#undef COMMA
#undef LIMIT
#define SYMBOL(ENUM,PREC,TOK,ARGC,DOC,EVAL)  { TOK, ARGC, PREC },
#include "expr-optab.inc"
  { NULL, 0, 0 }
};

#define op_name(OP)  expr_opinfo[(OP)].name
#define op_argc(OP)  expr_opinfo[(OP)].argc
#define op_prec(OP)  expr_opinfo[(OP)].prec

/* ********************************************************************** */

typedef struct expr_opcode_s {
  expr_oper_t type;
  double      value;
} OPCODE;

#define opcode_copy(dst,src) do { \
    (dst)->type = (src)->type; \
    (dst)->value = (src)->value; \
  } while (0)

/* ********************************************************************** */

struct EXPR_s {      /* typedef is in expr.h: EXPR */
  size_t   capacity; /* the size of the code and stack arrays */
  OPCODE * code;     /* the compiled program */
  double * stack;    /* the evaluation stack */
};

/* ********************************************************************** */

typedef struct expr_state_s {
  const char * src;       /* source script */
  const char * srcp;      /* first char of next token */
  size_t       curoffs;   /* index of first char of current token */
  OPCODE       tok;       /* current token */
  size_t       dstlen;    /* length of dst buffer */
  OPCODE     * dst;       /* destination buffer */
  OPCODE     * dstp;      /* pointer to output destination */
} EXPRSTATE;

#define CURTOKEN (&(pex->tok))
#define CURTYPE  (pex->tok.type)
#define CURVALUE (pex->tok.value)

/* ********************************************************************** */
/* Error Handling */
/* ********************************************************************** */

static void (*error_handler)(const char *, void *) = NULL;
static void * error_handler_ctxt = NULL;

static void
default_error_handler(const char * msg, void * unused)
{
  unused = unused;
  fprintf(stderr, "%s\n", msg);
  fflush(stderr);
}

static int
expr_error(EXPRSTATE * pex, const char * fmt, ...)
{
  char tmp[2048];
  char buf[2048];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(tmp, sizeof(tmp), fmt, ap);
  va_end(ap);
  snprintf(buf, sizeof(buf), "Syntax error at %lu: %s",
           (unsigned long)pex->curoffs, tmp);
  error_handler(buf, error_handler_ctxt);
  return -1;
}

void
expr_set_error_handler(void (*handle)(const char *, void *), void * ctxt)
{
  error_handler = handle ? handle : default_error_handler;
  error_handler_ctxt = ctxt;
}

/* ********************************************************************** */
/* Tokenizer */
/* ********************************************************************** */

static int
tok_init(EXPRSTATE * pex, const char * src, OPCODE * dst, size_t dstlen)
{
  pex->src = src;
  pex->srcp = src;
  pex->curoffs = 0;
  pex->tok.type = OP_EOF;
  pex->tok.value = 0.0;
  pex->dstlen = dstlen;
  pex->dst = dst;
  pex->dstp = dst;
  return 0;
}

static int
tok_next(EXPRSTATE * pex)
{
  char buf[64];
  size_t idx = 0;

#define p   (pex->srcp)
#define GATHER(TEST) do { \
    while (idx < sizeof(buf) - 1 && TEST(*(p + idx))) { \
      buf[idx] = *(p + idx); idx++; \
    } \
    buf[idx] = '\0'; \
  } while (0)

  CURVALUE = 0.0;

  /* *** Whitespace / End of Input *** */

  while (*p && *p <= ' ') p++;
  pex->curoffs = (size_t)(pex->srcp - pex->src) + 1;

  if (!*p) { CURTYPE = OP_EOF; return 0; }

  /* *** Numbers *** */

  if ((*p >= '0' && *p <= '9') || *p == '.') {
    char * tmp = NULL;
    errno = 0;
    if (*p == '0' && (*(p+1) == 'x' || *(p+1) == 'X')) {
      CURVALUE = (double)strtoul(p, &tmp, 16);
    } else {
      CURVALUE = strtod(p, &tmp);
    }
    if (errno || !tmp || p == tmp)
      return expr_error(pex, "invalid number");
    p = tmp;
    CURTYPE = OP_NUMBER;
    return 0;
  }

  /* *** Identifiers *** */

  if (isalpha(*p)) {
    GATHER(isalnum);
    for (CURTYPE = _OP_IDENT_MIN; CURTYPE <= _OP_IDENT_MAX; CURTYPE++) {
      if (!strcmp(op_name(CURTYPE), buf)) { p += idx; return 0; }
    }
    return expr_error(pex, "unknown identifier '%s'", buf);
  }

  /* *** Operators *** */

  if (ispunct(*p)) {
    GATHER(ispunct);
    do {
      for (CURTYPE = _OP_OPER_MIN; CURTYPE <= _OP_OPER_MAX; CURTYPE++) {
        if (!strcmp(op_name(CURTYPE), buf)) { p += idx; return 0; }
      }
      buf[--idx] = '\0';
    } while (idx);
  }
  if (isprint(*p)) return expr_error(pex, "unknown character '%c'", *p);
  return expr_error(pex, "unknown character '\\x%02X'", *p);
#undef GATHER
#undef p
}

/* ********************************************************************** */
/* Parser */
/* ********************************************************************** */

#define Q(X)              if ((X)) return -1
#define Q_NEXT()          Q(tok_next(pex))
#define Q_ADVANCE(dst)    opcode_copy((dst), CURTOKEN); Q_NEXT()
#define Q_REQUIRE(OP)  do { \
    if (CURTYPE == (OP)) { Q_NEXT(); } else { \
      return expr_error(pex, "unexpected '%s', expected '%s'", \
                        op_name(CURTYPE), op_name(OP)); \
    } \
  } while (0)

#define Q_PARSE_LEVEL(n)   Q(expr_prec_level(pex, (n)))
#define Q_PARSE_PRIMARY()  Q(expr_prec_primary(pex))
#define Q_APPEND(TK)  do { \
    assert((size_t)(pex->dstp - pex->dst) < pex->dstlen); \
    opcode_copy(pex->dstp, (TK)); \
    pex->dstp++; \
  } while (0)

static int expr_prec_level(EXPRSTATE * pex, int prec);

/* primary : NUMBER | VAR | IDENT '(' args? ')' | '(' expr ')'
 *         | unary_op primary ;
 * args : expr | args ',' expr ;
 */
static int
expr_prec_primary(EXPRSTATE * pex)
{
  OPCODE tmp;
  if (CURTYPE == OP_NUMBER || op_isConst(CURTYPE) || op_isVar(CURTYPE)) {
    Q_APPEND(CURTOKEN);
    Q_NEXT();
    return 0;
  }
  if (op_isFunc(CURTYPE)) {
    int argc = 0;
    Q_ADVANCE(&tmp);
    Q_REQUIRE(OP_OPEN);
    if (CURTYPE != OP_CLOSE) {
      Q_PARSE_LEVEL(0);
      argc++;
      while (CURTYPE == OP_COMMA) {
        Q_NEXT();
        Q_PARSE_LEVEL(0);
        argc++;
      }
    }
    Q_REQUIRE(OP_CLOSE);

    if (argc != op_argc(tmp.type))
      return expr_error(pex, "function takes %d args, found %d",
                        op_argc(tmp.type), argc);
    Q_APPEND(&tmp);
    return 0;
  }
  if (CURTYPE == OP_OPEN) {
    Q_NEXT();
    Q_PARSE_LEVEL(0);
    Q_REQUIRE(OP_CLOSE);
    return 0;
  }
  if (CURTYPE == OP_LOGNOT || CURTYPE == OP_BITNOT ||
      CURTYPE == OP_ADD || CURTYPE == OP_SUB) {
    Q_ADVANCE(&tmp);
    if      (tmp.type == OP_ADD) tmp.type = OP_POS;
    else if (tmp.type == OP_SUB) tmp.type = OP_NEG;
    Q_PARSE_PRIMARY();
    Q_APPEND(&tmp);
    return 0;
  }
  return expr_error(pex, "unexpected '%s'", op_name(CURTYPE));
}

/* level(n) : primary
 *          | level(n) op(n) level(n+1)
 *          | level(n) "?" level(0) ":" level("?") ;
 */
static int
expr_prec_level(EXPRSTATE * pex, int prec)
{
  OPCODE tmp;
  Q_PARSE_PRIMARY();
  while (op_isOper(CURTYPE) && op_argc(CURTYPE) >= 2 && op_prec(CURTYPE) >= prec) {
    Q_ADVANCE(&tmp);
    if (tmp.type == OP_COND) {
      Q_PARSE_LEVEL(0);
      Q_REQUIRE(OP_COLON);
      Q_PARSE_LEVEL(op_prec(tmp.type));
    } else {
      Q_PARSE_LEVEL(op_prec(tmp.type) + 1); /* all binops are left-assoc */
    }
    Q_APPEND(&tmp);
  }
  return 0;
}

/* parse : level(0) EOF ;
 */
static int
expr_parse(const char * src, OPCODE * out, size_t outlen)
{
  EXPRSTATE pex0;
#define pex  (&pex0)
  tok_init(pex, src, out, outlen);
  Q_NEXT();
  Q_PARSE_LEVEL(0);
  Q_REQUIRE(OP_EOF);
  Q_APPEND(CURTOKEN);
  return 0;
#undef pex
}

/* ********************************************************************** */
/* Evaluator */
/* ********************************************************************** */

int
expr_eval(const EXPR * ex, double x, double * rv)
{
  OPCODE * op;
  double * dst;

  if (!ex || !rv) return -1;

  for (op = ex->code, dst = ex->stack;
       op->type != OP_EOF;
       op++, dst++) {
    dst -= op_argc(op->type);
    switch (op->type) {
#undef COMMA
#undef LIMIT
#define zz  op->value
#define aa  dst[0]
#define bb  dst[1]
#define cc  dst[2]
#define SYMBOL(ENUM,PREC,TOK,ARGC,DOC,EVAL) case ENUM: dst[0]=(double)(EVAL); break;
#include "expr-optab.inc"
    }
  }
  assert((dst - ex->stack) == 1);
  *rv = ex->stack[0];
  return 0;
}

/* ********************************************************************** */
/* Constructor */
/* ********************************************************************** */

EXPR *
expr_new(const char * src)
{
  EXPR * ex;
  size_t srclen;

  if (!error_handler) expr_set_error_handler(NULL, NULL);

  if (!src || !*src) src = "x";

  /* worst case: opcode count == srclen + eof
   * worst case: func(every, token, gets, pushed, onto, the, stack)
   */
  srclen = strlen(src) + 1;

  if (srclen >= (size_t)(INT_MAX / sizeof(OPCODE)))
    return NULL; /* don't worry too much: 32-bits -> ~536M opcodes */

  ex = (EXPR *)calloc(1, sizeof(EXPR));
  assert(ex);
  if (!ex) goto error;

  ex->code = (OPCODE *)malloc(sizeof(OPCODE) * srclen);
  assert(ex->code);
  if (!ex->code) goto error;

  ex->stack = (double *)malloc(sizeof(double) * srclen);
  assert(ex->stack);
  if (!ex->stack) goto error;

  ex->capacity = srclen;
  
  if (expr_parse(src, ex->code, ex->capacity)) goto error;
  return ex;
error:
  expr_delete(ex);
  return NULL; /* no error printing on out-of-mem */
}

void
expr_delete(EXPR * ex)
{
  if (ex) {
    if (ex->stack) free(ex->stack);
    if (ex->code) free(ex->code);
    free(ex);
  }
}

/* ********************************************************************** */
/* Test */
/* ********************************************************************** */

#ifdef TEST

struct test_s {
  double rv;
  char * src;
} tests[] = {
  { 0.5, "x" },
  { 1.0/M_PHI, "1/PHI" },
  { M_PHI, "PHI" },
  { M_E, "E" },
  { M_GAMMA, "GAMMA" },
  { 55.0*5.0, "55*5" },
  { 1.0+ +2e3, "1++2e3" },
  { 1.0?2.0:3.0, "1?2:3" },
  { 1.0+2.0?3.0+4.0:5.0+6.0, "1+2?3+4:5+6" },
  { 0.0?1.0:2.0 ? 3.0?4.0:5.0 : 6.0?7.0:8.0, "0?1:2 ? 3?4:5 : 6?7:8" },
  { 5.0+0.5*7.0, "5+x*7" },
  { 5.0*0.5+7.0, "5*x+7" },
  { +5+-3- -3+ +3-+3, "+5+-3--3++3-+3" },
  { ~-!-3, "~-!-3" },
  { 5>>3>=2, "5>>3>=2" },
  { 1.5875015614538918, "log((5 * 9),(4 + 7))" },
  { 0.9163138199826525, "log(5 ? 9 : 8,4 + 7)" },
  { -(-2.0+5.0/3.0), "abs(-2+5/3)" },
  { 0.0, "0 && x" },
  { M_TAU, "TAU || x" },
  { -0.0, "-0" },
  { INFINITY, "1/0" },
  { 4.0, "atanh(5)??4" },
  { 1.0, "5>4>=3>>2" },
  { 1.0, "5<4<=3<<2" },
  { 0.0, "5==4!=3==!2" },
  { 3.0, "6^5 && 4&3 || 2|1" },
  { 7.0, "7 || 6|5 && 4&3" },
  { 5.0, "root(5*5, 2)" },
  { 7.0, "root(7*7*7, 3) == 7 ? 44 : cbrt(7*7*7)" },
  { 9.0, "root(9*9*9*9, 4)" },
  { 0.0, NULL }
};

void
print_opcode(OPCODE * opc)
{
  if (opc->type == OP_NUMBER) {
    fprintf(stdout, "(%.23g)", opc->value);
  } else
    fprintf(stdout, "%s", op_name(opc->type));

  if (opc->type == OP_EOF) {
    fprintf(stdout, "\n");
    fflush(stdout);
  }
}

void
test_parse(void)
{
  double rv = 0.0;
  EXPR * ex;
  size_t i, j;
  int err;
  for (i = 0; tests[i].src; i++) {

    printf("parsing '%s'...\n", tests[i].src); fflush(stdout);
    ex = expr_new(tests[i].src);
    if (!ex) {
      printf("parse failed.\n"); fflush(stdout);
      continue;
    }
    printf("parsed. evaling...\n"); fflush(stdout);

    err = expr_eval(ex, 0.5, &rv);
    printf("eval'ed\n"); fflush(stdout);

    if (err || rv != tests[i].rv) {
      fprintf(stdout, "    failed: \"%s\": %.23g should be %.23g\n",
              tests[i].src, rv, tests[i].rv); fflush(stdout);

      fprintf(stdout, "    ");
      for (j = 0; j < ex->capacity; j++) {
        print_opcode(&(ex->code[j]));
        if (ex->code[j].type == OP_EOF) break;
        fprintf(stdout, " ");
        fflush(stdout);
      }
    }
    expr_delete(ex);
  }
}

void
test_token(void)
{
  OPCODE buf[2048];
  EXPRSTATE ex;
  EXPRSTATE * pex = &ex;
  size_t i;
  for (i = 0; tests[i].src; i++) {
    printf("tokenizing '%s'...\n", tests[i].src); fflush(stdout);
    tok_init(pex, tests[i].src, buf, 2048);
    while (!tok_next(pex)) {
      printf("  %s %g\n", op_name(CURTYPE), CURVALUE); fflush(stdout);
      if (CURTYPE == OP_EOF) break;
    }
    printf("tokenized\n"); fflush(stdout);
  }
}

int
main(void)
{
  expr_set_error_handler(NULL, NULL);
  test_token();
  test_parse();
  printf("done\n");
  return 0;
}

#endif

/* ********************************************************************** */
/* ********************************************************************** */
