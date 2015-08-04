#ifndef __TRANSCODE_H__
#define __TRANSCODE_H__

#include <stdio.h>

typedef struct transcodeGlobalsTag {
	int		gdumpData;
	int		gStopFlag;
	FILE	*outFromServer;
	FILE	*outToEncoder;
	FILE	*outToServer;
	FILE	*outFile;
	FILE	*outFileRaw;
	void (*mainStatusCallback)(void *);
	void (*inputStatusCallback)(void *);
	void (*readBytesCallback)(void *);
	void (*inputServerTypeCallback)(void *);
	void (*inputStreamTypeCallback)(void *);
	void (*inputStreamURLCallback)(void *);
	void (*inputStreamNameCallback)(void *);
	void (*inputMetadataCallback)(void *);
	void (*inputBitrateCallback)(void *);
	void (*outputStatusCallback)(void *);
	void (*writeBytesCallback)(void *);
	void (*outputStreamURLCallback)(void *);
	void (*outputServerTypeCallback)(void *);
	void (*outputStreamTypeCallback)(void *);
	void (*outputBitrateCallback)(void *);
	long	totalBytesRead;
	long	totalBytesWritten;
} transcodeGlobals;

void setStopTranscoder(int flag);
int transcode_init(int argc, char **argv);
int transcode_main();

#endif

