#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#include <signal.h>
#else
#include "Win32\getopt.h"
#endif
#include <stdlib.h>
#include <time.h>

#include "log.h"
#include "transcurl.h"
#include "liboddcast.h"
#include "transcode.h"
#include <curses.h>
#include <string.h>
#include "transcode_nui.cpp"
#include <pthread.h>


transcodeGlobals	globals;

void signalHandler(int c) {
	        exit(1);
}

void mainStatusCallback(void *pValue) {
	// pValue is a string
	setmainStatus((char *)pValue);
	refresh();
}
void inputStatusCallback(void *pValue) {
	// pValue is a string
	setinputStatus((char *)pValue);
	refresh();
}
void readBytesCallback(void *pValue) {
	// pValue is a long
	static	long	startTime = 0;	
	static	long	endTime = 0;	
	static	long	bytesReadInterval = 0;
	char	kBPSstr[25] = "";

	long	bytesRead = (long)pValue;


	if (startTime == 0) {
		startTime = time(&startTime);
		bytesReadInterval = 0;
	}
	bytesReadInterval += bytesRead;
	globals.totalBytesRead += bytesRead;
	endTime = time(&endTime);
	if ((endTime - startTime) > 5) {
		int bytespersec = bytesReadInterval/(endTime - startTime);
		long kBPS = (bytespersec * 8)/1000;
		sprintf(kBPSstr, "%d Kbps", kBPS);
		setinputBPSRead((char *)kBPSstr);
		startTime = 0;
	}

	sprintf(kBPSstr, "%d bytes", globals.totalBytesRead);
	setinputTotalRead((char *)kBPSstr);
	refresh();
}
void inputServerTypeCallback(void *pValue) {
	// pValue is a string
	setinputServerType((char *)pValue);
	refresh();
}
void inputStreamTypeCallback(void *pValue) {
	// pValue is a string
	setinputStreamType((char *)pValue);
	refresh();
}
void inputStreamURLCallback(void *pValue) {
	// pValue is a string
	setstreamURL((char *)pValue);
	refresh();
}
void inputStreamNameCallback(void *pValue) {
	// pValue is a string
	setstreamName((char *)pValue);
	refresh();
}
void inputMetadataCallback(void *pValue) {
	// pValue is a string
	setmetaData((char *)pValue);
	refresh();
}
void inputBitrateCallback(void *pValue) {
	// pValue is a string
	setinputBitrate((char *)pValue);
	refresh();
}
void outputStatusCallback(void *pValue) {
	// pValue is a string
	setoutputStatus((char *)pValue);
	refresh();
}
void writeBytesCallback(void *pValue) {
	// pValue is a long
	static	long	startTime = 0;	
	static	long	endTime = 0;	
	static	long	bytesWrittenInterval = 0;
	char	kBPSstr[25] = "";

	long	bytesWritten = (long)pValue;

	if (startTime == 0) {
		startTime = time(&startTime);
		bytesWrittenInterval = 0;
	}
	bytesWrittenInterval += bytesWritten;
	globals.totalBytesWritten += bytesWritten;
	endTime = time(&endTime);
	if ((endTime - startTime) > 5) {
		int bytespersec = bytesWrittenInterval/(endTime - startTime);
		long kBPS = (bytespersec * 8)/1000;
		sprintf(kBPSstr, "%d Kbps", kBPS);
		setoutputBPSRead((char *)kBPSstr);
		startTime = 0;
	}

	sprintf(kBPSstr, "%d bytes", globals.totalBytesWritten);
	setoutputTotalRead((char *)kBPSstr);
	refresh();
}
void outputServerTypeCallback(void *pValue) {
	// pValue is a string
	setoutputServerType((char *)pValue);
	refresh();
}
void outputStreamTypeCallback(void *pValue) {
	// pValue is a string
	setoutputStreamType((char *)pValue);
	refresh();
}
void outputBitrateCallback(void *pValue) {
	// pValue is a string
	setoutputBitrate((char *)pValue);
	refresh();
}
void outputStreamURLCallback(void *pValue) {
	// pValue is a string
	setoutputURL((char *)pValue);
	refresh();
}


void usage() {
	fprintf(stdout, "usage: transcode -e errorlevel -d -c config -s serverURL\n");
	fprintf(stdout, "where :\n");
	fprintf(stdout, "	-e = error logging level 1=ERROR, 2=INFO, 3=DEBUG\n");
	fprintf(stdout, "	-d = dump what's being sent to/from the servers\n");
	fprintf(stdout, "	-c = config file\n");
	fprintf(stdout, "	-s = source server (in http://server:port form)\n");
	
	exit(1);
}

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
	globals.inputStreamURLCallback = inputStreamURLCallback;
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
	globals.outputStreamURLCallback = outputStreamURLCallback;

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
	while ((c = getopt(argc, argv, "hpc:e:d")) != -1) {
		switch (c) {
			case 'd':
				globals.gdumpData = 1;
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
		if (getOggFlag()) {
			globals.outToServer = fopen("toServer.ogg", "wb");
		}
		else {
			globals.outToServer = fopen("toServer.mp3", "wb");
		}
		setDumpData(globals.gdumpData);
		setoutToServer(globals.outToServer);
	}

	
	return 1;
}

void * startRunning(void *dummy) {
	mainStatusCallback((char *)"Starting!");
	refresh();
	int ret = transcode_main();
	pthread_exit((void *)1);
	return 0;
}
int main(int argc, char **argv)
{
	int ch;
	pthread_t	startThread = 0;
	
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	displayStaticContent();
	transcode_init(argc, argv);

	inputStatusCallback((char *)"Disconnected");
	mainStatusCallback((char *)"Initialized and Ready to Start");
	refresh();
	while (1) {
		int ch = getch();
		switch (ch) {
			case 'c': pthread_create(&startThread, NULL, &startRunning, NULL);
				  break;
			case 'x': system("clear");
				  exit(1);
		}
		refresh();
	}
	endwin();
	return 0;
}




