#ifndef __TRANSCURL_H__
#define __TRANSCURL_H__

#include "mp3decoder.h"
#include "oggvdecoder.h"

#ifdef _DMALLOC_
#include <dmalloc.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
int retrieveURL(char *url);
#ifdef __cplusplus
}
#endif

#define READ_PIPE 0
#define WRITE_PIPE 1

typedef struct tagGlobalStruct {
	MP3_DECODE_ST	mp3dec;
	OGGV_DECODE_ST	oggvdec;
} transcoderGlobalStruct;

void parseIcecastHeader(char *http_header);
void parseShoutcastHeader(char *http_header);
void	reset_transcoder();

#endif
