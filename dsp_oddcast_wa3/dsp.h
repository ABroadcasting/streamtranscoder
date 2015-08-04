#ifndef _DSP_H
#define _DSP_H

#include <servicei.h>
#include <svc_mediaconverter.h>

class DSP : public svc_mediaConverterI {
public:
  DSP();
  virtual ~DSP();

  //Service Name.
  static const char *getServiceName() { return "Oddcast DSP"; } //CHANGE ME!!

  //The format we accept.
  virtual int canConvertFrom(svc_fileReader *reader, const char *name, const char *chunktype) { 
    if(chunktype && !STRICMP(chunktype,"pcm")) return 1; // only accept PCM chunks.
    return 0;
  }

  //The format we are returning.
  virtual const char *getConverterTo() { return "PCM"; }

  //Where we do the magic things with our PCM data.
  virtual int processData(MediaInfo *infos, ChunkList *chunk_list, bool *killswitch);

  //Our latency.
  virtual int getLatency(void) { return 0; }

  //Media Information. Not used in the case of a DSP.
  virtual int getInfos(MediaInfo *infos) { return 0; };
  
};

#endif