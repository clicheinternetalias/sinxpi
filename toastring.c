
#include "toastring.h"
#include <libgimp/gimp.h>

size_t
toa_strcnt(const char * s, int c)
{
  char cc = (char)c;
  size_t rv = 0;
  while (*s) if (*s++ == cc) ++rv;
  return rv;
}

char *
toa_strtrim(char * s)
{
  char * dst = s;
  char * src = s;
  while (*src && *src <= ' ') ++src;
  while (*src) {
    if (*src <= ' ') {
      int nl = 0;
      while (*src && *src <= ' ') {
        nl += (*src == '\n' || *src == '\r');
        ++src;
      }
      *dst++ = nl ? '\n' : ' ';
    } else {
      *dst++ = *src++;
    }
  }
  while (dst > s && *(dst - 1) <= ' ') --dst;
  *dst = '\0';
  return s;
}

char **
toa_strparse(const char * s)
{
  char ** rv;
  char * d;
  if (!s || !*s) return NULL;
  d = g_strdup(s);
  rv = g_strsplit(toa_strtrim(d), "\n", -1);
  g_free(d);
  return rv;
}

#ifdef TEST
#include <stdio.h>

int
main(void)
{
  struct test_s {
    char * in;
    char * out;
  } tests[] = {
    { "  trim1   trim2 \n trim3 \n \n trim4\ntrim5  ", "trim1 trim2\ntrim3\ntrim4\ntrim5" },
    { "trim1   trim2 \n trim3 \n \n trim4\ntrim5  ", "trim1 trim2\ntrim3\ntrim4\ntrim5" },
    { "  trim1   trim2 \n trim3 \n \n trim4\ntrim5", "trim1 trim2\ntrim3\ntrim4\ntrim5" },
    { "trim1   trim2 \n trim3 \n \n trim4\ntrim5", "trim1 trim2\ntrim3\ntrim4\ntrim5" },
    { "trim1 trim2\ntrim3\ntrim4\ntrim5", "trim1 trim2\ntrim3\ntrim4\ntrim5" },
    { " trim1 ", "trim1" },
    { "trim1 ", "trim1" },
    { " trim1", "trim1" },
    { "trim1", "trim1" },
    { "  ", "" },
    { " ", "" },
    { "", "" },
    { NULL, NULL }
  };
  int i;
  for (i = 0; tests[i].in; ++i) {
    char * t = g_strdup(tests[i].in);
    toa_strtrim(t);
    if (strcmp(t, tests[i].out))
      fprintf(stderr, "%d '%s' != '%s'\n", i, tests[i].out, t);
    g_free(t);
  }
  fprintf(stderr, "done\n");
  return 0;
}

#endif
