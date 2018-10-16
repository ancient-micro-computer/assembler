#include <iostream>
#include <vcclr.h>
#include "windows.h"

using namespace std;

unsigned long htoi(char *str);
int String2Charp(System::String^ src, char **dst);
System::String^ Charp2String(char *src);
int Split(char *p_Splitstr, char *p_Delimiter);
int Replace(char *p_Src, char *p_a, char *p_b, char *io_Dst);
char *Left(char *p_Src, int p_Len, char *io_Dst);
char *Right(char *p_Src, int p_Len, char *io_Dst);
char *LCase(char *p_Src, char *io_Dst);
char *UCase(char *p_Src, char *io_Dst);
int StrScrape(char *inStr, char *outStr);
#ifndef WIN32
char tolower(char c);
char toupper(char c);
#endif
int isNumeric(char *val);
unsigned long Hex(char *hexval);