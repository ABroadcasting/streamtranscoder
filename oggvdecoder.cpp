// Mp3Decoder.cpp: implementation of the CMp3Decoder class.
//
//////////////////////////////////////////////////////////////////////

#include "oggvdecoder.h"
#ifdef _WIN32
#include <process.h>
#else
#include <pthread.h>
#endif
#include <math.h>
#include <assert.h>

#include "liboddcast.h"
#include "log.h"
#include "transcode.h"
#include "transcurl.h"
#include "cbuffer.h"


extern transcodeGlobals	globals;
extern int	decode_pipe[2];
OGGV_DECODE_ST 	decode_struct;
extern CBUFFER circularBuffer;



/* Takes a vorbis bitstream from stdin and writes raw stereo PCM to
   stdout using vorbisfile. Using vorbisfile is much simpler than
   dealing with libvorbis. */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#ifdef _WIN32 /* We need the following two to set stdin/stdout to binary */
#include <io.h>
#include <fcntl.h>
#endif

float	**pcmout;
int handle_vorbis_output(float **pcm, int nsamples);

size_t	vorbisReadFromPipe(void *ptr, size_t size, size_t nmemb, void *stream) {
	int ret = 0;
	//fprintf(stdout, "Going to Read %d bytes from the pipe\n", size*nmemb);
	while (cbuffer_get_used(&circularBuffer) < size*nmemb) {
#ifdef _WIN32
		Sleep(250);
#else
		usleep(250);
#endif
	}
	if (cbuffer_extract(&circularBuffer, (char *)ptr, size*nmemb)) {
		return size*nmemb;
	}
	else {
		return 0;
	}
}
int	vorbisSeekPipe(void *stream, ogg_int64_t offset, int whence) {
	return -1;
}
int	vorbisClosePipe(void *stream) {
	return 1;
}
long	vorbisTellPipe(void *stream) {
	return -1;
}

void	updateComments(OggVorbis_File	*vf) {
	char	StreamTitle[2046 + 25] = "";
	char	Artist[1024] = "";
	char	Title[1024] = "";

	memset(StreamTitle, '\000', sizeof(StreamTitle));
	memset(Artist, '\000', sizeof(Artist));
	memset(Title, '\000', sizeof(Title));

	char **ptr=ov_comment(vf,-1)->user_comments;

	vorbis_info *vi=ov_info(vf,-1);
	while(*ptr){
		char *pData = *ptr;
		if (!strncmp(pData, "ARTIST=", strlen("ARTIST="))) {
			strncpy(Artist, pData + strlen("ARTIST="), sizeof(Artist)-1);
		}
		if (!strncmp(pData, "TITLE=", strlen("TITLE="))) {
			strncpy(Title, pData + strlen("TITLE="), sizeof(Title)-1);
		}
		++ptr;
	}
	decode_struct.sampleRate = vi->rate;
	decode_struct.nch = vi->channels;

	LogMessage(LOG_DEBUG,"Encoded by: %s",ov_comment(vf, -1)->vendor);
	sprintf(StreamTitle, "%s - %s", Artist, Title);

	LogMessage(LOG_DEBUG, "StreamTitle = (%s)", StreamTitle);
	int songChange = setCurrentSongTitle(StreamTitle);
	if (songChange) {
		if (globals.inputMetadataCallback) {
			globals.inputMetadataCallback((char *)StreamTitle);
		}
	}
}

OggVorbis_File vf;

void * startDecodingVorbis(void *dummy) {
	int eof=0;
	int current_section;
	char	StreamTitle[1024] = "";
	char	Artist[1024] = "";
	char	Title[1024] = "";
	int	old_section = -1;
	static	int runOnce = 0;

	ov_callbacks callbacks = {
		(size_t (*)(void *, size_t, size_t, void *))  vorbisReadFromPipe,
		(int (*)(void *, ogg_int64_t, int))              vorbisSeekPipe,
		(int (*)(void *))                             vorbisClosePipe,
		(long (*)(void *))                            vorbisTellPipe
	};


	if (!runOnce) {
		// we don't really read from stdin, we read from the pipe :)
		if(ov_open_callbacks(stdin, &vf, NULL, 0, callbacks) < 0) {
			if (globals.mainStatusCallback) {
				globals.mainStatusCallback((void *)"Input does not appear to be an Ogg bitstream.");
				eof = 1;
				runOnce = 0;
			}
		}
		else {
			runOnce = 1;
		}
		
	}

	updateComments(&vf);

	LogMessage(LOG_DEBUG, "Going into EOF loop");
	current_section = 0;
	while(!eof){
		long ret=ov_read_float(&vf,&pcmout,1024, &current_section);
		LogMessage(LOG_DEBUG, "Read %d ret from ov_read", ret);
		if (ret == 0) {
			/* EOF */
			eof=1;
		} else if (ret < 0) {
			;
		} else {
			handle_vorbis_output(pcmout, ret);
		}
		if (current_section != old_section) {
			if (old_section != -1) {
				updateComments(&vf);
			}
			old_section = current_section;
		}
	}

	/* cleanup */
//	ov_clear(&vf);
#ifdef _WIN32
	ExitThread(1);
#else
	pthread_exit((void *)1);
#endif
	return 0;
}



///////////////////////////////////////////////////
// Private methods

int handle_vorbis_output(float **pcm, int nsamples)
{
	unsigned int nchannels;
	float	samples[8196];

	float const *left_ch, *right_ch;
	static short lastSample = 0;
	static float sample;

	nchannels = decode_struct.nch;
	left_ch   = pcm[0];
	right_ch  = pcm[1];

	long	in_samplerate = decode_struct.sampleRate;
	long	out_samplerate = getCurrentSamplerate();
	long	in_nch = decode_struct.nch;
	long	out_nch = getCurrentChannels();

	// Send it to liboddcast (cross fingers)
	int	samplecount = 0;
	LogMessage(LOG_DEBUG, "Sending it to oddcast");
	for (int i=0;i<nsamples;i++) {
		float sample;

		sample = *left_ch++;
		pcm++;
	
		samples[samplecount] = sample;
		// clipping
		if (samples[samplecount] > 1.0) {
			samples[samplecount] = 1.0;
		}
		if (samples[samplecount] < -1.0) {
			samples[samplecount] = -1.0;
		}
		samplecount++;

		if (nchannels == 2) 
		{
			sample = *right_ch++;
			pcm++;
			samples[samplecount] = sample;
			// clipping
			if (samples[samplecount] > 1.0) {
				samples[samplecount] = 1.0;
			}
			if (samples[samplecount] < -1.0) {
				samples[samplecount] = -1.0;
			}
			samplecount++;
		}
	}
	LogMessage(LOG_DEBUG, "handle_output - nsamples = %d, nchannels = %d, in_samplerate = %d, in_nch = %d", nsamples, nchannels, in_samplerate, in_nch);
	return handle_output((float *)&samples, nsamples, nchannels, in_samplerate);

}
