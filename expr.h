/* expr.h
 * Parse and evaluate expressions.
 * The only variable is 'x'.
 */
#ifndef EXPR_H_
#define EXPR_H_ 1

/** The EXPR program.
 * This is an opaque type which cannot be instantiated directly.
 */
typedef struct EXPR_s EXPR;

/** Parse and compile an expression into a program.
 *
 * @param src The source code of the expression.
 * @return The compiled program.
 * @see expr_set_error_handler
 */
extern EXPR * expr_new(const char * src);

/** Free an EXPR program.
 *
 * @param ex The EXPR program to destroy.
 */
extern void expr_delete(EXPR * ex);

/** Evaluate an expression program for a given value of 'x'.
 *
 * @param ex The expression program to evaluate.
 * @param x The value of the 'x' variable.
 * @param[out] rv The location of the expression's resulting value.
 * @return 0 on success. The current implementation always succeeds.
 */
extern int expr_eval(const EXPR * ex, double x, double * rv);

/** Set the callback function to report parsing errors.
 *
 * @param handle The function to call on error. Pass NULL to print to stderr.
 * @param ctxt Context data for the callback.
 */
extern void expr_set_error_handler(void (*handle)(const char *, void *), void * ctxt);

#endif /* EXPR_H_ */
