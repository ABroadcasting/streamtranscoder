// Mp3Decoder.h: interface for the CMp3Decoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGGVDECODER_H__F97A547C_9B74_439D_A931_D4044B99A6BE__INCLUDED_)
#define AFX_OGGVDECODER_H__F97A547C_9B74_439D_A931_D4044B99A6BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vorbis/codec.h>

#define ERR_OK	0
#define OGGVBUFSIZE (0x100000)	// 1M how much space we can hold
#define ERR_OGGVDECODER_BUFFER_FULL -1
#define MIN_OGGV_STREAM_SIZE	4096 // 64k - how much we should process at a time

typedef struct tagOGGV_DECODE_ST
{
	char m_poggVBuf[OGGVBUFSIZE];
	long m_oggVBufPos;
	ogg_sync_state   oy; /* sync and verify incoming physical bitstream */
	ogg_stream_state os; /* take physical pages, weld into a logical stream of packets */
	ogg_page         og; /* one Ogg bitstream page.  Vorbis packets are inside */
	ogg_packet       op; /* one raw packet of data for decode */
	vorbis_info      vi; /* struct that stores all the static vorbis bitstream settings */
	vorbis_comment   vc; /* struct that stores all the bitstream user comments */
	vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
	vorbis_block     vb; /* local working space for packet->PCM decode */
	int	sampleRate;
	long	bitRate;

	int	nch;
	long	length;
	float **pcm;
} OGGV_DECODE_ST;

// Prototypes
#ifdef __cplusplus
extern "C" {
#endif
void * startDecodingVorbis(void *dummy);
#ifdef __cplusplus
}
#endif


#endif // !defined(AFX_OGGVDECODER_H__F97A547C_9B74_439D_A931_D4044B99A6BE__INCLUDED_)
