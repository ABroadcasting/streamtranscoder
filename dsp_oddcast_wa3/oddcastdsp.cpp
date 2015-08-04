#include "log.h"
#include "liboddcast.h"
#include "oddcastdsp.h" //CHANGE ME!!
#include "dsp.h"
#include "oddcastwnd.h"
#include "bfc/wndcreator.h"




//CHANGE ME!! (Use GUIDGEN.EXE, comes with MSVC++)
// {9281E42C-C58E-4425-8BC2-ABB446C13297}
static const GUID oddcast_dsp_guid = 
{ 0x9281e42c, 0xc58e, 0x4425, { 0x8b, 0xc2, 0xab, 0xb4, 0x46, 0xc1, 0x32, 0x97 } };

static WACNAME oddcast_dsp_wac;  //CHANGE ME!!
WAComponentClient *the = &oddcast_dsp_wac; //CHANGE ME!!

WACNAME::WACNAME() : WACPARENT("Oddcast DSP") {  //CHANGE ME!!
  // 1) we are going to make a window,
  registerService(new WndCreateCreatorSingle< CreateWndByGuid</**/OddcastWnd/*EDITME*/> >);

  // 2) and have it be listed in the Thinger,
  registerService(new WndCreateCreatorSingle< CreateBucketItem</**/OddcastWnd/*EDITME*/> >);

  // 3) and in the main context menu.
  registerAutoPopup(getGUID(), getName());

  registerService(new waServiceT<svc_mediaConverter, DSP > );
  
  // Initialize Oddcast Globals
  initializeGlobals();

}

WACNAME::~WACNAME() {

}

GUID WACNAME::getGUID() {
  return oddcast_dsp_guid; //CHANGE ME!!
}



_bool dsp_enabled("Enable Dsp", 1);

void WACNAME::onRegisterServices() {
  registerAttribute(&dsp_enabled);
}