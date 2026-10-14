#ifndef __LIBK_CTYPE_H__
#define __LIBK_CTYPE_H__

#define DIGIT_CHAR_TO_NUM(c)    ((c) - '0')
#define DIGIT_NUM_TO_CHAR(n)    ((n) + '0')

int toupper(int c);
int tolower(int c);

int isdigit(int c);
int isspace(int c);
int isblank(int c);
int isalnum(int c);
int isalpha(int c);
int isupper(int c);
int islower(int c);

#endif