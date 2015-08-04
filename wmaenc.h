#ifdef WIN32
#ifndef __WMAENC_H__
#define __WMAENC_H__
#include <windows.h>

int startWMAServer(char *mes, char *error);
void stopWMAEncoding();
void updateWMATitle();
int doWMAencoding(void *buffer, DWORD length, DWORD user);
#endif
#endif
