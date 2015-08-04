#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <time.h>
#include <vorbis/vorbisenc.h>
#include "liboddcast.h"
#include "log.h"
#include "Socket.h"
#ifdef WIN32
#include <wmaenc.h>
#include <bass.h>
#else
#include <lame/lame.h>
#endif

#ifndef LAME_MAXMP3BUFFER   
#define LAME_MAXMP3BUFFER   16384
#endif

oddcastGlobals g;

typedef struct tagConfigFileValue {
	char	Variable[256];
	char	Value[256];
	char	Description[1024];
} configFileValue;

static configFileValue	configFileValues[100];
static int	numConfigValues = 0;

static int  greconnectFlag = 0;
int getReconnectFlag() {
	return g.gAutoReconnect;
}
int getReconnectSecs() {
	return g.gReconnectSec;
}

void	setDumpData(int dump) {
	g.gdumpData = dump;
}
int getLiveRecordingSetFlag() {
	return g.gLiveRecordingFlag;
}

bool getLiveRecordingFlag() {
	return g.areLiveRecording;
}
void setLiveRecordingFlag(bool flag) {
	g.areLiveRecording = flag;
}
int getLiveInSamplerate() {
	return g.gLiveInSamplerate;
}
void setLiveInSamplerate(int rate) {
	g.gLiveInSamplerate = rate;
}
int	getOggFlag() {
	return g.gOggFlag;
}
int	getWMAFlag() {
	return g.gWMAFlag;
}
char*	getServerDesc() {
	return g.gServDesc;
}
char*	getSourceURL() {

	return g.gSourceURL;
	
}
int getIsConnected() {
	return g.weareconnected;
}
long	getCurrentSamplerate() {
	return g.currentSamplerate;
}
int	getCurrentBitrate() {
	return g.currentBitrate;
}
int	getCurrentChannels() {
	return g.currentChannels;
}
void setoutToServer(FILE *filep) {
	g.outToServer = filep;
}
void setSourceDescription(char *desc) {
	strcpy(g.sourceDescription, desc);
//	initializeencoder();
}

char *getCurrentRecordingName() {
	return (g.gCurrentRecordingName);
}
void setCurrentRecordingName(char *name) {
	strcpy(g.gCurrentRecordingName, name);
}
int resetResampler() {
	if (g.initializedResampler) {
		res_clear(&(g.resampler));
	}
	g.initializedResampler = 0;
	return 1;
}
/* Gratuitously ripped from util.c
*/
static char base64table[64] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};
 
static signed char base64decode[256] = {
     -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
     -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
     -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
     52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -1, -2, -2,
     -2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
     15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
     -2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
     41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
     -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
     -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
     -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
     -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
     -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
     -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
     -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
     -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
};
/* This isn't efficient, but it doesn't need to be */
char *util_base64_encode(char *data)
{
    int len = strlen(data);
    char *out = (char *)malloc(len*4/3 + 4);
    char *result = out;
    int chunk;

    while(len > 0) {
        chunk = (len >3)?3:len;
        *out++ = base64table[(*data & 0xFC)>>2];
        *out++ = base64table[((*data & 0x03)<<4) | ((*(data+1) & 0xF0) >> 4)];
        switch(chunk) {
            case 3:
                *out++ = base64table[((*(data+1) & 0x0F)<<2) | ((*(data+2) & 0xC0)>>6)];
                *out++ = base64table[(*(data+2)) & 0x3F];
                break;
            case 2:
                *out++ = base64table[((*(data+1) & 0x0F)<<2)];
                *out++ = '=';
                break;
            case 1:
                *out++ = '=';
                *out++ = '=';
                break;
        }
        data += chunk;
        len -= chunk;
    }
    *out = 0;

    return result;
}

char *util_base64_decode(unsigned char *input)
{
    int len = strlen((char *)input);
    char *out = (char *)malloc(len*3/4 + 5);
    char *result = out;
    signed char vals[4];

    while(len > 0) {
        if(len < 4)
        {
            free(result);
            return NULL; /* Invalid Base64 data */
        }

        vals[0] = base64decode[*input++];
        vals[1] = base64decode[*input++];
        vals[2] = base64decode[*input++];
        vals[3] = base64decode[*input++];

        if(vals[0] < 0 || vals[1] < 0 || vals[2] < -1 || vals[3] < -1) {
            continue;
        }

        *out++ = vals[0]<<2 | vals[1]>>4;
        if(vals[2] >= 0)
            *out++ = ((vals[1]&0x0F)<<4) | (vals[2]>>2);
        else
            *out++ = 0;

        if(vals[3] >= 0)
            *out++ = ((vals[2]&0x03)<<6) | (vals[3]);
        else
            *out++ = 0;

        len -= 4;
    }
    *out = 0;

    return result;
}

int readConfigFile() {
	FILE	*filep;
	char	buffer[1024];
	char	defaultConfigName[] = "transcoder.cfg";

	numConfigValues = 0;
	memset(&configFileValues, '\000', sizeof(configFileValues));

	filep = fopen(g.gConfigFileName, "r");
	if (filep == 0) {
		LogMessage(LOG_ERROR, "Cannot open config file %s - we will create one (%s)\n", g.gConfigFileName, defaultConfigName);
		strcpy(g.gConfigFileName, defaultConfigName);
	}
	else {
		while (!feof(filep)) {
			memset(buffer, '\000', sizeof(buffer));
			fgets(buffer, sizeof(buffer)-1, filep);
			buffer[strlen(buffer)-1] = '\000'; // trim off the newline
			if (buffer[0] != '#') {
				char	*p1 = strchr(buffer, '=');
				if (p1) {
					strncpy(configFileValues[numConfigValues].Variable, buffer, p1-buffer);
					p1++; // Get past the =
					strcpy(configFileValues[numConfigValues].Value, p1);
					numConfigValues++;
				}
			}	
		}
			
		if (filep) {
			fclose(filep);
		}	
	}
	config_read();

	writeConfigFile();

	return 1;
}

void setConfigFileName(char *configFile) {
	strcpy(g.gConfigFileName, configFile);
}
char *getConfigFileName() {
	return g.gConfigFileName;
}

int writeConfigFile() {
	char	defaultConfigName[] = "transcoder.cfg";

	config_write();

	if (strlen(g.gConfigFileName) == 0) {
		strcpy(g.gConfigFileName, defaultConfigName);
	}
	FILE *filep = fopen(g.gConfigFileName, "w");
	if (filep == 0) {
		LogMessage(LOG_ERROR, "Cannot open config file %s\n", g.gConfigFileName);
		return 0;
	}
	for (int i=0;i<numConfigValues;i++) {
		fprintf(filep, "#\n#%s\n#\n", configFileValues[i].Description);
		fprintf(filep, "%s=%s\n\n", configFileValues[i].Variable, configFileValues[i].Value);	
	}
	fclose(filep);

	return 1;
}
void	printConfigFileValues() {

	for (int i=0;i<numConfigValues;i++) {
		LogMessage(LOG_DEBUG, "(%s) = (%s)\n", configFileValues[i].Variable, configFileValues[i].Value);	
	}
}

void GetConfigVariable(char *appName, char *paramName, char *defaultvalue, char *destValue, int destSize, char *initFile, char *desc) {

	for (int i=0;i<numConfigValues;i++) {
		if (!strcmp(paramName, configFileValues[i].Variable)) {
			strcpy(destValue, configFileValues[i].Value);
			strcpy(configFileValues[i].Description, desc);
			return;
		}
	}
	strcpy(configFileValues[numConfigValues].Variable, paramName);
	strcpy(configFileValues[numConfigValues].Value, defaultvalue);
	strcpy(configFileValues[numConfigValues].Description, desc);
	strcpy(destValue, configFileValues[numConfigValues].Value);
	numConfigValues++;
	return;
}
long GetConfigVariableLong(char *appName, char *paramName, long defaultvalue, char *initFile, char *desc) {
	char	buf[1024] = "";
	char	defaultbuf[1024] = "";

	sprintf(defaultbuf, "%d", defaultvalue);

	GetConfigVariable(appName, paramName, defaultbuf, buf, sizeof(buf), initFile, desc);

	return atol(buf);
}
void PutConfigVariable(char *appName, char *paramName, char *destValue, char *initFile) {

	for (int i=0;i<numConfigValues;i++) {
		if (!strcmp(paramName, configFileValues[i].Variable)) {
			strcpy(configFileValues[i].Value, destValue);
			return;
		}
	}
	strcpy(configFileValues[numConfigValues].Variable, paramName);
	strcpy(configFileValues[numConfigValues].Value, destValue);
	strcpy(configFileValues[numConfigValues].Description, "");
	numConfigValues++;
	return;
}
void PutConfigVariableLong(char *appName, char *paramName, long value, char *initFile) {
	char	buf[1024] = "";

	sprintf(buf, "%d", value);

	PutConfigVariable(appName, paramName, buf, initFile);

	return ;
}

void	restartConnection() {
	;
}

int trimVariable(char *variable) {
	char	*p1;
        // Trim off the back
        for (p1=variable + strlen(variable)-1;p1>variable;p1--) {
                if ((*p1 == ' ') || (*p1 == '	')) {
                        *p1 = '\000';
                }
                else {
                        break;
                }
        }
        // Trim off the front
        char    tempVariable[1024] = "";
        memset(tempVariable, '\000', sizeof(tempVariable));
        for (p1=variable;p1<variable + strlen(variable) - 1;p1++) {
                if ((*p1 == ' ') || (*p1 == '	')) {
                        ;
                }
                else {
                        break;
                }
        }
        strcpy(tempVariable, p1);
        strcpy(variable, tempVariable);
		return 1;
}

void setDestURLCallback(void (*pCallback)(void *)) {
	g.destURLCallback = pCallback;
}
void setSourceURLCallback(void (*pCallback)(void *)) {
	g.sourceURLCallback = pCallback;
}
void setServerStatusCallback(void (*pCallback)(void *)) {
	g.serverStatusCallback = pCallback;
}
void setGeneralStatusCallback(void (*pCallback)(void *)) {
	g.generalStatusCallback = pCallback;
}
void setWriteBytesCallback(void (*pCallback)(void *)) {
	g.writeBytesCallback = pCallback;
}
void setServerTypeCallback(void (*pCallback)(void *)) {
	g.serverTypeCallback = pCallback;
}
void setStreamTypeCallback(void (*pCallback)(void *)) {
	g.streamTypeCallback = pCallback;
}
void setBitrateCallback(void (*pCallback)(void *)) {
	g.bitrateCallback = pCallback;
}

void setOggEncoderText(char *text) {
	strcpy(g.gOggEncoderText, text);

}
void setVUCallback(void (*pCallback)(int, int)) {
	g.VUCallback = pCallback;
}
void setForceStop(int forceStop) {
	g.gForceStop = forceStop;
}

void initializeGlobals()
{
	// Global variables....gotta love em...
	g.gSCSocket = 0;
	g.gSCSocket2 = 0;
	g.gSCSocketControl = 0;
	g.gSCFlag = 0;
	g.gReconnectSec = 10;
	g.gAutoCountdown = 10;
	g.automaticconnect = 1;
	memset(g.gServer, '\000', sizeof(g.gServer));
	memset(g.gPort, '\000', sizeof(g.gPort));
	memset(g.gPassword, '\000', sizeof(g.gPassword));
	memset(g.gIniFile, '\000', sizeof(g.gIniFile));
	memset(g.gAppName, '\000', sizeof(g.gAppName));
	memset(g.gCurrentSong, '\000', sizeof(g.gCurrentSong));
	g.gPubServ = 0;
	memset(g.gServIRC, '\000', sizeof(g.gServIRC));
	memset(g.gServAIM, '\000', sizeof(g.gServAIM));
	memset(g.gServICQ, '\000', sizeof(g.gServICQ));
	memset(g.gServURL, '\000', sizeof(g.gServURL));
	memset(g.gServDesc, '\000', sizeof(g.gServDesc));
	memset(g.gMountpoint, '\000', sizeof(g.gMountpoint));
	g.gAutoReconnect = 0;
	memset(g.gAutoStart, '\000', sizeof(g.gAutoStart));
	memset(g.gAutoStartSec, '\000', sizeof(g.gAutoStartSec));
	memset(g.gQuality, '\000', sizeof(g.gQuality));
	g.gOggFlag = 0;
	memset(g.gIceFlag, '\000', sizeof(g.gIceFlag));
	g.gLAMEFlag = 0;
	g.gWMAFlag = 0;
	memset(g.gSaveDirectory, '\000', sizeof(g.gSaveDirectory));
	memset(g.gSaveDirectoryFlag, '\000', sizeof(g.gSaveDirectoryFlag));
	memset(g.gSaveAsWAV, '\000', sizeof(g.gSaveAsWAV));
	memset(g.gSongTitle, '\000', sizeof(g.gSongTitle));
	g.startTime = 0;
	g.endTime = 0;

	g.weareconnected = 0;

#ifndef WIN32
	g.gf;
#endif
	g.sampleData;
	g.gCurrentlyEncoding = 0;
	g.gShoutcastFlag = 0;
	g.gIcecastFlag = 0;
	g.gSaveWAV = 0;
	g.gSaveFile = 0;

	g.outToServer = 0;
	g.gdumpData = 0;
	g.outToEncoder = 0;

	g.destURLCallback = NULL;
	g.sourceURLCallback = NULL;
	g.serverStatusCallback = NULL;
	g.generalStatusCallback = NULL;
	g.writeBytesCallback = NULL;
	g.serverTypeCallback = NULL;
	g.streamTypeCallback = NULL;
	g.bitrateCallback = NULL;
	g.VUCallback = NULL;

	memset(g.sourceDescription, '\000', sizeof(g.sourceDescription));
	// OGG Stuff
	g.oggflag = 1;
	g.ice2songChange = false;
	g.in_header = 0;

	// Resampler stuff
	g.initializedResampler = 0;

	g.gLiveRecordingFlag = 0;
	g.areLiveRecording = FALSE;

	g.gWMAClients = 0;
	g.gWMAFlag = 0;
	g.gWMAPort = 0;
	memset(g.gWMAMode, '\000', sizeof(g.gWMAMode));
	memset(g.gOggEncoderText, '\000', sizeof(g.gOggEncoderText));
	g.gForceStop = 0;

	memset(&(g.vi), '\000', sizeof(g.vi));


}


int ice2_send_data(unsigned char *buff, unsigned long len)
{
	char *buffer;
	ogg_page og;


	buffer = ogg_sync_buffer(&g.oy_stream, len);
	memcpy(buffer, buff, len);
	ogg_sync_wrote(&g.oy_stream, len);

	while (ogg_sync_pageout(&g.oy_stream, &og) == 1) {
		
		int sentbytes = send(g.gSCSocket, (const char *)og.header, og.header_len, (int)0);
		sentbytes += send(g.gSCSocket, (const char *)og.body, og.body_len, (int)0);
		if (g.gdumpData) {
			fwrite(og.header, 1, og.header_len, g.outToServer);
			fwrite(og.body, 1, og.body_len, g.outToServer);
			fflush(g.outToServer);
		}

		if (g.writeBytesCallback) {
			g.writeBytesCallback((void *)sentbytes);
		}

		return sentbytes;
	}

	return 1;
}



int setCurrentSongTitle(char *song) {
	if (strcmp(g.gSongTitle, song)) {
		strcpy(g.gSongTitle, song);
		updateSongTitle();
		return 1;
	}
	return 0;
}

void getCurrentSongTitle(char *song, char *artist, char *full)
{
	char	songTitle[1024] = "";
	char	songTitle2[1024] = "";
	
	memset(songTitle2, '\000', sizeof(songTitle2));
	
	strcpy(songTitle, g.gSongTitle);

	strcpy(full, songTitle);

	char	*p1 = strchr(songTitle, '-');
	if (p1) {
		if (*(p1-1) == ' ') {
			p1--;
		}
		strncpy(artist, songTitle, p1-songTitle);
		p1 = strchr(songTitle, '-');
		p1++;
		if (*p1 == ' ') {
			p1++;
		}
		strcpy(song, p1);
	}
	else {
		strcpy(artist, "");
		strcpy(song, songTitle);
	}

}


void ReplaceString(char *source, char *dest, char *from, char *to)
{
	int loop = 1;
	char *p2 = (char *)1;
	char	*p1 = source;
	while (p2) {
		p2 = strstr(p1, from);
		if (p2) {
			strncat(dest, p1, p2-p1);
			strcat(dest, to);
			p1 = p2 + strlen(from);
		}
		else {
			strcat(dest, p1);
		}
	}
}
// This function URLencodes strings for use in sending them thru
// the Shoutcast admin.cgi interface to update song titles..
void URLize(char *input, char *output, int inputlen, int outputlen)
{

	ReplaceString(input, output, "%", "%25");
	memset(input, '\000', inputlen);
	ReplaceString(output, input, ";", "%3B");
	memset(output, '\000', outputlen);
	ReplaceString(input, output, "/", "%2F");
	memset(input, '\000', inputlen);
	ReplaceString(output, input, "?", "%3F");
	memset(output, '\000', outputlen);
	ReplaceString(input, output, ":", "%3A");
	memset(input, '\000', inputlen);
	ReplaceString(output, input, "@", "%40");
	memset(output, '\000', outputlen);
	ReplaceString(input, output, "&", "%26");
	memset(input, '\000', inputlen);
	ReplaceString(output, input, "=", "%3D");
	memset(output, '\000', outputlen);
	ReplaceString(input, output, "+", "%2B");
	memset(input, '\000', inputlen);
	ReplaceString(output, input, " ", "%20");
	memset(output, '\000', outputlen);
	ReplaceString(input, output, "\"", "%22");
	memset(input, '\000', inputlen);
	ReplaceString(output, input, "#", "%23");
	memset(output, '\000', outputlen);
	ReplaceString(input, output, "<", "%3C");
	memset(input, '\000', inputlen);
	ReplaceString(output, input, ">", "%3E");
	memset(output, '\000', outputlen);
	ReplaceString(input, output, "!", "%21");
	memset(input, '\000', inputlen);
	ReplaceString(output, input, "*", "%2A");
	memset(output, '\000', outputlen);
	ReplaceString(input, output, "'", "%27");
	memset(input, '\000', inputlen);
	ReplaceString(output, input, "(", "%28");
	memset(output, '\000', outputlen);
	ReplaceString(input, output, ")", "%29");
	memset(input, '\000', inputlen);
	ReplaceString(output, input, ",", "%2C");

	memset(output, '\000', outputlen);
	strcpy(output, input);

}


void updateSongTitle()
{
	char	contentString[2056] = "";
	char	URLPassword[255] = "";
	char	URLSong[1024] = "";
	char	Song[1024] = "";

	if (getIsConnected()) {
		if (!g.gOggFlag) {
			if ((g.gSCFlag) || (g.gIcecastFlag)) {
				URLize(g.gPassword, URLPassword, sizeof(g.gPassword), sizeof(URLPassword));

				strcpy(g.gCurrentSong, g.gSongTitle);

				URLize(g.gSongTitle, URLSong, sizeof(g.gSongTitle), sizeof(URLSong));

				if (g.gIcecastFlag) {
					sprintf(contentString, "GET /admin.cgi?pass=%s&mode=updinfo&mount=%s&song=%s HTTP/1.0\r\nUser-Agent: (Mozilla Compatible)\r\n\r\n", URLPassword,g.gMountpoint,URLSong);
				}
				else {
					sprintf(contentString, "GET /admin.cgi?pass=%s&mode=updinfo&song=%s HTTP/1.0\r\nUser-Agent: (Mozilla Compatible)\r\n\r\n", URLPassword,URLSong);
				}
				g.gSCSocketControl = g.controlChannel.DoSocketConnect(g.gServer, atoi(g.gPort));
				if (g.gSCSocketControl != -1) {
					int sent = send(g.gSCSocketControl, contentString, strlen(contentString), (int)0);
					closesocket(g.gSCSocketControl);
				}
				else {
					LogMessage(LOG_ERROR, "Cannot connect to server");
				}
			}
			if (g.gWMAFlag) {
				;
	#ifdef WIN32
				updateWMATitle();
	#endif
			}
		}
		else {
				g.ice2songChange = true;
		}
	}

}

void doUpdateSong()
{
	updateSongTitle();
}



// This function does some magic in order to change the metadata
// in a vorbis stream....Vakor helped me with this, and it's pretty
// much all his idea anyway...and probably the reason why it actually 
// does work..:)
void icecast2SendMetadata()
{

	
    ogg_packet op;
	
    vorbis_analysis_wrote(&g.vd, 0);

    while(vorbis_analysis_blockout(&g.vd, &g.vb)==1)
    {
	vorbis_analysis(&g.vb, NULL);
	vorbis_bitrate_addblock(&g.vb);
        //    vorbis_analysis(&g.vb, &op);
         //   ogg_stream_packetin(&g.os, &op);
    }

	ogg_page ogp;

	while (ogg_stream_pageout(&g.os, &ogp) > 0) {

			int sentbytes = ice2_send_data(ogp.header, ogp.header_len);
			sentbytes += ice2_send_data(ogp.body, ogp.body_len);

			if (g.gSaveFile) {
				if (!g.gSaveAsWAV) {
					int ret = fwrite(ogp.header, ogp.header_len, 1, g.gSaveFile);
					ret += fwrite(ogp.body, ogp.body_len, 1, g.gSaveFile);
				}
			}
	}

	/*
	ogg_stream_clear(&g.os);
	vorbis_block_clear(&g.vb);
	vorbis_dsp_clear(&g.vd);
	vorbis_info_clear(&g.vi);
*/
	initializeencoder();
}


// This function will disconnect the DSP from the server (duh)
int disconnectFromServer()
{

	g.weareconnected = 0;


	if (g.serverStatusCallback) {
		g.serverStatusCallback((char *)"Disconnecting");
	}

	if (g.gWMAFlag) {
#ifdef WIN32
		stopWMAEncoding();
#endif
	}

	if (g.gCurrentlyEncoding) {
#ifdef WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}

	// Close all open sockets
	closesocket(g.gSCSocket);
	closesocket(g.gSCSocketControl);

	// Reset the Status to Disconnected, and reenable the config
	// button

	g.gSCSocket = 0;
	g.gSCSocketControl = 0;


	ogg_stream_clear(&g.os);
	vorbis_block_clear(&g.vb);
	vorbis_dsp_clear(&g.vd);
	vorbis_info_clear(&g.vi);
	memset(&(g.vi), '\000', sizeof(g.vi));

	if (g.serverStatusCallback) {
		g.serverStatusCallback((void *)"Disconnected");
	}

	return 1;
}

// This funciton will connect to a server (Shoutcast/Icecast/Icecast2)
// and send the appropriate password info and check to make sure things
// are connected....
int connectToServer()
{
	int			s_socket = 0;
	char		buffer[1024] = "";
	char	contentString[1024] = "";

	char	brate[25] = "";
	sprintf(brate, "%d", g.currentBitrate);

	g.gSCFlag = 0;

	greconnectFlag = 0;

	if (g.serverStatusCallback) {
		g.serverStatusCallback((void *)"Connecting");
	}

#ifdef WIN32
	if (g.gWMAFlag) {

		char	message[2056] = "";
		if (g.serverStatusCallback) {
			g.serverStatusCallback((void *)"WMA encoding session started");
		}
		g.weareconnected = 1;
		return 1;
	}
#endif
#ifdef WIN32
	g.dataChannel.initWinsockLib();
#endif

	// If we are Icecast/Icecast2, then connect to specified port
	if (g.gIcecastFlag || g.gIcecast2Flag) {
		g.gSCSocket = g.dataChannel.DoSocketConnect(g.gServer, atoi(g.gPort));
	}
	else {
		// If we are Shoutcast, then the control socket (used for password)
		// is port+1.
		g.gSCSocket = g.dataChannel.DoSocketConnect(g.gServer, atoi(g.gPort)+1);
	}

	// Check to see if we connected okay
	if (g.gSCSocket == -1) {
		if (g.serverStatusCallback) {
			g.serverStatusCallback((void *)"Unable to connect to socket");
		}
		return 0;
	}

	int pswdok = 1;
	// Yup, we did.
	if (g.serverStatusCallback) {
		g.serverStatusCallback((void *)"Socket connected");
	}

	// Here are all the variations of sending the password to 
	// a server..This if statement really is ugly...must fix.
	if (g.gIcecastFlag || g.gIcecast2Flag) {
		// The Icecast/Icecast2 Way
		if (g.gIcecastFlag) {
			sprintf(contentString, "SOURCE %s %s\ncontent-type: audio/mpeg\nx-audiocast-name: %s\nx-audiocast-url: %s\nx-audiocast-genre: %s\nx-audiocast-bitrate: %s\nx-audiocast-public: %d\nx-audiocast-description: %s\n\n", g.gPassword,g.gMountpoint, g.gServDesc, g.gServURL, g.gServGenre, brate, g.gPubServ, g.gServDesc);
		}
		if (g.gIcecast2Flag) {
			char	contentType[255] = "";
			if (g.gOggFlag) {
				strcpy(contentType, "application/x-ogg");
			}
			else {
				strcpy(contentType, "audio/mpeg");
			}
			char userAuth[1024] = "";
			sprintf(userAuth, "source:%s", g.gPassword);
			char *puserAuthbase64 = util_base64_encode(userAuth);
			sprintf(contentString, "SOURCE %s ICE/1.0\ncontent-type: %s\nAuthorization: Basic %s\nicy-password: %s\nice-name: %s\nice-url: %s\nice-genre: %s\nice-bitrate: %s\nice-public: %d\nice-description: %s\n\n", g.gMountpoint, contentType, puserAuthbase64, g.gPassword, g.gServDesc, g.gServURL, g.gServGenre, brate, g.gPubServ, g.gServDesc);
			free(puserAuthbase64);
		}
	}
	else {
		// The Shoutcast way
			send(g.gSCSocket, g.gPassword, strlen(g.gPassword), (int)0);
			send(g.gSCSocket, "\r\n", strlen("\r\n"), (int)0);

			recv(g.gSCSocket, buffer, sizeof(buffer), (int)0);
			// if we get an OK, then we are not a Shoutcast server
			// (could be live365 or other variant)..And OK2 means it's 
			// Shoutcast and we can safely send in metadata via the
			// admin.cgi interface.
			if (!strncmp(buffer, "OK", strlen("OK"))) {
				if (!strncmp(buffer, "OK2", strlen("OK2"))) {
					g.gSCFlag = 1;
				}
				else {
					g.gSCFlag = 0;
				}
				if (g.serverStatusCallback) {
					g.serverStatusCallback((void *)"Password OK");
				}
			}
			else {
				if (g.serverStatusCallback) {
					g.serverStatusCallback((void *)"Password Failed");
				}
				closesocket(g.gSCSocket);
				return 0;
			}
			
			memset(contentString, '\000', sizeof(contentString));
			if (strlen(g.gServICQ) == 0) {
				strcpy(g.gServICQ, "N/A");
			}
			if (strlen(g.gServAIM) == 0) {
				strcpy(g.gServAIM, "N/A");
			}
			if (strlen(g.gServIRC) == 0) {
				strcpy(g.gServIRC, "N/A");
			}
			sprintf(contentString, "icy-name:%s\r\nicy-genre:%s\r\nicy-url:%s\r\nicy-pub:%d\r\nicy-irc:%s\r\nicy-icq:%s\r\nicy-aim:%s\r\nicy-br:%s\r\n\r\n", g.gServDesc,g.gServGenre,g.gServURL, g.gPubServ, g.gServIRC, g.gServICQ, g.gServAIM, brate);
	}

	send(g.gSCSocket, contentString, strlen(contentString), (int)0);


	
	send(g.gSCSocket, contentString, strlen(contentString), (int)0);
	if (g.gIcecastFlag) {
		// Here we are checking the response from Icecast/Icecast2
		// from when we sent in the password...OK means we are good..if the
		// password is bad, Icecast just disconnects the socket.
		if (g.gOggFlag) {
			recv(g.gSCSocket, buffer, sizeof(buffer), 0);
			if (!strncmp(buffer, "OK", strlen("OK"))) {
				// I don't think this check is needed..
				if (!strncmp(buffer, "OK2", strlen("OK2"))) {
					g.gSCFlag = 1;
				}
				else {
					g.gSCFlag = 0;
				}
				if (g.serverStatusCallback) {
					g.serverStatusCallback((void *)"Password OK");
				}
			}
			else {
				if (g.serverStatusCallback) {
					g.serverStatusCallback((void *)"Password Failed");
				}
				closesocket(g.gSCSocket);
				return 0;
			}
		}
	}

	/*
	if (g.gSCFlag) {
		sprintf(contentString, "GET /index.html HTTP/1.0\r\nUser-Agent: (Mozilla Compatible)\r\n\r\n");
		g.gSCSocketControl = g.controlChannel.DoSocketConnect(g.gServer, atoi(g.gPort));
		if (g.gSCSocketControl != -1) {
			send(g.gSCSocketControl, contentString, strlen(contentString), (int)0);
			closesocket(g.gSCSocketControl);
			updateSongTitle();
		}
	}
	*/
	
	/* We are connected */
	char	outFilename[1024] = "";
	char	outputFile[1024] = "";
	struct tm *newtime;
	time_t aclock;

	time( &aclock );
	newtime = localtime( &aclock ); 

	int ret = 0;


	if (!g.gOggFlag) {
		ret = initializeencoder();
	}
	else {
		ret = 1;
	}
	if (ret) {
		g.weareconnected = 1;
		g.automaticconnect = 1;


		if (g.serverStatusCallback) {
			g.serverStatusCallback((void *)"Success");
		}
		// Start up song title check 
	}
	else {
		disconnectFromServer();
		if (g.serverStatusCallback) {
#ifdef WIN32
			if (g.gLAMEFlag) {
				g.serverStatusCallback((void *)"error with lame_enc.dll");
			}
			else { 
				g.serverStatusCallback((void *)"Encoder init failed");
			}
#else
			g.serverStatusCallback((void *)"Encoder init failed");
#endif
		}
		return 0;
	}
	if (g.serverStatusCallback) {
		g.serverStatusCallback((void *)"Connected");
	}
	return 1;
}



// These are some ogg routines that are used for Icecast2
int ogg_encode_dataout()
{
        ogg_packet op;
	ogg_page og;
		
        int result;
		int sentbytes = 0;


        if(g.in_header) {
                result = ogg_stream_flush(&g.os, &og);
                g.in_header = 0;
	}
	
	while(vorbis_analysis_blockout(&g.vd, &g.vb)==1)
	{
		vorbis_analysis(&g.vb, NULL);
		vorbis_bitrate_addblock(&g.vb);

		int packetsdone = 0;
		while(vorbis_bitrate_flushpacket(&g.vd, &op))
		{
			/* Add packet to bitstream */
			ogg_stream_packetin(&g.os,&op);
			packetsdone++;

			/* If we've gone over a page boundary, we can do actual output,
			   so do so (for however many pages are available) */
							int eos = 0;
			while(!eos)
			{
				int result = ogg_stream_pageout(&g.os,&og);
				if(!result) break;

				int ret = 0;
				sentbytes = ice2_send_data(og.header, og.header_len);
				sentbytes += ice2_send_data(og.body, og.body_len);

				if(ogg_page_eos(&og))
					eos = 1;
			}
		}

	}
	return sentbytes;
		
}

void oddsock_error_handler_function(const char *format, va_list ap) {
      return;
}


int initializeResampler(long inSampleRate, long inNCH)
{
	if (!g.initializedResampler) {
		long    in_samplerate = inSampleRate;
		long    out_samplerate = getCurrentSamplerate();
		long    in_nch = inNCH;
		long    out_nch = getCurrentChannels();


		if (res_init(&(g.resampler), out_nch, out_samplerate, in_samplerate, RES_END)) {
			LogMessage(LOG_ERROR, "Error initializing resampler");
			return 0;
		}
		g.initializedResampler = 1;
	}
	return 1;

}

int  ocConvertAudio(float *in_samples, float *out_samples, int num_in_samples, int num_out_samples) {
	int max_num_samples = res_push_max_input(&(g.resampler), num_out_samples);

	int ret_samples = res_push_interleaved(&(g.resampler), (SAMPLE *)out_samples, (const SAMPLE *)in_samples, max_num_samples);
	if (ret_samples <= 0) {
		LogMessage(LOG_ERROR, "Whoop, got no output samples!");
	}
	return ret_samples;

}

int initializeencoder()
{
	int	ret = 0;
	char	outFilename[1024] = "";
	char	message[1024] = "";

	resetResampler();

	if (g.gLAMEFlag) {

#ifdef WIN32
		BE_ERR		err				=0;
		BE_VERSION	Version			={0,};
		BE_CONFIG	beConfig		={0,};


		
		g.hDLL = LoadLibrary("lame_enc.dll");
	
		if(g.hDLL == NULL) {
			sprintf(message, "Unable to load DLL (lame_enc.dll)\nYou have selected encoding with LAME, but apparently the plugin cannot find LAME installed. Due to legal issues, \
oddcast cannot distribute LAME directly, and so you'll have to download it \
separately. You will need to put the LAME DLL (lame_enc.dll) into the same directory as the application in order to get it working. To download the LAME DLL, check \
out http://www.mp3dev.org/mp3/");
			LogMessage(LOG_ERROR, message);
			if (g.serverStatusCallback) {
				g.serverStatusCallback((void *)"can't find lame_enc.dll");
			}
			return 0;
		}

		// Get Interface functions from the DLL
		g.beInitStream	= (BEINITSTREAM) GetProcAddress(g.hDLL, TEXT_BEINITSTREAM);
		g.beEncodeChunk	= (BEENCODECHUNK) GetProcAddress(g.hDLL, TEXT_BEENCODECHUNK);
		g.beDeinitStream	= (BEDEINITSTREAM) GetProcAddress(g.hDLL, TEXT_BEDEINITSTREAM);
		g.beCloseStream	= (BECLOSESTREAM) GetProcAddress(g.hDLL, TEXT_BECLOSESTREAM);
		g.beVersion		= (BEVERSION) GetProcAddress(g.hDLL, TEXT_BEVERSION);
		g.beWriteVBRHeader= (BEWRITEVBRHEADER) GetProcAddress(g.hDLL,TEXT_BEWRITEVBRHEADER);

		if(!g.beInitStream || !g.beEncodeChunk || !g.beDeinitStream || !g.beCloseStream || !g.beVersion || !g.beWriteVBRHeader)	{
			sprintf(message, "Unable to get LAME interfaces - This DLL (lame_enc.dll) doesn't appear to be LAME?!?!?");
			LogMessage(LOG_ERROR, message);
			return 0;
		}

		// Get the version number
		g.beVersion( &Version );

		if (Version.byMajorVersion < 3) {
			sprintf(message, "This version of oddcast expects at least version 3.91 of the LAME DLL, the DLL found is at %u.%02u, please consider upgrading", Version.byDLLMajorVersion, Version.byDLLMinorVersion);
			LogMessage(LOG_ERROR, message);
		}
		else {
			if (Version.byMinorVersion < 91) {
				sprintf(message, "This version of oddcast expects at least version 3.91 of the LAME DLL, the DLL found is at %u.%02u, please consider upgrading", Version.byDLLMajorVersion, Version.byDLLMinorVersion);
				LogMessage(LOG_ERROR, message);
			}
		}
		// Check if all interfaces are present
		memset(&beConfig,0,sizeof(beConfig));					// clear all fields

		// use the LAME config structure
		beConfig.dwConfig = BE_CONFIG_LAME;

		if (g.currentChannels == 1) {
			beConfig.format.LHV1.nMode	= BE_MP3_MODE_MONO;
		}
		else {
			beConfig.format.LHV1.nMode	= BE_MP3_MODE_STEREO;
		}

		// this are the default settings for testcase.wav
		beConfig.format.LHV1.dwStructVersion	= 1;
		beConfig.format.LHV1.dwStructSize		= sizeof(beConfig);		
		beConfig.format.LHV1.dwSampleRate		= g.currentSamplerate;				// INPUT FREQUENCY
		beConfig.format.LHV1.dwReSampleRate		= g.currentSamplerate;					// DON"T RESAMPLE
//		beConfig.format.LHV1.dwReSampleRate		= 0;
		beConfig.format.LHV1.dwBitrate			= g.currentBitrate;					// MINIMUM BIT RATE
		if (g.gLAMEpreset > 0) {
			switch (g.gLAMEpreset) {
			case 0:
					beConfig.format.LHV1.nPreset = LQP_NOPRESET;
					break;
			case 1:
					beConfig.format.LHV1.nPreset = LQP_NORMAL_QUALITY;
					break;
			case 2:
					beConfig.format.LHV1.nPreset = LQP_LOW_QUALITY;
					break;
			case 3:
					beConfig.format.LHV1.nPreset = LQP_HIGH_QUALITY;
					break;
			case 4:
					beConfig.format.LHV1.nPreset = LQP_VOICE_QUALITY;
					break;
			case 5:
					beConfig.format.LHV1.nPreset = LQP_R3MIX;
					break;
			case 6:
					beConfig.format.LHV1.nPreset = LQP_VERYHIGH_QUALITY;
					break;
			case 7:
					beConfig.format.LHV1.nPreset = LQP_PHONE;
					break;
			case 8:
					beConfig.format.LHV1.nPreset = LQP_SW;
					break;
			case 9:
					beConfig.format.LHV1.nPreset = LQP_AM;
					break;
			case 10:
					beConfig.format.LHV1.nPreset = LQP_FM;
					break;
			case 11:
					beConfig.format.LHV1.nPreset = LQP_VOICE;
					break;
			case 12:
					beConfig.format.LHV1.nPreset = LQP_RADIO;
					break;
			case 13:
					beConfig.format.LHV1.nPreset = LQP_TAPE;
					break;
			case 14:
					beConfig.format.LHV1.nPreset = LQP_HIFI;
					break;
			case 15:
					beConfig.format.LHV1.nPreset = LQP_CD;
					break;
			case 16:
					beConfig.format.LHV1.nPreset = LQP_STUDIO;
					break;
			default:
					beConfig.format.LHV1.nPreset = LQP_NOPRESET;
					break;
			}
		}
		beConfig.format.LHV1.dwMpegVersion		= MPEG1;				// MPEG VERSION (I or II)
		beConfig.format.LHV1.dwPsyModel			= 0;					// USE DEFAULT PSYCHOACOUSTIC MODEL 
		beConfig.format.LHV1.dwEmphasis			= 0;					// NO EMPHASIS TURNED ON
		beConfig.format.LHV1.bWriteVBRHeader	= TRUE;					// YES, WRITE THE XING VBR HEADER


		if (g.gLAMEOptions.cbrflag) {
			beConfig.format.LHV1.bEnableVBR			= FALSE;
		}
		else {
			beConfig.format.LHV1.bEnableVBR			= TRUE;
			beConfig.format.LHV1.dwMaxBitrate = g.currentBitrateMax;
			beConfig.format.LHV1.dwBitrate			= g.currentBitrate;					// MINIMUM BIT RATE
		}
		beConfig.format.LHV1.nVBRQuality		= g.gLAMEOptions.quality;

		beConfig.format.LHV1.nVbrMethod			= VBR_METHOD_DEFAULT;
		if (!strcmp(g.gLAMEOptions.VBR_mode, "vbr_rh")) {
			beConfig.format.LHV1.nVbrMethod	= VBR_METHOD_OLD;
		}
		if (!strcmp(g.gLAMEOptions.VBR_mode, "vbr_mtrh")) {
			beConfig.format.LHV1.nVbrMethod	= VBR_METHOD_MTRH;
		}
		if (!strcmp(g.gLAMEOptions.VBR_mode, "vbr_abr")) {
			beConfig.format.LHV1.nVbrMethod	= VBR_METHOD_ABR;
		}

		beConfig.format.LHV1.bNoRes				= TRUE;					// No Bit resorvoir


		err = g.beInitStream(&beConfig, &(g.dwSamples), &(g.dwMP3Buffer), &(g.hbeStream));

		if(err != BE_ERR_SUCCESSFUL) {
			sprintf(message, "Error opening encoding stream (%lu)", err);
			LogMessage(LOG_ERROR, message);
			return 0;
		}
#else
		g.gf = lame_init();

		lame_set_errorf(g.gf,oddsock_error_handler_function);
		lame_set_debugf(g.gf,oddsock_error_handler_function);
		lame_set_msgf(g.gf,oddsock_error_handler_function);

		lame_set_brate(g.gf, g.currentBitrate);
		lame_set_quality(g.gf, g.gLAMEOptions.quality);
		
		if (g.currentChannels == 1) {
			lame_set_mode(g.gf, MONO);
			lame_set_num_channels(g.gf, 1);
		}
		else {
			lame_set_mode(g.gf, STEREO);
			lame_set_num_channels(g.gf, 2);
		}


		// Make the input sample rate the same as output..i.e. don't make lame do
		// any resampling...cause we are handling it ourselves...
		lame_set_in_samplerate(g.gf, g.currentSamplerate); 
		lame_set_out_samplerate(g.gf, g.currentSamplerate);
		lame_set_copyright(g.gf, g.gLAMEOptions.copywrite);
		lame_set_strict_ISO(g.gf, g.gLAMEOptions.strict_ISO);
		lame_set_disable_reservoir(g.gf, g.gLAMEOptions.disable_reservoir);

		if (!g.gLAMEOptions.cbrflag) {
			if (!strcmp(g.gLAMEOptions.VBR_mode, "vbr_rh")) {
				lame_set_VBR(g.gf, vbr_rh);
			}
			if (!strcmp(g.gLAMEOptions.VBR_mode, "vbr_mtrh")) {
				lame_set_VBR(g.gf, vbr_mtrh);
			}
			if (!strcmp(g.gLAMEOptions.VBR_mode, "vbr_abr")) {
				lame_set_VBR(g.gf, vbr_abr);
			}
			lame_set_VBR_mean_bitrate_kbps(g.gf, g.currentBitrate);
			lame_set_VBR_min_bitrate_kbps(g.gf, g.currentBitrateMin);
			lame_set_VBR_max_bitrate_kbps(g.gf, g.currentBitrateMax);
		}

		if (strlen(g.gLAMEbasicpreset) > 0) {
			if (!strcmp(g.gLAMEbasicpreset, "r3mix")) {
			//	presets_set_r3mix(g.gf, g.gLAMEbasicpreset, stdout);
			}
			else {
			//	presets_set_basic(g.gf, g.gLAMEbasicpreset, stdout);
			}
		}
		if (strlen(g.gLAMEaltpreset) > 0) {
			int altbitrate = atoi(g.gLAMEaltpreset);
		//	dm_presets(g.gf, 0, altbitrate, g.gLAMEaltpreset, "oddcast");
		}

		// do internal inits...
		lame_set_lowpassfreq(g.gf, g.gLAMEOptions.lowpassfreq);
		lame_set_highpassfreq(g.gf, g.gLAMEOptions.highpassfreq);
		
		lame_init_params(g.gf);
#endif
	}
	if (g.gOggFlag) {
		// Ogg Vorbis Initialization
		
			ogg_stream_clear(&g.os);
			vorbis_block_clear(&g.vb);
			vorbis_dsp_clear(&g.vd);
			vorbis_info_clear(&g.vi);

		int	bitrate = 0;
		
		vorbis_info_init(&g.vi);

		int encode_ret = 0;
		
		/*
		if (g.gOggBitQualFlag) {
			encode_ret = vorbis_encode_init(&g.vi,g.currentChannels,g.currentSamplerate, g.currentBitrateMax*1000, g.currentBitrate*1000, g.currentBitrateMin*1000);
		}
		else {
			encode_ret = vorbis_encode_init_vbr(&g.vi,g.currentChannels,g.currentSamplerate, atof(g.gOggQuality)*.1);
		}
		*/
		if(!g.gOggBitQualFlag) {
			encode_ret = vorbis_encode_setup_vbr(&g.vi, g.currentChannels, g.currentSamplerate, atof(g.gOggQuality)*.1);
			//encode_ret = vorbis_encode_init_vbr(&g.vi,g.currentChannels,g.currentSamplerate, atof(g.gOggQuality)*.1);
			if (encode_ret) {
				vorbis_info_clear(&g.vi);
			}
			else {
				if(g.currentBitrateMax > 0 || g.currentBitrateMin > 0) {
					struct ovectl_ratemanage_arg ai;
					vorbis_encode_ctl(&g.vi, OV_ECTL_RATEMANAGE_GET, &ai);

					ai.bitrate_hard_min=g.currentBitrateMin;
					ai.bitrate_hard_max=g.currentBitrateMax;
					ai.management_active=1;

					vorbis_encode_ctl(&g.vi, OV_ECTL_RATEMANAGE_SET, &ai);
				}
			}
		} else {
			int maxbit = -1;
			int minbit = -1;
			if (g.currentBitrateMax > 0) {
				maxbit = g.currentBitrateMax;
			}
			if (g.currentBitrateMin > 0) {
				minbit = g.currentBitrateMin;
			}

			encode_ret = vorbis_encode_setup_managed(&g.vi, g.currentChannels, g.currentSamplerate, 
								maxbit*1000, g.currentBitrate*1000, minbit*1000);

			if (encode_ret) {
				vorbis_info_clear(&g.vi);
			}
		}

		if (encode_ret == OV_EIMPL) {
			LogMessage(LOG_ERROR, "Sorry, but this vorbis mode is not supported currently...");
			return 0;
		}
		if (encode_ret == OV_EINVAL) {
			LogMessage(LOG_ERROR, "Sorry, but this is an illegal vorbis mode...");
			return 0;
		}

		vorbis_encode_setup_init(&g.vi);
		/* Now, set up the analysis engine, stream encoder, and other
		   preparation before the encoding begins.
		 */

		ret = vorbis_analysis_init(&g.vd,&g.vi);
		ret = vorbis_block_init(&g.vd,&g.vb);

		g.serialno = 0;
		srand(time(0));
		ret = ogg_stream_init(&g.os, rand());

		/* Now, build the three header packets and send through to the stream 
		   output stage (but defer actual file output until the main encode loop) */

		ogg_packet header_main;
		ogg_packet header_comments;
		ogg_packet header_codebooks;

		vorbis_comment	vc;

		char	title[1024] = "";
		char	artist[1024] = "";

		char	FullTitle[1024] = "";
		char	SongTitle[1024] = "";
		char	Artist[1024] = "";
		char	Streamed[1024] = "";

		memset(Artist, '\000', sizeof(Artist));
		memset(SongTitle, '\000', sizeof(SongTitle));
		memset(FullTitle, '\000', sizeof(FullTitle));
		memset(Streamed, '\000', sizeof(Streamed));

		vorbis_comment_init(&vc);

		getCurrentSongTitle(SongTitle, Artist, FullTitle);
		if ((strlen(SongTitle) == 0) && (strlen(Artist) == 0))  {
			sprintf(title, "TITLE=%s", FullTitle);
		}
		else {
			sprintf(title, "TITLE=%s", SongTitle);
		}
		vorbis_comment_add(&vc,title);
		sprintf(artist, "ARTIST=%s", Artist);
		vorbis_comment_add(&vc,artist);
		sprintf(Streamed, "ENCODEDBY=oddcast %s", g.gOggEncoderText);
		vorbis_comment_add(&vc,Streamed);
		if (strlen(g.sourceDescription) > 0) {
			sprintf(Streamed, "TRANSCODEDFROM=%s", g.sourceDescription);
			vorbis_comment_add(&vc,Streamed);
		}

		/* Build the packets */
		memset(&header_main, '\000', sizeof(header_main));
		memset(&header_comments, '\000', sizeof(header_comments));
		memset(&header_codebooks, '\000', sizeof(header_codebooks));

		vorbis_analysis_headerout(&g.vd,&vc, &header_main,&header_comments,&header_codebooks);


		ogg_stream_packetin(&g.os,&header_main);
		ogg_stream_packetin(&g.os,&header_comments);
		ogg_stream_packetin(&g.os,&header_codebooks);

		g.in_header = 1;

		ogg_page	og;

		int eos = 0;

		int sentbytes = 0;
		int ret = 0;
		while(!eos){
			int result=ogg_stream_flush(&g.os,&og);
			if(result==0)break;
			sentbytes += ice2_send_data(og.header, og.header_len);
			sentbytes += ice2_send_data(og.body, og.body_len);

			if (g.gSaveFile) {
				if (!g.gSaveWAV) {
					ret = fwrite(og.header, og.header_len, 1, g.gSaveFile);
					ret += fwrite(og.body, og.body_len, 1, g.gSaveFile);
				}
			}
		}

		
		vorbis_comment_clear(&vc);
	}
	return 1;

}

int do_encoding(float *samples, int numsamples, int nch) {
        g.gCurrentlyEncoding = 1;
        g.sampleData.samples = samples;
        g.sampleData.numsamples = numsamples;
        g.sampleData.nch = nch;
        return encodeit();
}

int encodeit()
{

	int s;
	int count = 0;
	unsigned char mp3buffer[LAME_MAXMP3BUFFER];
	int imp3;
	
	float *samples;
	short int	int_samples[10000];

	int			numsamples;
	int			nch;

	int		eos = 0;
	int		ret = 0;
	int		sentbytes = 0;
	char	buf[255] = "";


	g.gCurrentlyEncoding = 1;

	samples = g.sampleData.samples;
	numsamples = g.sampleData.numsamples;
	nch = g.sampleData.nch;

	if (g.weareconnected) {
		s = numsamples*nch;

		long leftMax = 0;
		long rightMax = 0;


		
		int	samplecounter = 0;

		for (int i = 0; i < numsamples; i++) {
//			if (samples[samplecounter] > leftMax) {
				//leftMax = (int)((float)samples[samplecounter]*32768.f);
				leftMax += abs((int)((float)samples[samplecounter]*32768.f));
//			}
			samplecounter++;
			if (nch == 2) {
//				if (samples[samplecounter] > rightMax) {
					//rightMax = (int)((float)samples[samplecounter]*32768.f);
					rightMax += abs((int)((float)samples[samplecounter]*32768.f));
//				}
				samplecounter++;
			}
			else {
				rightMax = leftMax;
			}
		}
		if (numsamples > 0) {
			leftMax = leftMax/(numsamples*nch);
			rightMax = rightMax/(numsamples*nch);

			int divider = 64;
			if (nch == 1) {
				divider = 128;
			}
			else {
				divider = 64;
			}
			  leftMax = (int)((leftMax) / divider);
			  rightMax = (int)((rightMax) / divider);
			  if (g.VUCallback) {
				  g.VUCallback(leftMax, rightMax);
			  }
		}

		if (g.gOggFlag) {

			// If a song change was detected, close the stream and resend new 
			// vorbis headers (with new comments) - all done by icecast2SendMetadata();
			if (g.ice2songChange) {
				g.ice2songChange = false;
				icecast2SendMetadata();

			}


			float **buffer = vorbis_analysis_buffer(&g.vd, numsamples);
			int samplecount = 0;
			int i;
			samplecounter = 0;
			
			for (i = 0; i < numsamples; i++) {
				buffer[0][i] = samples[samplecounter];
				samplecounter++;
				if (nch == 2) {
					buffer[1][i] = samples[samplecounter];
					samplecounter++;
				}
			}

			ret = vorbis_analysis_wrote(&g.vd, numsamples);

			// Stream out what we just prepared for Vorbis...
			sentbytes = ogg_encode_dataout();

		}
		if (g.gLAMEFlag) {
			// Lame encoding is simple, we are passing it interleaved samples
			for (int i = 0; i < numsamples*nch; i++) {
				int_samples[i] = (int)(samples[i]*32768.0);
			}
#ifdef WIN32
			unsigned long dwWrite = 0;
			int err = g.beEncodeChunk(g.hbeStream, numsamples*nch, (short *)&int_samples, (PBYTE)mp3buffer, &dwWrite);
			imp3 = dwWrite;
#else
			if (nch ==1) {
				imp3 = lame_encode_buffer(g.gf, (short int *)&int_samples, (short int *)&int_samples, numsamples, mp3buffer, sizeof(mp3buffer));
			}
			else {
				imp3 = lame_encode_buffer_interleaved(g.gf, (short int *)&int_samples, numsamples, mp3buffer, sizeof(mp3buffer));
			}
#endif

			if (imp3==-1) {
					LogMessage(LOG_ERROR, "mp3 buffer is not big enough!");
					return -1;
			}

			if (g.writeBytesCallback) {
				g.writeBytesCallback((void *)imp3);
			}
			//Send out the encoded buffer
			sentbytes = send(g.gSCSocket, (const char *)mp3buffer, imp3, (int)0);
			if (g.gdumpData) {
				fwrite(mp3buffer, 1, imp3, g.outToServer);
				fflush(g.outToServer);
			}
		}	
		if (g.gWMAFlag) {
			for (int i = 0; i < numsamples*nch; i++) {
				int_samples[i] = (short int)(samples[i]*32768.0);
			}
#ifdef WIN32
			sentbytes = doWMAencoding((void *)&int_samples, numsamples*nch, 0);
#endif
			
		}
		// Generic error checking, if there are any socket problems, the trigger
		// a disconnection handling...

		if (sentbytes == SOCKET_ERROR) {
			disconnectFromServer();
			if (g.gForceStop) {
				g.gForceStop = 0;
				return 0;
			}
			if (g.gAutoReconnect) {
				if (g.serverStatusCallback) {
					g.serverStatusCallback((void *)"Disconnected from ouput server..reconnecting");
#ifdef ODDCAST_DSP_WA3
					return 0;
#endif
					greconnectFlag = 1;
					while (greconnectFlag) {
#ifdef WIN32
						Sleep(g.gReconnectSec*1000);
#else
						sleep(g.gReconnectSec);
#endif
						if (greconnectFlag) {
							if (connectToServer()) {
								initializeencoder();
								greconnectFlag = 0;
							}
							else {
								greconnectFlag = 1;
							}
						}
					}
				}
			}
			else {
				if (g.serverStatusCallback) {
					g.serverStatusCallback((void *)"Was disconnected from server");
				}
				return 0;
			}
		}


	}
	g.gCurrentlyEncoding = 0;
	return 1;
}


/*
/////////////////////////////////////////////
//// UP TO HERE
/////////////////////////////////////////////
*/


void config_read()
{
	strcpy(g.gAppName, "oddcast");

	char    buf[255] = "";
	char	desc[1024] = "";

	sprintf(desc, "The source URL for the broadcast. It must be in the form http://server:port/mountpoint.  For those servers without a mountpoint (Shoutcast) use http://server:port. To record from line in, use 'linein' for this entry");
	GetConfigVariable(g.gAppName,"SourceURL","http://localhost/",g.gSourceURL,sizeof(g.gSourceURL), g.gIniFile, desc);
	if (g.sourceURLCallback) {
		g.sourceURLCallback((char *)g.gSourceURL);
	}
	sprintf(desc, "The server to which the stream is sent. It can be a hostname  or IP (example: www.stream.com, 192.168.1.100)");
	GetConfigVariable(g.gAppName,"Server","localhost",g.gServer,sizeof(g.gServer), g.gIniFile, desc);
	sprintf(desc, "The port to which the stream is sent. Must be a number (example: 8000)");
	GetConfigVariable(g.gAppName,"Port", "8000", g.gPort, sizeof(g.gPort), g.gIniFile, desc);
	sprintf(desc, "This is the encoder password for the destination server (example: hackme)");
	GetConfigVariable(g.gAppName,"ServerPassword", "changemenow", g.gPassword, sizeof(g.gPassword), g.gIniFile, desc);
	sprintf(desc, "This setting tells the destination server to list on any available YP listings. Not all servers support this (Shoutcast does, Icecast2 doesn't) (example: 1 for YES, 0 for NO)");
	g.gPubServ = GetConfigVariableLong(g.gAppName,"ServerPublic",1, g.gIniFile, desc);
	sprintf(desc, "This is used in the YP listing, I think only Shoutcast supports this (example: #mystream)");
	GetConfigVariable(g.gAppName,"ServerIRC","",g.gServIRC,sizeof(g.gServIRC), g.gIniFile, desc);
	sprintf(desc, "This is used in the YP listing, I think only Shoutcast supports this (example: myAIMaccount)");
	GetConfigVariable(g.gAppName,"ServerAIM","",g.gServAIM,sizeof(g.gServAIM), g.gIniFile, desc);
	sprintf(desc, "This is used in the YP listing, I think only Shoutcast supports this (example: 332123132)");
	GetConfigVariable(g.gAppName,"ServerICQ","",g.gServICQ,sizeof(g.gServICQ), g.gIniFile, desc);
	sprintf(desc, "The URL that is associated with your stream. (example: http://www.mystream.com)");
	GetConfigVariable(g.gAppName,"ServerStreamURL","http://www.oddsock.org",g.gServURL,sizeof(g.gServURL), g.gIniFile, desc);
	sprintf(desc, "A short description of the stream (example: Stream House on Fire!)");
	GetConfigVariable(g.gAppName,"ServerDescription","This is my server desription",g.gServDesc,sizeof(g.gServDesc), g.gIniFile, desc);
	sprintf(desc, "Genre of music, can be anything you want... (example: Rock)");
	GetConfigVariable(g.gAppName,"ServerGenre","Rock",g.gServGenre,sizeof(g.gServGenre), g.gIniFile, desc);
	sprintf(desc, "Used for Icecast/Icecast2 servers, The mountpoint must end in .ogg for Vorbis streams and have NO extention for MP3 streams.  If you are sending to a Shoutcast server, this MUST be blank. (example: /mp3, /myvorbis.ogg)");
	GetConfigVariable(g.gAppName,"ServerMountpoint","",g.gMountpoint,sizeof(g.gMountpoint), g.gIniFile, desc);
	sprintf(desc, "Wether or not oddcast will reconnect if it is disconnected from the destination server (example: 1 for YES, 0 for NO)");
	g.gAutoReconnect = GetConfigVariableLong(g.gAppName,"AutomaticReconnect",0, g.gIniFile, desc);

	sprintf(desc, "How long it will wait (in seconds) between reconnect attempts. (example: 10)");
	g.gReconnectSec = GetConfigVariableLong(g.gAppName,"AutomaticReconnectSecs",10, g.gIniFile, desc);
	sprintf(desc, "What format to encode to. Valid values are (OGG, LAME) (example: OGG, LAME)");
	GetConfigVariable(g.gAppName,"Encode","LAME",g.gEncodeType,sizeof(g.gEncodeType), g.gIniFile, desc);
	if (!strncmp(g.gEncodeType, "L", 1)) { //LAME
		g.gOggFlag = 0;
		g.gLAMEFlag = 1;
#ifdef WIN32
		g.gWMAFlag = 0;
#endif
	}
	if (!strncmp(g.gEncodeType, "O", 1)) { //OGG
		g.gOggFlag = 1;
		g.gLAMEFlag = 0;
#ifdef WIN32
		g.gWMAFlag = 0;
#endif
	}
	if (!strncmp(g.gEncodeType, "W", 1)) { //WMA
#ifndef WIN32
		LogMessage(LOG_ERROR, "WMA encoding not supported on non-win32 platforms");
		g.gOggFlag = 0;
		g.gLAMEFlag = 0;
#else
		g.gWMAFlag = 1;
#endif
		g.gOggFlag = 0;
		g.gLAMEFlag = 0;

	}
	if (g.streamTypeCallback) {
		g.streamTypeCallback((void *)buf);
	}

		
	sprintf(desc, "The port used for WMA stream (example: 8200)");
	GetConfigVariable(g.gAppName,"WMAPort", "8200", buf, sizeof(buf), g.gIniFile, desc);
	g.gWMAPort = atoi(buf);


	memset(buf, '\000', sizeof(buf));
	sprintf(desc, "Number of clients to support WMA encoding. (example: 1, 10)");
	GetConfigVariable(g.gAppName,"WMAClients","5",buf,sizeof(buf), g.gIniFile, desc);
	g.gWMAClients = atoi(buf);

	GetConfigVariable(g.gAppName,"WMAMode","Stereo",g.gWMAMode,sizeof(g.gWMAMode), g.gIniFile, desc);

	if (g.gWMAFlag) {
		if (g.destURLCallback) {
			sprintf(buf, "http://localhost:%d", g.gWMAPort);	
			g.destURLCallback((char *)buf);
		}
	}
	else {
		if (g.destURLCallback) {
			sprintf(buf, "http://%s:%s%s", g.gServer, g.gPort, g.gMountpoint);	
			g.destURLCallback((char *)buf);
		}
	}

	sprintf(desc, "Bitrate. This is the mean bitrate if using VBR.");
	g.currentBitrate = GetConfigVariableLong(g.gAppName,"BitrateNominal",128,g.gIniFile, desc);

	sprintf(desc, "Minimum Bitrate. Used only if using Bitrate Management (not recommended) or LAME VBR(example: 64, 128)");
	g.currentBitrateMin = GetConfigVariableLong(g.gAppName,"BitrateMin",128, g.gIniFile, desc);

	sprintf(desc, "Maximum Bitrate. Used only if using Bitrate Management (not recommended) or LAME VBR (example: 64, 128)");
	g.currentBitrateMax = GetConfigVariableLong(g.gAppName,"BitrateMax",128, g.gIniFile, desc);

	sprintf(desc, "Number of channels. Valid values are (1, 2). 1 means Mono, 2 means Stereo (example: 2,1)");
	g.currentChannels = GetConfigVariableLong(g.gAppName,"NumberChannels",2,g.gIniFile, desc);

	sprintf(desc, "Vorbis Quality Level. Valid values are between -1 (lowest quality) and 10 (highest).  The lower the quality the lower the output bitrate. (example: -1, 3)");
	GetConfigVariable(g.gAppName,"OggQuality","0",g.gOggQuality,sizeof(g.gOggQuality), g.gIniFile, desc);

	sprintf(desc, "Sample rate for the stream. Valid values depend on wether using lame or vorbis. Vorbis supports odd samplerates such as 32kHz and 48kHz, but lame appears to not.feel free to experiment (example: 44100, 22050, 11025)");
	g.currentSamplerate = GetConfigVariableLong(g.gAppName,"Samplerate",44100, g.gIniFile, desc);

	sprintf(desc, "This flag specifies if you want Vorbis Quality or Bitrate Management.  Quality is always recommended. Valid values are (Bitrate, Quality). (example: Quality, Bitrate Management)");
	GetConfigVariable(g.gAppName,"OggBitrateQualityFlag","Quality",g.gOggBitQual,sizeof(g.gOggBitQual), g.gIniFile, desc);
	g.gOggBitQualFlag = 0;
	if (!strncmp(g.gOggBitQual, "Q", 1)) { //Quality
		   g.gOggBitQualFlag = 0;
	}
	if (!strncmp(g.gOggBitQual, "B", 1)) { //Bitrate
		   g.gOggBitQualFlag = 1;
	}

	g.gAutoCountdown = atoi(g.gAutoStartSec);
	if (strlen(g.gMountpoint) > 0) {
		strcpy(g.gIceFlag, "1");
	}
	else {
		strcpy(g.gIceFlag, "0");
	}

	char tempString[255] = "";

	memset(tempString, '\000', sizeof(tempString));
	ReplaceString(g.gServer, tempString, " ", "");
	strcpy(g.gServer, tempString);

	memset(tempString, '\000', sizeof(tempString));
	ReplaceString(g.gPort, tempString, " ", "");
	strcpy(g.gPort, tempString);

	
	sprintf(desc, "This LAME flag indicates that CBR encoding is desired. If this flag is set then LAME with use CBR, if not set then it will use VBR (and you must then specify a VBR mode). Valid values are (1 for SET, 0 for NOT SET) (example: 1)");
	g.gLAMEOptions.cbrflag = GetConfigVariableLong(g.gAppName,"LameCBRFlag",1, g.gIniFile, desc);
	sprintf(desc, "A number between 1 and 10 which indicates the desired quality level of the stream.  The higher the number, the higher the quality.  Also, the higher the number, the higher the CPU utilization. So for slower CPUs, try a low quality number (example: 5)");
	g.gLAMEOptions.quality = GetConfigVariableLong(g.gAppName,"LameQuality",1, g.gIniFile, desc);

	sprintf(desc, "Copywrite flag. Not used for much. Valid values (1 for YES, 0 for NO)");
	g.gLAMEOptions.copywrite = GetConfigVariableLong(g.gAppName,"LameCopywrite",0, g.gIniFile, desc);
	sprintf(desc, "Original flag. Not used for much. Valid values (1 for YES, 0 for NO)");
	g.gLAMEOptions.original = GetConfigVariableLong(g.gAppName,"LameOriginal",0, g.gIniFile, desc);
	sprintf(desc, "Strict ISO flag. Not used for much. Valid values (1 for YES, 0 for NO)");
	g.gLAMEOptions.strict_ISO = GetConfigVariableLong(g.gAppName,"LameStrictISO",0, g.gIniFile, desc);
	sprintf(desc, "Disable Reservior flag. Not used for much. Valid values (1 for YES, 0 for NO)");
	g.gLAMEOptions.disable_reservoir = GetConfigVariableLong(g.gAppName,"LameDisableReservior",0, g.gIniFile, desc);
	sprintf(desc, "This specifies the type of VBR encoding LAME will perform if VBR encoding is set (CBRFlag is NOT SET). See the LAME documention for more on what these mean. Valid values are (vbr_rh, vbr_mt, vbr_mtrh, vbr_abr)");
	GetConfigVariable(g.gAppName,"LameVBRMode","vbr_abr",g.gLAMEOptions.VBR_mode,sizeof(g.gLAMEOptions.VBR_mode), g.gIniFile, desc);

	sprintf(desc, "Use LAMEs lowpass filter. If you set this to 0, then no filtering is done.");
	g.gLAMEOptions.lowpassfreq = GetConfigVariableLong(g.gAppName,"LameLowpassfreq",0, g.gIniFile, desc);
	sprintf(desc, "Use LAMEs highpass filter. If you set this to 0, then no filtering is done.");
	g.gLAMEOptions.highpassfreq = GetConfigVariableLong(g.gAppName,"LameHighpassfreq",0, g.gIniFile, desc);

	if (g.gLAMEOptions.lowpassfreq > 0) {
		g.gLAMELowpassFlag = 1;
	}
	if (g.gLAMEOptions.highpassfreq > 0) {
		g.gLAMELowpassFlag = 1;
	}

	sprintf(desc, "The destination server type.  You must set this correctly otherwise you will get problems connecting the encoder.  Please make sure you are using the correct one. Valid values are (Shoutcast, Icecast, Icecast2) (example: Icecast2)");
	GetConfigVariable(g.gAppName,"ServerType","Shoutcast",g.gServerType,sizeof(g.gServerType), g.gIniFile, desc);

	
	if (!strcmp(g.gServerType, "Shoutcast")) {
		g.gShoutcastFlag = 1;
		g.gIcecastFlag = 0;
		g.gIcecast2Flag = 0;
	}
	if (!strcmp(g.gServerType, "Icecast")) {
		g.gShoutcastFlag = 0;
		g.gIcecastFlag = 1;
		g.gIcecast2Flag = 0;
	}
	if (!strcmp(g.gServerType, "Icecast2")) {
		g.gShoutcastFlag = 0;
		g.gIcecastFlag = 0;
		g.gIcecast2Flag = 1;
	}
	if (!strcmp(g.gServerType, "WMA")) {
		g.gShoutcastFlag = 0;
		g.gIcecastFlag = 0;
		g.gIcecast2Flag = 0;
		g.gWMAFlag = 1;
	}
	if (g.serverTypeCallback) {
		g.serverTypeCallback((void *)g.gServerType);
	}

	sprintf(desc, "If recording from linein, what device to use (not needed for win32) (example: /dev/dsp)");
	GetConfigVariable(g.gAppName,"AdvRecDevice", "/dev/dsp",buf,sizeof(buf), g.gIniFile, desc);
	strcpy(g.gAdvRecDevice, buf);

	sprintf(desc, "If recording from linein, what sample rate to open the device with. (example: 44100, 48000)");
	GetConfigVariable(g.gAppName,"LiveInSamplerate", "44100",buf,sizeof(buf), g.gIniFile, desc);
	g.gLiveInSamplerate = atoi(buf);

	sprintf(desc, "Flag which indicates we are recording from line in");
	g.gLiveRecordingFlag = GetConfigVariableLong(g.gAppName,"LineInFlag",0,g.gIniFile, desc);

	// Set some derived values
	char localBitrate[255] = "";

	char	mode[25] = "";

	if (g.currentChannels == 1) {
		strcpy(mode, "Mono");
	}
	if (g.currentChannels == 2) {
		strcpy(mode, "Stereo");
	}
	

	if (g.gOggFlag) {
		if (g.bitrateCallback) {
			if (g.gOggBitQualFlag == 0) { // Quality
				sprintf(localBitrate, "Quality %s/%s/%d", g.gOggQuality, mode, g.currentSamplerate);
			}
			else {
				sprintf(localBitrate, "(%d/%d/%d)/%s/%d", g.currentBitrateMin, g.currentBitrate, g.currentBitrateMax, mode, g.currentSamplerate);
			}
			g.bitrateCallback((void *)localBitrate);
		}
	}
	if (g.gLAMEFlag) {
		if (g.bitrateCallback) {
			if (g.gLAMEOptions.cbrflag) {
				sprintf(localBitrate, "%dkbps/%dHz/%s", g.currentBitrate, g.currentSamplerate,mode); 
			}
			else {
				sprintf(localBitrate, "(%d/%d/%d)/%s/%d", g.currentBitrateMin, g.currentBitrate, g.currentBitrateMax, mode, g.currentSamplerate); 
			}
			g.bitrateCallback((void *)localBitrate);
		}
	} 
	if (g.gWMAFlag) {
		if (g.bitrateCallback) {
			sprintf(localBitrate, "%dkbps/%dHz/%s", g.currentBitrate, g.currentSamplerate,mode); 
			g.bitrateCallback((void *)localBitrate);
		}
	} 
	if (g.serverStatusCallback) {
		g.serverStatusCallback((void *)"Disconnected");
	}

}

void config_write()
{
	strcpy(g.gAppName, "oddcast");

	char    buf[255] = "";
	char	desc[1024] = "";
	char	tempString[1024] = "";

	memset(tempString, '\000', sizeof(tempString));
	ReplaceString(g.gServer, tempString, " ", "");
	strcpy(g.gServer, tempString);

	memset(tempString, '\000', sizeof(tempString));
	ReplaceString(g.gPort, tempString, " ", "");
	strcpy(g.gPort, tempString);

	PutConfigVariable(g.gAppName,"SourceURL",g.gSourceURL,g.gIniFile);
	PutConfigVariable(g.gAppName,"Server",g.gServer, g.gIniFile);
	PutConfigVariable(g.gAppName,"Port",  g.gPort, g.gIniFile);
	PutConfigVariable(g.gAppName,"ServerPassword", g.gPassword, g.gIniFile);
	PutConfigVariableLong(g.gAppName,"ServerPublic",g.gPubServ, g.gIniFile);
	PutConfigVariable(g.gAppName,"ServerIRC",g.gServIRC,g.gIniFile);
	PutConfigVariable(g.gAppName,"ServerAIM",g.gServAIM, g.gIniFile);
	PutConfigVariable(g.gAppName,"ServerICQ",g.gServICQ, g.gIniFile);
	PutConfigVariable(g.gAppName,"ServerStreamURL",g.gServURL,g.gIniFile);
	PutConfigVariable(g.gAppName,"ServerDescription",g.gServDesc, g.gIniFile);
	PutConfigVariable(g.gAppName,"ServerGenre",g.gServGenre,g.gIniFile);
	PutConfigVariable(g.gAppName,"ServerMountpoint",g.gMountpoint, g.gIniFile);
	PutConfigVariableLong(g.gAppName,"AutomaticReconnect",g.gAutoReconnect,g.gIniFile);
	PutConfigVariableLong(g.gAppName,"AutomaticReconnectSecs", g.gReconnectSec, g.gIniFile);
	PutConfigVariable(g.gAppName,"Encode",g.gEncodeType,g.gIniFile);

	PutConfigVariableLong(g.gAppName,"WMAPort", g.gWMAPort,  g.gIniFile);

	PutConfigVariableLong(g.gAppName,"WMAClients",g.gWMAClients, g.gIniFile);
	PutConfigVariable(g.gAppName,"WMAMode",g.gWMAMode, g.gIniFile);
	PutConfigVariableLong(g.gAppName,"BitrateNominal",g.currentBitrate,g.gIniFile);
	PutConfigVariableLong(g.gAppName,"BitrateMin",g.currentBitrateMin, g.gIniFile);
	PutConfigVariableLong(g.gAppName,"BitrateMax",g.currentBitrateMax, g.gIniFile);
	PutConfigVariableLong(g.gAppName,"NumberChannels",g.currentChannels,g.gIniFile);
	PutConfigVariable(g.gAppName,"OggQuality",g.gOggQuality, g.gIniFile);
	PutConfigVariableLong(g.gAppName,"Samplerate",g.currentSamplerate, g.gIniFile);
	PutConfigVariable(g.gAppName,"OggBitrateQualityFlag",g.gOggBitQual, g.gIniFile);
	PutConfigVariableLong(g.gAppName,"LameCBRFlag",g.gLAMEOptions.cbrflag, g.gIniFile);
	PutConfigVariableLong(g.gAppName,"LameQuality",g.gLAMEOptions.quality, g.gIniFile);
	PutConfigVariableLong(g.gAppName,"LameCopywrite",g.gLAMEOptions.copywrite, g.gIniFile);
	PutConfigVariableLong(g.gAppName,"LameOriginal",g.gLAMEOptions.original, g.gIniFile);
	PutConfigVariableLong(g.gAppName,"LameStrictISO",g.gLAMEOptions.strict_ISO, g.gIniFile);
	PutConfigVariableLong(g.gAppName,"LameDisableReservior",g.gLAMEOptions.disable_reservoir, g.gIniFile);
	PutConfigVariable(g.gAppName,"LameVBRMode",g.gLAMEOptions.VBR_mode, g.gIniFile);
	PutConfigVariableLong(g.gAppName,"LameLowpassfreq",g.gLAMEOptions.lowpassfreq , g.gIniFile);
	PutConfigVariableLong(g.gAppName,"LameHighpassfreq",g.gLAMEOptions.highpassfreq, g.gIniFile);
	PutConfigVariable(g.gAppName,"ServerType",g.gServerType, g.gIniFile);
	PutConfigVariable(g.gAppName,"AdvRecDevice", g.gAdvRecDevice, g.gIniFile);
	PutConfigVariableLong(g.gAppName,"LiveInSamplerate", g.gLiveInSamplerate, g.gIniFile);
	PutConfigVariableLong(g.gAppName,"LineInFlag", g.gLiveRecordingFlag, g.gIniFile);

}

/*
* Input is in interleaved float samples
*
*/
int handle_output(float *samples, int nsamples, int nchannels, int in_samplerate)
{
//	float	samples_resampled[8196];
//	short int	samples_resampled_int[8196];
//	float	samples_rechannel[8196];
	int  ret = 1;

	long	out_samplerate = getCurrentSamplerate();
	long	out_nch = getCurrentChannels();
	int	samplecount = 0;
	float	*samplePtr = 0;

	float *samples_resampled = NULL;
	short *samples_resampled_int = NULL;
	float *samples_rechannel = NULL;

	samples_rechannel = (float *)malloc(sizeof(float)*nsamples*nchannels);
	memset(samples_rechannel, '\000', sizeof(float)*nsamples*nchannels);

	samplePtr = samples;
	if ((nchannels == 2) && (out_nch == 1)) {
		make_mono((float *)samples,(float *)samples_rechannel, nsamples);
		samplePtr = (float *)samples_rechannel;
	}
	if ((nchannels == 1) && (out_nch == 2)) {
		make_stereo((float *)samples,(float *)samples_rechannel, nsamples);
		samplePtr = (float *)samples_rechannel;
	}

	// Call the resampler
	int buf_samples = (( nsamples * out_samplerate ) / in_samplerate);
	initializeResampler(in_samplerate, nchannels);

	samples_resampled = (float *)malloc(sizeof(float)*buf_samples*nchannels);
	memset(samples_resampled, '\000', sizeof(float)*buf_samples*nchannels);

	long out_samples = ocConvertAudio((float *)samplePtr,(float *)samples_resampled,nsamples,buf_samples);

	samples_resampled_int = (short *)malloc(sizeof(short)*out_samples*nchannels);
	memset(samples_resampled_int, '\000', sizeof(short)*out_samples*nchannels);

	if (out_samples > 0) {
		samplecount = 0;

		LogMessage(LOG_DEBUG, "Calling DoEncoding with out_samples %d and out_nch = %d", out_samples, out_nch);
		// Here is the call to actually do the encoding....
		if (g.outToEncoder != 0) {
			for (int i=0;i<out_samples*nchannels;i++) {
				samples_resampled_int[i] = (int)(samples_resampled[i]*32767.f);
			}
			fwrite(samples_resampled_int, out_samples, 1, g.outToEncoder);
		}
		
		ret = do_encoding((float *)(samples_resampled), out_samples, out_nch);
	}

	if (samples_resampled_int) {
		free(samples_resampled_int);
		samples_resampled_int = NULL;
	}
	if (samples_resampled) {
		free(samples_resampled);
		samples_resampled = NULL;
	}
	if (samples_rechannel) {
		free(samples_rechannel);
		samples_rechannel = NULL;
	}

	/*
	samplePtr = samples;
	if ((nchannels == 2) && (out_nch == 1)) {
		make_mono((float *)samples,(float *)&samples_rechannel, nsamples);
		samplePtr = (float *)&samples_rechannel;
	}
	if ((nchannels == 1) && (out_nch == 2)) {
		make_stereo((float *)samples,(float *)&samples_rechannel, nsamples);
		samplePtr = (float *)&samples_rechannel;
	}

	// Call the resampler
	int buf_samples = (( nsamples * out_samplerate ) / in_samplerate);
	initializeResampler(in_samplerate, nchannels);


	long out_samples = ocConvertAudio((float *)samplePtr,(float *)&samples_resampled,nsamples,buf_samples);

	if (out_samples > 0) {
		samplecount = 0;

		LogMessage(LOG_DEBUG, "Calling DoEncoding with out_samples %d and out_nch = %d", out_samples, out_nch);
		// Here is the call to actually do the encoding....
		
		ret = do_encoding((float *)&(samples_resampled), out_samples, out_nch);
	}
	*/
	return ret;
}
void	startRecording() {
	if (!g.areLiveRecording) {
#ifdef WIN32
		waveInStart(g.inHandle);
#else
		short int buffer[8192*2];
        int length = 0;
        int nch = 2;

		while (g.areLiveRecording) {
			length = read(g.inHandle, buffer, sizeof(buffer));

			if (length < sizeof(buffer)) {
					stopRecording();
					g.gLiveRecordingFlag = 0;
					break;
			}
			else {
					unsigned int nchannels, nsamples;
					float	samples[8196];
					short	*in_samples;

					static short lastSample = 0;
					static signed int sample;

					long	in_samplerate = g.gLiveInSamplerate;
					long	out_samplerate = getCurrentSamplerate();
					long	in_nch = 2;
					long	out_nch = getCurrentChannels();

					nchannels = 2;
					nsamples  = 1152;
					in_samples = (short *)&buffer;
					// Send it to liboddcast (cross fingers)
					int	samplecount = 0;
					for (int i=0;i<nsamples;i++) {
						signed int sample;

						samples[i] = *in_samples/32767.f;
						// clipping
						if (samples[i] > 1.0) {
							samples[i] = 1.0;
						}
						if (samples[i] < -1.0) {
							samples[i] = -1.0;
						}
						in_samples++;
					}
					LogMessage(LOG_DEBUG, "handle_output - nsamples = %d, nchannels = %d, in_samplerate = %d, in_nch = %d", nsamples, nchannels, in_samplerate, in_nch);
					handle_output((float *)&samples, nsamples, nchannels, in_samplerate);
			}
		}
#endif
		g.areLiveRecording = TRUE;
	}
}
void	stopRecording() {
	if (g.areLiveRecording) {
#ifdef WIN32
		//waveInStop(g.inHandle);
		BASS_ChannelStop(RECORDCHAN);
		BASS_RecordFree();
#endif
		g.areLiveRecording = FALSE;
	}
}

#ifdef WIN32
void CALLBACK waveInputProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1,DWORD dwParam2) 
{
	if (uMsg == MM_WIM_DATA) {
		
		WAVEHDR *WAVbuf;
		
		if (g.gLiveRecordingFlag) {
			WAVbuf = (WAVEHDR *)dwParam1;

			unsigned int nchannels, nsamples;
			float	samples[8196*2];
			short	*in_samples;

			static short lastSample = 0;
			static signed int sample;



			long	in_samplerate = 44100;
			long	out_samplerate = getCurrentSamplerate();
			long	in_nch = 2;
			long	out_nch = getCurrentChannels();

			nchannels = 2;
			nsamples  = 1152;
			//nsamples = WAVbuf->dwBytesRecorded;
			in_samples = (short *)WAVbuf->lpData;
			// Send it to liboddcast (cross fingers)
			int	samplecount = 0;
			for (int i=0;i<nsamples*nchannels;i++) {
				signed int sample;

				samples[i] = *in_samples/32767.f;
				// clipping
				if (samples[i] > 1.0) {
					samples[i] = 1.0;
				}
				if (samples[i] < -1.0) {
					samples[i] = -1.0;
				}
				in_samples++;
			}
			LogMessage(LOG_DEBUG, "handle_output - nsamples = %d, nchannels = %d, in_samplerate = %d, in_nch = %d", nsamples, nchannels, in_samplerate, in_nch);
			handle_output((float *)&samples, nsamples, nchannels, in_samplerate);

			waveInAddBuffer(g.inHandle, WAVbuf, sizeof(g.WAVbuffer1));
		}
	}
}
int CALLBACK BASSwaveInputProc(void *buffer, DWORD length, DWORD user) 
{
	int n;
	char *name;
		
	if (g.gLiveRecordingFlag) {
		for (n=0;name=BASS_RecordGetInputName(n);n++) {
			int s=BASS_RecordGetInput(n);
			if (!(s&BASS_INPUT_OFF)) {
				setCurrentRecordingName(name);
			}
		}

		  unsigned int c_size = length;  //in bytes.
		  short * z = (short *)buffer;   //signed short for pcm data.

		  int numsamples = c_size/sizeof(short);

		  int nch = 2;
		  int srate = getLiveInSamplerate();
		  float	*samples;
		  
		  samples = (float *)malloc(sizeof(short)*numsamples*2);
		  memset(samples,'\000',sizeof(short)*numsamples*2);

		  long avgLeft = 0;
		  long avgRight = 0;
		  int flip = 0;

//		  memset(&samples, '\000', sizeof(samples));

		  for(unsigned int i = 0; i < numsamples; i++) {
				signed int sample;
				sample = z[i];
				samples[i] = sample/32767.f;
				// clipping
				if (samples[i] > 1.0) {
					samples[i] = 1.0;
				}
				if (samples[i] < -1.0) {
					samples[i] = -1.0;
				}
		  }
		
		  int ret = handle_output((float *)samples, numsamples/nch, nch, srate);
		  free(samples);
		  return ret;
	}
	else {
		return 0;
	}
	return 0;
}
#endif
int initLiveRecording() 
{
	/* This is for Advanced Recording */
	/* Initialize the WAVEFORMATEX for 16-bit, 44KHz, stereo */

#ifdef WIN32
/*
	g.waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	g.waveFormat.nChannels = 2;
	g.waveFormat.nSamplesPerSec = getLiveInSamplerate();
	g.waveFormat.wBitsPerSample = 16;
	g.waveFormat.nBlockAlign = g.waveFormat.nChannels * (g.waveFormat.wBitsPerSample/8);
	g.waveFormat.nAvgBytesPerSec = g.waveFormat.nSamplesPerSec * g.waveFormat.nBlockAlign;
	g.waveFormat.cbSize = 0;

//	Open the preferred Digital Audio In device 

	unsigned long	result = 0;
	if (g.inHandle) {
		waveInClose((g.inHandle));
		g.inHandle = NULL;
	}
		
	result = waveInOpen(&(g.inHandle), WAVE_MAPPER, &g.waveFormat, (DWORD)waveInputProc,0, CALLBACK_FUNCTION);
	if (result)	{
		if (g.generalStatusCallback) {
			g.generalStatusCallback((char *)"There was an error opening the preferred Digital Audio In device!");
		}

		return 0;
	}
	else {
		g.WAVbuffer1.lpData = (char *)&g.WAVsamplesbuffer1;
		g.WAVbuffer1.dwBufferLength = sizeof(g.WAVsamplesbuffer1);
		g.WAVbuffer1.dwFlags = 0;
		g.WAVbuffer2.lpData = (char *)&g.WAVsamplesbuffer2;
		g.WAVbuffer2.dwBufferLength = sizeof(g.WAVsamplesbuffer2);
		g.WAVbuffer2.dwFlags = 0;
		waveInPrepareHeader(g.inHandle, &g.WAVbuffer1, sizeof(g.WAVbuffer1));
		waveInPrepareHeader(g.inHandle, &g.WAVbuffer2, sizeof(g.WAVbuffer2));
		waveInAddBuffer(g.inHandle, &g.WAVbuffer1, sizeof(g.WAVbuffer1));
		waveInAddBuffer(g.inHandle, &g.WAVbuffer2, sizeof(g.WAVbuffer2));
	}
	*/
	char	buffer[1024] = "";
	char	buf[255] = "";


	int ret = BASS_RecordInit(-1);
	if (!ret) {
		DWORD errorCode = BASS_ErrorGetCode();
		switch (errorCode) {
			case BASS_ERROR_ALREADY:
				if (g.generalStatusCallback) {
					g.generalStatusCallback((char *)"Recording device already opened!");
				}
				return 0;
			case BASS_ERROR_DEVICE:
				if (g.generalStatusCallback) {
					g.generalStatusCallback((char *)"Recording device invalid!");
				}
				return 0;
			case BASS_ERROR_DRIVER:
				if (g.generalStatusCallback) {
					g.generalStatusCallback((char *)"Recording device driver unavailable!");
				}
				return 0;
			default:
				if (g.generalStatusCallback) {
					g.generalStatusCallback((char *)"There was an error opening the preferred Digital Audio In device!");
				}
				return 0;
		}
	}

	ret = BASS_RecordStart(getLiveInSamplerate(),0, BASSwaveInputProc, NULL);


	g.gLiveRecordingFlag = 1;
#else
	// Need some advanced recording stuff here

        int format = AFMT_S16_LE;
        int channels, rate;
        if((g.inHandle = open(g.gAdvRecDevice, O_RDONLY, 0)) == -1) {
			char	msg[1024] = "";
			sprintf(msg, "There was an error opening the preferred Digital Audio In device! (%s)", g.gAdvRecDevice);
			if (g.generalStatusCallback) {
				g.generalStatusCallback((char *)msg);
			}
            return 0;
        }

        if(ioctl(g.inHandle, SNDCTL_DSP_SETFMT, &format) == -1) {
			if (g.generalStatusCallback) {
				g.generalStatusCallback((char *)"There was an error setting sample format!");
			}
            return 0;
        }
        if(format != AFMT_S16_LE) {
			if (g.generalStatusCallback) {
				g.generalStatusCallback((char *)"Could not set sample format to AFMT_S16_LE");
			}
            return 0;
        }

        channels = 2;
        if(ioctl(g.inHandle, SNDCTL_DSP_CHANNELS, &channels) == -1) {
 				if (g.generalStatusCallback) {
					g.generalStatusCallback((char *)"Could not set number of channels");
				}
                return 0;
        }
        if(channels != 2) {
 				if (g.generalStatusCallback) {
					g.generalStatusCallback((char *)"Could not set number of channels");
				}
                return 0;
        }

        rate = 44100;
        if(ioctl(g.inHandle, SNDCTL_DSP_SPEED, &rate) == -1) {
 				if (g.generalStatusCallback) {
					g.generalStatusCallback((char *)"Could not set sample rate");
				}
                return 0;
        }
        if(rate != 44100) {
 				if (g.generalStatusCallback) {
					g.generalStatusCallback((char *)"Could not set sample rate");
				}
                return 0;
        }
        g.gLiveRecordingFlag = 1;

#endif

	return 1;
}
