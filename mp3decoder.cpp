// Mp3Decoder.cpp: implementation of the CMp3Decoder class.
//
//////////////////////////////////////////////////////////////////////

#include "mp3decoder.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <mad.h>	//mpeg library
#ifdef __cplusplus
}
#endif
#ifdef _WIN32
#include <process.h>
#endif
#include <math.h>
#include <assert.h>

#include "liboddcast.h"
#include "log.h"

#define READSIZE	2000	// JCBUG i forget if we need a READSIZE, and if so, why this size

extern FILE	*outToEncoder;

///////////////////////////////////////////////////
// Private methods

void mp3dec_init(MP3_DECODE_ST* pdecode)
{
	assert(pdecode);
	pdecode->m_mp3BufPos = 0;
	mad_stream_init(&pdecode->stream);
	mad_frame_init(&pdecode->frame);
	mad_synth_init(&pdecode->synth);

}
long mp3dec_feed_stream(MP3_DECODE_ST* pdecode, char *pdata, long size)
{
	assert(pdecode);
	LogMessage(LOG_DEBUG, "feed_stream: size=%d, mp3pos=%d", size, pdecode->m_mp3BufPos);
	if (pdecode->m_mp3BufPos+size > MP3BUFSIZE)
	{
		assert(0);
		return ERR_MP3DECODER_BUFFER_FULL;
	}

	memcpy(&pdecode->m_pmp3Buf[pdecode->m_mp3BufPos], pdata, size);
	pdecode->m_mp3BufPos += size;
	return ERR_OK;
}

long mp3dec_get_stream_size(MP3_DECODE_ST* pdecode)
{
	return pdecode->m_mp3BufPos;
}

static signed int scale(mad_fixed_t sample)
{
	// round 
	sample += (1L << (MAD_F_FRACBITS - 16));

	// clip 
	if (sample >= MAD_F_ONE)
		sample = MAD_F_ONE - 1;
	else if (sample < -MAD_F_ONE)
		sample = -MAD_F_ONE;

	// quantize 
	return sample >> (MAD_F_FRACBITS + 1 - 16);
}



int handle_mp3_output(MP3_DECODE_ST *pdecode)
{
	unsigned int nchannels, nsamples;
	float	samples[8196];
	struct mad_pcm *pcm;

	mad_fixed_t const *left_ch, *right_ch;
	static short lastSample = 0;
	static signed int sample;

	pcm = &pdecode->synth.pcm;

	nchannels = pcm->channels;
	nsamples  = pcm->length;
	left_ch   = pcm->samples[0];
	right_ch  = pcm->samples[1];

	long	in_samplerate = pdecode->sampleRate;
	long	out_samplerate = getCurrentSamplerate();
	long	in_nch = pdecode->nch;
	long	out_nch = getCurrentChannels();

	// Send it to liboddcast (cross fingers)
	LogMessage(LOG_DEBUG, "Read %d/%d/%d - Sending %d/%d/%d", pdecode->bitRate/1000, pdecode->sampleRate, pdecode->nch, getCurrentBitrate(), getCurrentSamplerate(), getCurrentChannels());
	int	samplecount = 0;
	for (int i=0;i<nsamples;i++) {
		signed int sample;

		sample = scale(*left_ch++);
	
		samples[samplecount] = sample/32767.f;
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
			sample = scale(*right_ch++);
			samples[samplecount] = sample/32767.f;
			// clipping
			if (samples[samplecount] > 1.0) {
				samples[samplecount] = 1.0;
			}
			if (samples[samplecount] < -1.0) {
				samples[samplecount] = -1.0;
			}
			samplecount++;
		}
		else {
			//samples[samplecount] = 0.0;
			//samplecount++;
		}
	}
	LogMessage(LOG_DEBUG, "handle_output - nsamples = %d, nchannels = %d, in_samplerate = %d, in_nch = %d", nsamples, nchannels, in_samplerate, in_nch);
	return handle_output((float *)&samples, nsamples, nchannels, in_samplerate);

}


int mp3dec_decode(MP3_DECODE_ST* pdecode)
{
	struct mad_stream* pstream = &pdecode->stream;
	struct mad_frame* pframe = &pdecode->frame;
	struct mad_synth* psynth = &pdecode->synth;
	int ret = 1;

	mad_stream_buffer(pstream, (const unsigned char*)pdecode->m_pmp3Buf, pdecode->m_mp3BufPos);
	
	while(1)
	{
		long ret = mad_frame_decode(pframe, pstream);
		if (pstream->error != MAD_ERROR_NONE) {
			if (pstream->error == MAD_ERROR_BUFLEN ||
				!MAD_RECOVERABLE(pstream->error))
				break;
			//LogMessage(LOG_ERROR, "pstream Error: 0x%x", pstream->error);
			//pstream->error = MAD_ERROR_NONE;
		}
		
		if (pstream->error == MAD_ERROR_NONE) {
			mad_synth_frame(psynth, pframe);
			pdecode->sampleRate = pframe->header.samplerate;
			pdecode->bitRate = pframe->header.bitrate;
			switch (pframe->header.mode) {
				case MAD_MODE_SINGLE_CHANNEL:
					pdecode->nch = 1;
					break;
				case MAD_MODE_DUAL_CHANNEL:
				case MAD_MODE_JOINT_STEREO:
				case MAD_MODE_STEREO:
					pdecode->nch = 2;
					break;
			}
			ret = handle_mp3_output(pdecode);
		}
	}

	pstream->error = MAD_ERROR_NONE;
	
	if (!pstream->next_frame) 
		assert(0);

	long bufferleft = (long)&(pdecode->m_pmp3Buf[pdecode->m_mp3BufPos]) - 
					  (long)pstream->next_frame;
	assert(bufferleft > 0);

	// move the mp3buf to the begining
	memmove(pdecode->m_pmp3Buf, 
			&pdecode->m_pmp3Buf[pdecode->m_mp3BufPos-bufferleft], 
			bufferleft);
	pdecode->m_mp3BufPos = bufferleft;
	LogMessage(LOG_DEBUG, "---------");

	return ret;
}

