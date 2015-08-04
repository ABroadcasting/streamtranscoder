#ifndef _EXAMPLEDSP_H //CHANGE ME!!
#define _EXAMPLEDSP_H //CHANGE ME!!

#include "../../studio/wac.h"
#include "../../attribs/attribs.h"

#define WACPARENT WAComponentClient
#define WACNAME WACexampledsp //CHANGE ME!!

class WACNAME : public WACPARENT {
public:
  WACNAME();
  virtual ~WACNAME();

  virtual const char * getName() { return "Oddcast DSP for Winamp3"; };  //CHANGE ME!!!
  virtual GUID getGUID();

  virtual void onRegisterServices();
	


};

#endif