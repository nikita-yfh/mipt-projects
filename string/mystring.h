#ifndef MYSTRING_H
#define MYSTRING_H // override string.h if included firstly :)

#include <stdlib.h>

void *mymemset(void *s, int c, size_t n);

size_t mystrlen(const char *str);

int mystrcmp(const char *s1, const char *s2);

char *mystrcat(char *dest, const char *src);
char *mystrncat(char *dest, const char *src, size_t num);

char *mystrcpy(char *dest, const char *src);
char *mystrncpy(char *dest, const char *src, size_t num);

char *mystrstr1(const char *haystack, const char *needle);
char *mystrstr2(const char *haystack, const char *needle);
char *mystrstr3(const char *haystack, const char *needle);

#endif
