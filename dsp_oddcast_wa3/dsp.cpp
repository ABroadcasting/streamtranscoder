#include "log.h"
//#include "transcurl.h"
#include "liboddcast.h"

#include "dsp.h"
#include <mad.h>
#include "../../attribs/attribs.h"
#include "oddcastwnd.h"

extern _bool dsp_enabled;

extern	long	gAvgLeft;
extern	long	gAvgRight;
extern OddcastWnd *pOddcast;
#define myThirdTimer 103

/**
  Nothing to do in the constructor.
**/
DSP::DSP() {
}

/**
  Nothing to do in the destructor.
**/
DSP::~DSP() {
	int a = 1;
}


static signed int scale(int sample)
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


/**
  Replace my lame volume conrol routine with whatever
  manipulation you want to do on the PCM data.
**/
DSP::processData(MediaInfo *infos, ChunkList *chunk_list, bool *killswitch) {
  if(!dsp_enabled) return 1;

  if (getIsConnected()) {
	  if (!getLiveRecordingFlag()) {
		  Chunk* chunk = chunk_list->getChunk("PCM");

		  if(!chunk) return 1;
  
		  unsigned int c_size = chunk->getSize();  //in bytes.
		  if (c_size == 0) {
			  return 1;
		  }
		  void * d = chunk->getData();
		  if (!d) {
			  return 1;
		  }
		  short * z = (short *)d;   //signed short for pcm data

		  /**
			I know i don't actually use these, but it's to show you how to read that type of info
		  **/
		  int srate = chunk->getInfo("srate");  //sample rate
		  int bps = chunk->getInfo("bps");  //bits per sample
		  int nch = chunk->getInfo("nch");  //number of channels
		  int numsamples = c_size/sizeof(short);

		  float	*samples;

		  samples = (float *)malloc(numsamples*sizeof(float));


		  long avgLeft = 0;
		  long avgRight = 0;
		  int flip = 0;

		  memset(samples, '\000', numsamples*sizeof(float));

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
		
		  /*
		  memset(&gSampleData, '\000', sizeof(gSampleData));


		  gSampleData.samples = (float *)&samples;
		  gSampleData.nch = nch;
		  gSampleData.numsamples = numsamples;
		  gSampleData.srate = srate;
  		  
		  HANDLE	ThreadHandle = 0;
		  DWORD	ThreadID = 0;

		  ThreadHandle = CreateThread(NULL, 0, handleOutputThread, (void *)&gSampleData, 0, &ThreadID);
		  */
		  int ret = handle_output((float *)samples, numsamples/nch, nch, srate);
		  if (ret == 0) {
			  if (getReconnectFlag()) {
				  if (pOddcast) {
					  int duration = getReconnectSecs()*1000;
					  pOddcast->setTimer(myThirdTimer, duration);
				  }
			  }
		  }
	  }
  }
  return 1;
}
