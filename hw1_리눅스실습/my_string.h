//-----------------------------------------------------------
// 
// SWE2007: Software Experiment II (2018 Fall)
//
// Skeleton code for PA #0
// September 12, 2018
//
// Somm Kim, Sooyun Lee
// Embedded Software Laboratory
// Sungkyunkwan University
//
// Student Name	: lee young suk ÀÌ¿µ¼÷
// Student ID	: 2014314433
//
//-----------------------------------------------------------

#include <stddef.h>

/* Conversion of string to numeric format */
extern int my_atoi (const char *str);
extern long my_atol (const char *str);

/* Conversion of numeric format to string */
extern char *int2str (char *dest, int num);

/* String manipulation */
extern char *strcpy (char *dest, const char *src);
extern char *strncpy (char *dest, const char *src, size_t n);
extern char *strcat (char *dest, const char *src);
extern char *strncat (char *dest, const char *src, size_t n);
extern char *strdup (const char *str);

/* String examination */
extern size_t strlen (const char *str);
extern int strcmp (const char *str1, const char *str2);
extern int strncmp (const char *str1, const char *str2, size_t n);
extern char *strchr (const char *str, int c);
extern char *strrchr (const char *str, int c);
extern char *strpbrk (const char *str1, const char *str2);
extern char *strstr (const char *haystack, const char *needle);
extern char *strtok (char *str, const char *delim);
extern char *strtok_r (char *str, const char *delim, char **save_ptr);

/* Character array manipulation */
extern void *memcpy (void *dest, const void *src, size_t n);
extern void *memset (void *str, int c, size_t n);
