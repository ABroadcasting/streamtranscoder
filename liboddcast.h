#ifndef __DSP_ODDCAST_H
#define __DSP_ODDCAST_H

#include "Socket.h"
#include <vorbis/vorbisenc.h>

#include <stdio.h>

#ifdef _DMALLOC_
#include <dmalloc.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "resample.h"
#ifdef __cplusplus
}
#endif

#include "wmaenc.h"

#ifdef WIN32
#include <BladeMP3EncDLL.h>
#else
#include <lame/lame.h>
#endif

#define FormatID 'fmt '   /* chunkID for Format Chunk. NOTE: There is a space at the end of this ID. */
// For skin stuff
#define WINDOW_WIDTH		276
#define WINDOW_HEIGHT		150

#ifndef FALSE
#define FALSE false
#endif
#ifndef TRUE
#define TRUE true
#endif
#ifndef WIN32
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#else
#include <mmreg.h>
#include <mmsystem.h>
#endif
// Callbacks
#define	BYTES_PER_SECOND 1

typedef struct tagPCMData {
	float	*samples;
	int			numsamples;
	int			nch;
	int			srate;
} PCMData;

typedef struct tagLAMEOptions {
	int		cbrflag;
	int		out_samplerate;
	int		quality;
#ifdef WIN32
	int		mode;
#else
	MPEG_mode	mode;
#endif
	int		brate;
	int		copywrite;
	int		original;
	int		strict_ISO;
	int		disable_reservoir;
	char		VBR_mode[25];
	int		VBR_mean_bitrate_kbps;
	int		VBR_min_bitrate_kbps;
	int		VBR_max_bitrate_kbps;
	int		lowpassfreq;
	int		highpassfreq;
} LAMEOptions;

typedef struct {
	char	RIFF[4];
	long	chunkSize;
	char	WAVE[4];
} RIFFChunk;

typedef struct {
  char		chunkID[4];
  long		chunkSize;

  short          wFormatTag;
  unsigned short wChannels;
  unsigned long  dwSamplesPerSec;
  unsigned long  dwAvgBytesPerSec;
  unsigned short wBlockAlign;
  unsigned short wBitsPerSample;

/* Note: there may be additional fields here, depending upon wFormatTag. */

} FormatChunk;


typedef struct {
	char	chunkID[4];
	long	chunkSize;
	short *	waveformData;
} DataChunk;

struct wavhead
{
	unsigned int  main_chunk;
	unsigned int  length;
	unsigned int  chunk_type;
	unsigned int  sub_chunk;
	unsigned int  sc_len;
	unsigned short  format;
	unsigned short  modus;
	unsigned int  sample_fq;
	unsigned int  byte_p_sec;
	unsigned short  byte_p_spl;
	unsigned short  bit_p_spl;
	unsigned int  data_chunk;
	unsigned int  data_length;
};

static struct wavhead	wav_header;


// Global variables....gotta love em...
typedef struct {
	long		currentSamplerate;
	int		currentBitrate;
	int		currentBitrateMin;
	int		currentBitrateMax;
	int		currentChannels;
	int		gSCSocket;
	int		gSCSocket2;
	int		gSCSocketControl;
	CMySocket	dataChannel;
	CMySocket	controlChannel;
	int		gSCFlag;
	int		gCountdown;
	int		gAutoCountdown;
	int		automaticconnect;
	char		gSourceURL[1024];
	char		gServer[256];
	char		gPort[10];
	char		gPassword[256];
	int		weareconnected;
	char		gIniFile[1024];
	char		gAppName[256];
	char		gCurrentSong[1024];
	int			gSongUpdateCounter;
	char		gMetadataUpdate[10];
	int			gPubServ;
	char		gServIRC[20];
	char		gServICQ[20];
	char		gServAIM[20];
	char		gServURL[1024];
	char		gServDesc[1024];
	char		gServGenre[100];
	char		gMountpoint[100];
	char		gFrequency[10];
	char		gChannels[10];
	int			gAutoReconnect;
	int 		gReconnectSec;
	char		gAutoStart[10];
	char		gAutoStartSec[20];
	char		gQuality[5];
#ifndef WIN32
	lame_global_flags *gf;
#endif
	PCMData		sampleData;
	int		gCurrentlyEncoding;
	int		gOggFlag;
	char		gIceFlag[10];
	int		gLAMEFlag;
	char		gOggQuality[25];
	int			gLiveRecordingFlag;
	int		gOggBitQualFlag;
	char	gOggBitQual[40];
	char	gEncodeType[25];
	int		gAdvancedRecording;
	int		gNOggchannels;
	char		gModes[4][255];
	int		gShoutcastFlag;
	int		gIcecastFlag;
	int		gIcecast2Flag;
	char		gSaveDirectory[1024];
	char		gSaveDirectoryFlag[15];
	char		gSaveAsWAV[15];
	int		gSaveWAV;
	FILE		*gSaveFile;
	LAMEOptions	gLAMEOptions;
	int		gLAMEHighpassFlag;
	int		gLAMELowpassFlag;

	int		oggflag;
	FILE		*outogg;
	ogg_sync_state  oy_stream;
	int		serialno;
	ogg_packet header_main_save;
	ogg_packet header_comments_save;
	ogg_packet header_codebooks_save;
	bool		ice2songChange;
	int			in_header;
	long		 written;

	int		gLAMEpreset;
	char	gLAMEbasicpreset[255];
	char	gLAMEaltpreset[255];
	char    gSongTitle[1024];
	res_state	resampler;
	int	initializedResampler;
	void (*sourceURLCallback)(void *);
	void (*destURLCallback)(void *);
	void (*serverStatusCallback)(void *);
	void (*generalStatusCallback)(void *);
	void (*writeBytesCallback)(void *);
	void (*serverTypeCallback)(void *);
	void (*streamTypeCallback)(void *);
	void (*bitrateCallback)(void *);
	void (*VUCallback)(int, int);
	long	startTime;
	long	endTime;
	FILE	*outToServer;
	int		gdumpData;
	FILE       *outToEncoder;
	char	sourceDescription[255];

	int		gWMAFlag;
	long	gWMABitrate;
	long	gWMASamplerate;
	long	gWMAClients;
	char	gWMAMode[25];
	char	gServerType[25];
	int		gWMAPort;
#ifdef WIN32
	WAVEFORMATEX waveFormat;
	HWAVEIN      inHandle;
	WAVEHDR                 WAVbuffer1;
	WAVEHDR                 WAVbuffer2;
#else
	int	inHandle; // for advanced recording
#endif
	unsigned long result;
	short int WAVsamplesbuffer1[1152*2];
	short int WAVsamplesbuffer2[1152*2];
	bool  areLiveRecording;
	char	gAdvRecDevice[255];
#ifndef WIN32
	char	gAdvRecServerTitle[255];
#endif
	int		gLiveInSamplerate;
#ifdef WIN32
	// These are for the LAME DLL
        BEINITSTREAM            beInitStream;
        BEENCODECHUNK           beEncodeChunk;
        BEDEINITSTREAM          beDeinitStream;
        BECLOSESTREAM           beCloseStream;
        BEVERSION               beVersion;
        BEWRITEVBRHEADER        beWriteVBRHeader;
        HINSTANCE       hDLL;
        DWORD           dwSamples;
        DWORD           dwMP3Buffer;
        HBE_STREAM      hbeStream;
#endif
		char	gConfigFileName[255];
		char	gOggEncoderText[255];
		int		gForceStop;
		char	gCurrentRecordingName[1024];

		ogg_stream_state os;
		vorbis_dsp_state vd;
		vorbis_block     vb;
		vorbis_info      vi;
} oddcastGlobals;



int initializeencoder();
void doUpdateSong();
void getCurrentSongTitle(char *song, char *artist, char *full);
void initializeGlobals();
int encodeit();
void ReplaceString(char *source, char *dest, char *from, char *to);
int initAdvancedRecording();
void splitOggBitrate(char *bitrate);
void	startRecording();
void	stopRecording();
void config_read();
void config_write();
int connectToServer();
int disconnectFromServer();
int do_encoding(short int *samples, int numsamples, int nch);
int ice2_send_data(unsigned char *buff, unsigned long len);
void URLize(char *input, char *output, int inputlen, int outputlen);
void updateSongTitle();
void icecast2SendMetadata();
int ogg_encode_dataout();
int	trimVariable(char *variable);
int readConfigFile();
int writeConfigFile();
void    printConfigFileValues();
void ErrorMessage(char *title, char *fmt, ...);
int setCurrentSongTitle(char *song);
char*   getSourceURL();
long    getCurrentSamplerate();
int     getCurrentBitrate();
int     getCurrentChannels();
int  ocConvertAudio(float *in_samples, float *out_samples, int num_in_samples, int num_out_samples);
int initializeResampler(long inSampleRate, long inNCH);
int handle_output(float *samples, int nsamples, int nchannels, int in_samplerate);
void setServerStatusCallback(void (*pCallback)(void *));
void setGeneralStatusCallback(void (*pCallback)(void *));
void setWriteBytesCallback(void (*pCallback)(void *));
void setServerTypeCallback(void (*pCallback)(void *));
void setStreamTypeCallback(void (*pCallback)(void *));
void setBitrateCallback(void (*pCallback)(void *));
void setVUCallback(void (*pCallback)(int, int));
void setSourceURLCallback(void (*pCallback)(void *));
void setDestURLCallback(void (*pCallback)(void *));
void setSourceDescription(char *desc);
void setoutToServer(FILE *filep);
int  getOggFlag();
bool  getLiveRecordingFlag();
void setLiveRecordingFlag(bool flag);
void setDumpData(int dump);
void startRecording();
void stopRecording();
int initLiveRecording();
int getLiveInSamplerate();
void setLiveInSamplerate(int rate);
int getWMAFlag();
void setConfigFileName(char *configFile);
char *getConfigFileName();
char*	getServerDesc();
int	getReconnectFlag();
int getReconnectSecs();
int getIsConnected();
int resetResampler();
void setOggEncoderText(char *text);
int getLiveRecordingSetFlag();
char *getCurrentRecordingName();
void setCurrentRecordingName(char *name);
void setForceStop(int forceStop);
#endif
