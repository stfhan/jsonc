#ifndef __MINIRE_H__
#define __MINIRE_H__

#define MINIRE_09 "0123456789"
#define MINIRE_D MINIRE_09
#define MINIRE_az "abcdefghijklmnopqrstuvwxyz"
#define MINIRE_AZ "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define MINIRE_W MINIRE_az MINIRE_AZ

#define MINIRE_WORD MINIRE_az MINIRE_AZ MINIRE_09 "_"

extern int minire(const char *str, const char *pref, const char *characters, const char *suff, char **ret);

#endif