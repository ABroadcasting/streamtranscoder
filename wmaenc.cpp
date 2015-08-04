#ifdef WIN32

#include <liboddcast.h>
#include <windows.h>
#include <stdio.h>
#include "basswma.h"
#include "bass.h"

HINSTANCE inst;
HWND win=NULL;

#define SAMPLERATE 44100

static HWMENCODE handle;	// encoder handle
float time;			// elapsed time
DWORD level;		// input level
extern oddcastGlobals g;
int startWMAServer(char *mes, char *error)
{
	char	buff[1024] = "";
	int		wmaBitrate = g.currentBitrate*1000;

	if (BASS_GetVersion()!=MAKELONG(1,7)) {
		sprintf(error, "BASS version 1.7 was not loaded, incorrect BASS.DLL");
		return 0;
	}

	
	int	valid = 0;

	DWORD flags = 0;

	if (g.currentChannels == 1) {
		flags = BASS_SAMPLE_MONO;
	}

	long *rates=(long *)BASS_WMA_EncodeGetRates(g.currentSamplerate,flags);
	if (!rates) {
		strcat(error, "No valid bitrates");
		;
	} else {
		char buf[20];
		sprintf(error, "Valid bitrates for %dHz: ", g.currentSamplerate);
		while (*rates) {
			sprintf(buf,"%dkbps,",(*rates)/1000);
			strcat(error, buf);
			if (wmaBitrate == *rates) {
				valid = 1;
			}
//			strcat(buff, buf);
			rates++;
		}
		error[strlen(error)-1] = '\000';
	}
	
	if (!valid) {
		sprintf(mes, "%d is not a valid bitrate for samplerate %d", wmaBitrate/1000, g.currentSamplerate);
		return 0;
	}

	if (g.currentChannels == 1) {
		flags = BASS_SAMPLE_MONO | BASS_WMA_ENCODE_TAGS | BASS_WMA_ENCODE_SCRIPT;
	}
	else {
		flags = BASS_WMA_ENCODE_TAGS | BASS_WMA_ENCODE_SCRIPT;
	}

	handle=BASS_WMA_EncodeOpenNetwork(g.currentSamplerate,flags,wmaBitrate,g.gWMAPort,g.gWMAClients);
	//handle = BASS_WMA_EncodeOpenNetwork(44100,0,128000,0,5);

	if (!handle) {
		char	buff2[1024] = "";

		sprintf(buff2,"Failed with bitrate %d, samplerate %d(error code: %d %d)",wmaBitrate/1000, g.currentSamplerate, BASS_ErrorGetCode(),BASS_WMA_ErrorGetCode());
		strcat(buff, buff2);
		strcpy(mes, buff);
		return 0;
	}
	
	BASS_WMA_EncodeSetTag(handle,"Title",g.gServDesc); // set WMA title tag
	BASS_WMA_EncodeSetTag(handle,"Artist",g.gSongTitle); // set WMA title tag
	BASS_WMA_EncodeSetTag(handle,"Caption",g.gSongTitle); // set WMA title tag
	BASS_WMA_EncodeSetTag(handle,"WM/PromotionURL",g.gServURL);

	BASS_WMA_EncodeSetTag(handle,0,0); // done setting tags
	time=0;
	return 1;
}

void updateWMATitle()
{
	BOOL ret = BASS_WMA_EncodeSetTag(handle,"Caption",g.gSongTitle); // set WMA title tag
	ret = BASS_WMA_EncodeSetTag(handle,"Artist",g.gSongTitle); // set WMA title tag
	ret = BASS_WMA_EncodeSetTag(handle,"Caption",g.gSongTitle); // set WMA title tag
	ret = BASS_WMA_EncodeSetTag(handle,"WM/PromotionURL",g.gServURL);

	
	if (ret == FALSE) {
		switch (BASS_WMA_ErrorGetCode()) {
		case BASS_ERROR_HANDLE:
			//MessageBox(NULL, "Invalid encoder handle","", MB_OK);
			break;
		case BASS_ERROR_NOTAVAIL:
			//MessageBox(NULL, "Mid-stream tags not available","", MB_OK);
			break;
		case BASS_ERROR_ILLPARAM:
			//MessageBox(NULL, "Tag/Text invalid","", MB_OK);
			break;
		case BASS_ERROR_UNKNOWN:
			//MessageBox(NULL, "Unknown error","", MB_OK);
			break;
		}
	}
	/*
	ret = BASS_WMA_EncodeSetTag(handle,0,0); // done setting tags
	if (ret == FALSE) {
		switch (BASS_WMA_ErrorGetCode()) {
		case BASS_ERROR_HANDLE:
			MessageBox(NULL, "Invalid encoder handle","", MB_OK);
			break;
		case BASS_ERROR_NOTAVAIL:
			MessageBox(NULL, "Mid-stream tags not available","", MB_OK);
			break;
		case BASS_ERROR_ILLPARAM:
			MessageBox(NULL, "Tag/Text invalid","", MB_OK);
			break;
		case BASS_ERROR_UNKNOWN:
			MessageBox(NULL, "Unknown error","", MB_OK);
			break;
		}
	}
	*/
	
}
void stopWMAEncoding()
{
	if (handle) {
		BASS_WMA_EncodeClose(handle);
	}
}

int doWMAencoding(void *buffer, DWORD length, DWORD user)
{
	static int counter = 0;

	if (counter > 500) {
		updateWMATitle();
		counter = 0;
	}
	counter++;
	// encode the sample data, and continue recording if successful
	return BASS_WMA_EncodeWrite(handle,buffer,length*2);
}

#endif