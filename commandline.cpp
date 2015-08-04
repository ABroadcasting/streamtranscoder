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

transcodeGlobals	globals;

void signalHandler(int c) {
	        exit(1);
}

void mainStatusCallback(void *pValue) {
	// pValue is a string
	LogMessage(LOG_INFO, "Main Status : (%s)", (char *)pValue);
}
void inputStatusCallback(void *pValue) {
	// pValue is a string
	LogMessage(LOG_INFO, "Input Status : (%s)", (char *)pValue);
}
void readBytesCallback(void *pValue) {
	// pValue is a long
	//LogMessage(LOG_INFO, "We just read : (%d) bytes", (long)pValue);
}
void inputServerTypeCallback(void *pValue) {
	// pValue is a string
	LogMessage(LOG_INFO, "Input Server Type : (%s)", (char *)pValue);
}
void inputStreamTypeCallback(void *pValue) {
	// pValue is a string
	LogMessage(LOG_INFO, "Input Stream Type : (%s)", (char *)pValue);
}
void inputStreamURLCallback(void *pValue) {
	// pValue is a string
	LogMessage(LOG_INFO, "Input Stream URL : (%s)", (char *)pValue);
}
void inputStreamNameCallback(void *pValue) {
	// pValue is a string
	LogMessage(LOG_INFO, "Input Stream Name : (%s)", (char *)pValue);
}
void inputMetadataCallback(void *pValue) {
	// pValue is a string
	LogMessage(LOG_INFO, "Metadata : (%s)", (char *)pValue);
}
void inputBitrateCallback(void *pValue) {
	// pValue is a string
	LogMessage(LOG_INFO, "Input Bitrate : (%s)", (char *)pValue);
}
void outputStatusCallback(void *pValue) {
	// pValue is a string
	LogMessage(LOG_INFO, "Output status : (%s)", (char *)pValue);
}
void writeBytesCallback(void *pValue) {
	// pValue is a long
	//LogMessage(LOG_INFO, "We just wrote : (%d) bytes", (char *)pValue);
}
void outputServerTypeCallback(void *pValue) {
	// pValue is a string
	LogMessage(LOG_INFO, "Output Server Type : (%s)", (char *)pValue);
}
void outputStreamTypeCallback(void *pValue) {
	// pValue is a string
	LogMessage(LOG_INFO, "Output Stream Type : (%s)", (char *)pValue);
}
void outputBitrateCallback(void *pValue) {
	// pValue is a string
	LogMessage(LOG_INFO, "Output Bitrate : (%s)", (char *)pValue);
}


void usage() {
	fprintf(stdout, "usage: transcode -b -e errorlevel -d -c config -s serverURL\n");
	fprintf(stdout, "where :\n");
	fprintf(stdout, "	-b = run in background\n");
	fprintf(stdout, "	-e = error logging level 1=ERROR, 2=INFO, 3=DEBUG\n");
	fprintf(stdout, "	-d = dump what's being sent to/from the servers\n");
	fprintf(stdout, "	-c = config file\n");
	fprintf(stdout, "	-s = source server (in http://server:port form)\n");
	
	exit(1);
}
static int	backgroundProcess = 0;

int transcode_init(int argc, char **argv)
{
	char	*serverURL = NULL;
	char	*configFile = NULL;
	char	*defaultConfigFile = "transcoder.cfg";
	int 	c = 0;
	int	printConfig = 0;
	
	memset(&globals, '\000', sizeof(globals));

	// Setup Callbacks for all the interaction
	globals.mainStatusCallback = mainStatusCallback;
	globals.inputStatusCallback = inputStatusCallback;
	globals.readBytesCallback = readBytesCallback;
	globals.inputServerTypeCallback = inputServerTypeCallback;
	globals.inputStreamTypeCallback = inputStreamTypeCallback;
	globals.inputStreamURLCallback = inputStreamURLCallback;
	globals.inputStreamNameCallback = inputStreamNameCallback;
	globals.inputMetadataCallback = inputMetadataCallback;
	globals.inputBitrateCallback = inputBitrateCallback;
	globals.outputStatusCallback = outputStatusCallback;
	globals.writeBytesCallback = writeBytesCallback;
	globals.outputServerTypeCallback = outputServerTypeCallback;
	globals.outputStreamTypeCallback = outputStreamTypeCallback;
	globals.outputBitrateCallback = outputBitrateCallback;

	setServerStatusCallback(globals.outputStatusCallback);
	setGeneralStatusCallback(globals.mainStatusCallback);
	setWriteBytesCallback(globals.writeBytesCallback);
	setBitrateCallback(globals.outputBitrateCallback);
	setStreamTypeCallback(globals.outputStreamTypeCallback);
	setServerTypeCallback(globals.outputServerTypeCallback);
	setSourceURLCallback(globals.inputStreamURLCallback);
	setDestURLCallback(globals.outputStreamURLCallback);


#ifndef WIN32
	signal(SIGUSR1, signalHandler);
#endif
	while ((c = getopt(argc, argv, "hpc:e:db")) != -1) {
		switch (c) {
			case 'b':
				backgroundProcess = 1;
				break;
			case 'd':
				globals.gdumpData = 1;
				break;
			case 'c':
				configFile = optarg;
				break;
			case 'p':
				printConfig = 1;
				break;
			case 'e':
				setErrorType(atoi(optarg));
				break;
			case 'h':
				usage();
				break;
			default:
				usage();
		}
	}
	if (configFile == NULL) {
		configFile = defaultConfigFile;
	}
	setConfigFileName(configFile);
	readConfigFile();
	if (printConfig) {
		printConfigFileValues();
	}

	if (globals.gdumpData) {
		globals.outFromServer = fopen("fromServer.mp3", "wb");
		globals.outToEncoder = fopen("toEncoder.pcm", "wb");
		globals.outToServer = fopen("toServer.mp3", "wb");
	}

	
	return 1;
}

main(int argc, char **argv) {
	int processID = 0;
	transcode_init(argc, argv);
	if (backgroundProcess) {
		fprintf(stdout, "Starting transcoder\nDetaching from the console\n");
		if ((processID = (int)fork()) > 0) {
		/*
		*              * Parent process, just exit
		*                           */

			_exit(0);
		}
	}
	else {
		fprintf(stdout, "Press enter to begin");
		char tmp = getc(stdin);
	}
			
	return transcode_main();
}
