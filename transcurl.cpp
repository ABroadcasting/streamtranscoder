/***************************************************************************** *                                  _   _ ____  _     
 *
 * $Id: transcurl.cpp,v 1.17 2002/12/20 23:46:35 oddsock Exp $
 */


#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#endif
#include <string.h>

#ifdef WIN32
#include <malloc.h>
#include <winsock2.h>
#endif
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <assert.h>
#include "transcurl.h"
#include "cbuffer.h"
#include "log.h"
#include "liboddcast.h"
#include "transcode.h"


extern transcodeGlobals	globals;

CBUFFER	circularBuffer;
char	vorbisHeader[8192];
int		vorbisHeaderPos = 0;

void parseHeader(char *http_header);

#ifdef WIN32
#define alloca	_alloca
#endif

#define MP3_FORMAT	1
#define OGG_FORMAT  2

#define SERVER_FORMAT_ICECAST 1
#define SERVER_FORMAT_SHOUTCAST 2

int	decode_pipe[2];

static	int	oggvHeaderRead = 0;
static	int header_read = 0;
static	int partial_header = 0;
static	int	metaDataFlag = 0;
static	int	chunkSize = 0;
static	int	formatFlag = MP3_FORMAT;
static	int serverFormatFlag = SERVER_FORMAT_SHOUTCAST;


transcoderGlobalStruct 	transglobal;

void	reset_transcoder()
{
		oggvHeaderRead = 0;
		header_read = 0;
		partial_header = 0;
		metaDataFlag = 0;
		chunkSize = 0;
		formatFlag = MP3_FORMAT;
		serverFormatFlag = SERVER_FORMAT_SHOUTCAST;
}
size_t header_function(void *ptr, size_t size, size_t nmemb, void *stream)
{
	if (!strcmp((char *)ptr, "\r\n")) {
		// We are done reading the headers
		metaDataFlag = 0;
		cbuffer_init(&circularBuffer, chunkSize*20);	//JCMOD, made this a lot larger
		switch (formatFlag) {
			case MP3_FORMAT:
							mp3dec_init(&(transglobal.mp3dec));
							initializeencoder();
							break;
			case OGG_FORMAT:
							break;
		}
		header_read = 1;
	}
	else {
		parseIcecastHeader((char *)ptr);
	}
	LogMessage(LOG_DEBUG, "Header: %s\n", ptr);
	return nmemb;
}

int sendToMP3Decoder(char *pData, long realsize) {
	static	char	bitrate[255] = "";
	char	currentBitrate[255] = "";
	char	currentChannels[255] = "";

	mp3dec_feed_stream(&(transglobal.mp3dec), pData, realsize);
	if (mp3dec_get_stream_size(&(transglobal.mp3dec)) > MIN_MP3_STREAM_SIZE) {
		if (!mp3dec_decode(&(transglobal.mp3dec))) {
			return 0;
		}
	}
	memset(currentBitrate, '\000', sizeof(currentBitrate));
	memset(currentChannels, '\000', sizeof(currentChannels));

	if (transglobal.mp3dec.nch == 1) {
		strcpy(currentChannels, "Mono");
	}
	if (transglobal.mp3dec.nch == 2) {
		strcpy(currentChannels, "Stereo");
	}

	sprintf(currentBitrate, "%dHz/%s MP3",  transglobal.mp3dec.sampleRate, currentChannels); 
	if (strcmp(currentBitrate, bitrate)) {
		strcpy(bitrate, currentBitrate);
		if (globals.inputBitrateCallback) {
			globals.inputBitrateCallback(bitrate);
		}
		setSourceDescription(bitrate);
	}
	return 1;

}

char	httpHeader[2046] = "";

#ifdef _WIN32
DWORD WINAPI startDecodingVorbisWin32(LPVOID lParam)
{
	startDecodingVorbis(NULL);
	return 0;
}
#endif

void parseIcecastHeader(char *http_header) {
	char *p2 = 0;
	char *p3 = 0;
	char	tmpBuf[1024] = "";
	char	icyName[1024] = "";
	char	icyMetaIntstr[1024] = "";
	int	icyMetaInt = 0;



	p2 = strstr(http_header, "icy-name:");
	if (p2) {
		p2 = p2 + strlen("icy-name:");
		p3 = strstr(p2, "\r\n");
		if (p3) {
			memset(icyName, '\000', sizeof(icyName));
			strncpy(icyName, p2, p3-p2);
		}
		if (globals.inputStreamNameCallback) {
			globals.inputStreamNameCallback((char *)icyName);
		}
	}
	p2 = strstr(http_header, "Content-Type: ");
	if (p2) {
		p2 = p2 + strlen("Content-Type: ");
		p3 = strstr(p2, "\r\n");
		if (p3) {
			memset(tmpBuf, '\000', sizeof(tmpBuf));
			strncpy(tmpBuf, p2, p3-p2);
			if (!strcmp(tmpBuf, "application/x-ogg")) {
				formatFlag = OGG_FORMAT;
				chunkSize = 8192;
				if (globals.inputStreamTypeCallback) {
					globals.inputStreamTypeCallback((char *)"Vorbis");
				}
				cbuffer_init(&circularBuffer, 8196*20);	//JCMOD, made this a lot larger

#ifdef _WIN32
				DWORD dwThreadId = 0;
				HANDLE hThread = NULL;
				hThread = CreateThread(
									NULL,                  // Default thread security descriptor
									0,                     // Default stack size
									startDecodingVorbisWin32,  // Start routine
									0,                 // Start routine parameter
									0,                     // Run immediately
									&dwThreadId            // Thread ID
									);
#else
				pthread_t	startThread = 0;
				pthread_create(&startThread, NULL, &startDecodingVorbis, NULL);
#endif
			}
			else {
				formatFlag = MP3_FORMAT;
				chunkSize = 8192;
				if (globals.inputStreamTypeCallback) {
					globals.inputStreamTypeCallback((char *)"MP3");
				}
			}
		}
	}
	if (globals.inputServerTypeCallback) {
		globals.inputServerTypeCallback((char *)"Icecast/Icecast2");
	}
	initializeencoder();

		if (globals.mainStatusCallback) {
			globals.mainStatusCallback((void *)"Transcoding......");
		}

		if (globals.inputStatusCallback) {
			globals.inputStatusCallback((void *)"Connected");
		}

}

void parseShoutcastHeader(char *http_header) {
	char *p2 = 0;
	char *p3 = 0;
	char	icyName[1024] = "";
	char	icyMetaIntstr[1024] = "";
	int	icyMetaInt = 0;


	p2 = strstr(http_header, "icy-name:");
	if (p2) {
		p2 = p2 + strlen("icy-name:");
		p3 = strstr(p2, "\r\n");
		if (p3) {
			memset(icyName, '\000', sizeof(icyName));
			strncpy(icyName, p2, p3-p2);
		}
	}
	p2 = strstr(http_header, "icy-metaint:");
	if (p2) {
		p2 = p2 + strlen("icy-metaint:");
		p3 = strstr(p2, "\r\n");
		if (p3) {
			icyMetaInt = 0;
			memset(icyMetaIntstr, '\000', sizeof(icyMetaIntstr));
			strncpy(icyMetaIntstr, p2, p3-p2);
			icyMetaInt = atoi(icyMetaIntstr);
			metaDataFlag = 1;
		}
	}
	if (metaDataFlag) {
		chunkSize = icyMetaInt;
	}
	else {
		chunkSize = 8192*2;
		chunkSize = 8192;
	}

	serverFormatFlag = SERVER_FORMAT_SHOUTCAST;
	formatFlag = MP3_FORMAT;

	LogMessage(LOG_DEBUG, "Stream: (%s)\nMetaInterval: (%d)\n", icyName, icyMetaInt);
	if (globals.inputStreamNameCallback) {
		globals.inputStreamNameCallback((char *)icyName);
	}
	if (globals.inputServerTypeCallback) {
		globals.inputServerTypeCallback((char *)"Shoutcast");
	}
	if (globals.inputStreamTypeCallback) {
		globals.inputStreamTypeCallback((char *)"MP3");
	}
	cbuffer_init(&circularBuffer, chunkSize*20);	//JCMOD, made this a lot larger
	mp3dec_init(&(transglobal.mp3dec));
	initializeencoder();
		if (globals.mainStatusCallback) {
			globals.mainStatusCallback((void *)"Transcoding......");
		}

		if (globals.inputStatusCallback) {
			globals.inputStatusCallback((void *)"Connected");
		}

}

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int	processHeader = 0;

	int 	realsize = size*nmemb;
	char	*realptr = (char *)ptr;
	char*	pStreamData;
	char*	pMetaData;
	char c;
	char	*pStreamTitle = 0;
	char	*pEndStreamTitle = 0;
	char	StreamTitle[8192] = "";
	int	ret = 0;


	if (globals.gStopFlag) {
		disconnectFromServer();
		return 0;
	}
	// If we haven't read the header yet, and we get into write_data, then
	// this means that we are dealing with a Shoutcast-style server because Icecast style
	// servers actually contain a HTTP header which is caught by header_function
	if (!header_read) {
		if (globals.inputStatusCallback) {
			globals.inputStatusCallback((void *)"Reading input header...");
		}
		char *p1;
		p1 = (char *)strstr((char *)realptr, "\r\n\r\n");
	
		if (p1 == NULL) {
			strncat(httpHeader, realptr, size*nmemb);
			processHeader = 0;
			return nmemb;
		}

		p1 = p1 + strlen("\r\n\r\n");
		// Complete header read
		strncat(httpHeader, realptr, p1-realptr);
		realsize = realsize - (p1 - realptr);
		realptr = (char *)(realptr + (p1 - realptr));
		parseShoutcastHeader(httpHeader);
		header_read = 1;
		if (realsize == 0) {
			return nmemb;
		}
	}


	if (header_read) {
		if (cbuffer_insert(&circularBuffer, realptr, realsize) == BUFFER_FULL)
			assert(0);

		if (formatFlag == OGG_FORMAT) {
			if (globals.readBytesCallback) {
				globals.readBytesCallback((void *)((long)realsize));
			}
			return nmemb;
		}

		if (cbuffer_get_used(&circularBuffer) >= (unsigned long)(chunkSize*2)) {
			pStreamData = (char*)alloca(chunkSize);
			assert(pStreamData != NULL);
			LogMessage(LOG_DEBUG, "Extracting %d from the circular buffer into %x\n", 
					chunkSize, pStreamData);

			cbuffer_extract(&circularBuffer, pStreamData, chunkSize);
			if (metaDataFlag) {
				cbuffer_extract(&circularBuffer, &c, 1);
				LogMessage(LOG_DEBUG, "Metadata size %d\n", c*16);
				if (c > 0) {
					// We have metadata, lets read it!
					pMetaData = (char*)alloca(c*16);
					memset(pMetaData, '\000', sizeof(pMetaData));
					cbuffer_extract(&circularBuffer, pMetaData, c*16);
					pStreamTitle = strchr(pMetaData, '\'');
					if (pStreamTitle) {
						pEndStreamTitle = strstr(pStreamTitle+1, "';");
						if (pEndStreamTitle) {
							memset(StreamTitle, '\000', sizeof(StreamTitle));
							pStreamTitle++;
							strncpy(StreamTitle, pStreamTitle, pEndStreamTitle-pStreamTitle);
							int songChange = setCurrentSongTitle(StreamTitle);
							if (songChange) {
								LogMessage(LOG_INFO, "Metadata = (%s)\n", StreamTitle);
								if (globals.inputMetadataCallback) {
									globals.inputMetadataCallback((char *)StreamTitle);
								}
							}
						}
					}
				}
				if (c == 0) {
					//logDebug("No Metadata!\n");
				}
				if (c < 0) {
					LogMessage(LOG_DEBUG, "Metadata ERROR!\n");
					return 0;
				}
			}
			if (globals.gdumpData) {
				fwrite(pStreamData, 1, chunkSize, globals.outFromServer);
				fflush(globals.outFromServer);
			}
			int ret = 1;
			switch (formatFlag) {
				case MP3_FORMAT:		
					ret = sendToMP3Decoder(pStreamData, chunkSize);
					break;
				case OGG_FORMAT:		
					break;
			}
			if (ret == 0) {
				return 0;
			}

		}
		else {
			//LogMessage(LOG_DEBUG, "Buffering...(%d) (%d)", cbuffer_get_used(&circularBuffer),chunkSize*3 );
		}
	}
	if (globals.readBytesCallback) {
		globals.readBytesCallback((void *)((long)size*(long)nmemb));
	}
	return nmemb;
}

CURL *curl_handle = NULL;

int retrieveURL(char *url) {
	struct curl_slist *headerlist=NULL;
	int loop = 1;

	while (loop) {
		memset(httpHeader, '\000', sizeof(httpHeader));

		if (!connectToServer()) {
			if (globals.mainStatusCallback) {
				globals.mainStatusCallback((void *)"Failed Connecting To Output Stream");
			}
			if (globals.outputStatusCallback) {
				//globals.outputStatusCallback((void *)"Failed");
			}
			return(0);
		}

		// Set callbacks for oddcast
		//setCallback(BYTES_PER_SECOND, bytesPerSec);

		if (!strcmp(url, "linein")) {
			if (!initLiveRecording()) {
				if (globals.inputStatusCallback) {
					globals.inputStatusCallback((void *)"Fail to open input device");
				}
			}
			else {
				if (globals.inputStatusCallback) {
					globals.inputStatusCallback((void *)"Input device opened...");
				}

				if (globals.inputStatusCallback) {
					globals.inputStatusCallback((void *)"Recording from linein");
				}
				if (globals.inputServerTypeCallback) {
					globals.inputStatusCallback((void *)"Line-In");
				}
				if (globals.inputStreamTypeCallback) {
					globals.inputStreamTypeCallback((void *)"PCM");
				}
				if (globals.inputStreamTypeCallback) {
					globals.inputStreamTypeCallback((void *)"PCM");
				}
				char	buf[255] = "";
				sprintf(buf, "%dHz/Stereo PCM", getLiveInSamplerate());

				if (globals.inputBitrateCallback) {
					globals.inputBitrateCallback(buf);
				}
				int songChange = setCurrentSongTitle(getServerDesc());
				setSourceDescription(buf);
	//			initializeencoder();
				startRecording();

#ifdef WIN32
				while (getLiveRecordingFlag()) {
					if (globals.gStopFlag) {
						disconnectFromServer();
						return 0;
					}
					Sleep(500);
				}
#endif
			}
		}
		else {
			headerlist = curl_slist_append(headerlist, "Icy-MetaData:1");

			/* init the curl session */
			if (curl_handle == NULL) {
				curl_handle = curl_easy_init();
			}

			/* set URL to get */
			curl_easy_setopt(curl_handle, CURLOPT_URL, url);

			curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headerlist);
			curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Winamp/2.x");

			/* send all data to this function  */
			curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
			curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, header_function);

			if (globals.inputStreamURLCallback) {
				globals.inputStreamURLCallback(url);
			}
			if (globals.mainStatusCallback) {
				globals.mainStatusCallback((void *)"Connecting To Input");
			}
			if (globals.inputStatusCallback) {
				globals.inputStatusCallback((void *)"Connecting...");
			}

			reset_transcoder();

			curl_easy_perform(curl_handle);

			disconnectFromServer();
			cbuffer_destroy(&circularBuffer);
			/* cleanup curl stuff */
			curl_easy_cleanup(curl_handle);
			curl_handle = NULL;
		}

		if (globals.gStopFlag) {
			loop = 0;
		}
		else {
			if (getReconnectFlag()) {
				if (globals.inputStatusCallback) {
					globals.inputStatusCallback((void *)"Disconnected from input..reconnecting");
				}
				int seconds = getReconnectSecs();
#ifdef WIN32
				Sleep(seconds*1000);
#else
				sleep(seconds);
#endif
				loop = 1;
			}
			else {
				if (globals.inputStatusCallback) {
					globals.inputStatusCallback((void *)"Disconnected...");
				}
				loop = 0;
			}
		}
	}

	if (globals.gdumpData) {
		fclose(globals.outFromServer);
		fclose(globals.outToEncoder);
		fclose(globals.outToServer);
	}

	if (globals.mainStatusCallback) {
		globals.mainStatusCallback((void *)"Done");
	}
	if (globals.gStopFlag) {
		return 0;
	}

	return 0;
}


