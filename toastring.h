
#ifndef _TOASTRING_H_
#define _TOASTRING_H_ 1

#include <stddef.h>

/** Count the occurances of a byte in a string.
 * @param s The string to search in.
 * @param c The byte value to search for.
 * @return The number of times the specified value was found.
 */
extern size_t toa_strcnt(const char * s, int c);

/** Collapse white-space in a string.
 *
 * Multiple newlines separated only by white-space will be collapsed
 * to a single newline. Leading and trailing white-space will be removed.
 *
 * The operation is performed in-place.
 *
 * @param s The string to trim.
 * @return The string that was trimmed.
 */
extern char * toa_strtrim(char * s);

/** Parse a string into an array of strings.
 *
 * The string will be split into a white-space collapsed array of lines.
 * The white-space will be collapsed as if by a call to toa_strtrim.
 *
 * @param s The string to split into lines.
 * @return A newly-allocated NULL-terminated array of strings.
 *         Use g_strfreev() to free it.
 */
extern char ** toa_strparse(const char * s);

#endif /* _TOASTRING_H_ */
