#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#include <signal.h>
#else
#include "Win32\getopt.h"
#endif
#include <stdlib.h>

#include "log.h"
#include "transcurl.h"
#include "liboddcast.h"
#include "transcode.h"

extern transcodeGlobals	globals;
int	gIsRunning = 1;

void setStopTranscoder(int flag)
{
	globals.gStopFlag = flag;
}

int transcode_main()
{
	char	*serverURL = NULL;

	globals.gStopFlag = 0;
	if (globals.mainStatusCallback) {
		globals.mainStatusCallback((void *)"Connecting To Output Stream");
	}

//	if (!connectToServer()) {
//		if (globals.mainStatusCallback) {
//			globals.mainStatusCallback((void *)"Failed Connecting To Output Stream");
//		}
//		if (globals.outputStatusCallback) {
//			//globals.outputStatusCallback((void *)"Failed");
//		}
//		return(0);
//	}
//	if (globals.outputStatusCallback) {
//		globals.outputStatusCallback((void *)"Connected");
//	}
	if (serverURL == NULL) {
		serverURL = getSourceURL();
	}

	retrieveURL(serverURL);

	return 1;
}

