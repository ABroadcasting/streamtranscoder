/* oddcastwnd.c - oddsock@oddsock.org
 * copied and modified from sripperwnd.c jonclegg@yahoo.com
 * 
 * Portions from the ExampleVisData example from the Winamp3 SDK
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include "log.h"
//#include "transcurl.h"
#include "liboddcast.h"
#include "transcode.h"

#include "resource.h"
#include <assert.h>

#include "bfc/std.h"
#include "common/buttwnd.h"
#include "common/textbar.h"
#include "common/corehandle.h"
#include "bfc/notifmsg.h" 
#include "bfc/canvas.h"
#include "pledit/svc_pldir.h"
#include "pledit/playlist.h"
#include <svc_mediaconverter.h>
#include <servicei.h>
#include <wac.h>

extern WAComponentClient *the;

#include <string>

#include "oddcastwnd.h"

#define snprintf	_snprintf

long	gAvgLeft;
long	gAvgRight;

void UpdateWhatsPlaying();

#define myTimer  101
#define myTimerDuration  100
#define myThirdTimer 103

enum {
	// Core Buttons Tabsheet IDs
	ODDCAST_CONNECT = 1,
	ODDCAST_CONFIG,
	ODDCAST_VU,
};


int OC_WINDOW_WIDTH = 300;
int OC_WINDOW_HEIGHT = 175;
int	EXB_LEFT_BUTTON_OFFSET = 5;
int	EXB_TOP_BUTTON_OFFSET = OC_WINDOW_HEIGHT - 24;
int	EXB_BUTTON_WIDTH  = 50;
int	EXB_BUTTON_HEIGHT = 20;
int	EXB_BUTTON_HEIGHT_SMALL = 15;
int	EXB_BUTTON_FONT_SIZE = 14;
int	EXB_TEXT_FONT_SIZE = 12;

int	EXB_TOP_LABEL_OFFSET = 0;
int	EXB_LEFT_LABEL_OFFSET = 10;
int	EXB_LABEL_INC_AMOUNT = 12;
//int	EXB_LABEL_WIDTH = OC_WINDOW_WIDTH-EXB_LEFT_LABEL_OFFSET-20;
int	EXB_COLUMN1_WIDTH = 65;
int	EXB_COLUMN2_WIDTH = OC_WINDOW_WIDTH - EXB_LEFT_LABEL_OFFSET - EXB_COLUMN1_WIDTH;

#define NO_STREAM_MSG	"Winamp is not listening to a stream"


// static membor vars
//////////////////////////////////////////////////////////////
OddcastWnd* OddcastWnd::m_pcurInst = NULL;

OddcastWnd *pOddcast = NULL;


transcodeGlobals	globals;
char	*gConfigFile = NULL;

void outputStatusCallback(void *pValue) {
	// pValue is a string
	if (pOddcast) {
		pOddcast->m_StatusValue->setName((char *)pValue);
	}
}
void writeBytesCallback(void *pValue) {
	// pValue is a long
	static	long	startTime = 0;	
	static	long	endTime = 0;	
	static	long	bytesWrittenInterval = 0;
	char	kBPSstr[25] = "";
	static  char avgBPS[255] = "";
	static  incrementer = 0;
	char    spinner[10][2] = {"|", "/", "-", "\\", "|", "/", "-", "\\"};


	long	bytesWritten = (long)pValue;

	if (bytesWritten == -1) {
		sprintf(kBPSstr, "");
		globals.totalBytesWritten = 0;
//		pApp->setOutputBPSRead((char *)kBPSstr);
		startTime = 0;
		return;
	}
	if (startTime == 0) {
		startTime = time(&startTime);
		bytesWrittenInterval = 0;
	}
	bytesWrittenInterval += bytesWritten;
	globals.totalBytesWritten += bytesWritten;
	endTime = time(&endTime);
	if ((endTime - startTime) > 5) {
		int bytespersec = bytesWrittenInterval/(endTime - startTime);
		long kBPS = (bytespersec * 8)/1000;
		sprintf(avgBPS, "%d Kbps", kBPS);
//		if (pOddcast) {
//			pOddcast->m_BPSValue->setName((char *)avgBPS);
//		}
		startTime = 0;
		if (getLiveRecordingSetFlag()) {
			char	buf[255] = "";
			sprintf(buf, "Recording from %s %dHz/Stereo PCM", getCurrentRecordingName(),getLiveInSamplerate());

			pOddcast->m_LiveRecordingText->setName((char *)buf);
		}
		else {
			pOddcast->m_LiveRecordingText->setName((char *)"");
		}
	}

	if (incrementer > 7) {
		incrementer = 0;
	}
	sprintf(kBPSstr, "%s (%s avg) %s", spinner[incrementer], avgBPS, spinner[incrementer]);
	if (pOddcast) {
		pOddcast->m_BPSValue->setName((char *)kBPSstr);
	}
	incrementer++;
//	pApp->setOutputTotalRead((char *)kBPSstr);
}
void outputServerTypeCallback(void *pValue) {
	// pValue is a string
//	pApp->setOutputServerType((char *)pValue);
	char	buf[2046] = "";
	sprintf(buf, "%s Server : %s", (char *)pValue, getServerDesc());
	if (pOddcast) {
		pOddcast->m_ServerDescValue->setName((char *)buf);
	}
}
void outputStreamTypeCallback(void *pValue) {
	// pValue is a string
//	pApp->setOutputStreamType((char *)pValue);
}
void outputBitrateCallback(void *pValue) {
	// pValue is a string
//	pApp->setOutputBitrate((char *)pValue);
	if (pOddcast) {
		pOddcast->m_BitrateValue->setName((char *)pValue);
		if (getLiveRecordingSetFlag()) {
			char	buf[255] = "";
			sprintf(buf, "Recording Live Input %dHz/Stereo PCM", getLiveInSamplerate());

			pOddcast->m_LiveRecordingText->setName((char *)buf);
		}
		else {
			pOddcast->m_LiveRecordingText->setName((char *)"");
		}
	}
}
void outputStreamURLCallback(void *pValue) {
	// pValue is a string
//	pApp->setMainOutputURL((char *)pValue);

	if (pOddcast) {
		pOddcast->m_DestinationValue->setName((char *)pValue);
	}
}
void VUCallback(int leftMax, int rightMax) {
	// pValue is a string
//	pApp->setMainOutputURL((char *)pValue);

	if (pOddcast) {
		gAvgLeft = leftMax;
		gAvgRight = rightMax;
	}
}

int transcode_init()
{
	char	*serverURL = NULL;
	char	*defaultConfigFile = "transcoder.cfg";
	int 	c = 0;
	int	printConfig = 0;
	
	memset(&globals, '\000', sizeof(globals));

	// Setup Callbacks for all the interaction
	globals.mainStatusCallback = NULL;
	globals.inputStatusCallback = NULL;
	globals.inputStreamURLCallback = NULL;
	globals.readBytesCallback = NULL;
	globals.inputServerTypeCallback = NULL;
	globals.inputStreamTypeCallback = NULL;
	globals.inputStreamURLCallback = NULL;
	globals.inputStreamNameCallback = NULL;
	globals.inputMetadataCallback = NULL;
	globals.inputBitrateCallback = NULL;
	globals.outputStatusCallback = outputStatusCallback;
	globals.writeBytesCallback = writeBytesCallback;
	globals.outputServerTypeCallback = outputServerTypeCallback;
	globals.outputStreamTypeCallback = outputStreamTypeCallback;
	globals.outputBitrateCallback = outputBitrateCallback;
	globals.outputStreamURLCallback = outputStreamURLCallback;

	setServerStatusCallback(globals.outputStatusCallback);
	setGeneralStatusCallback(globals.mainStatusCallback);
	setWriteBytesCallback(globals.writeBytesCallback);
	setBitrateCallback(globals.outputBitrateCallback);
	setStreamTypeCallback(globals.outputStreamTypeCallback);
	setServerTypeCallback(globals.outputServerTypeCallback);
	setSourceURLCallback(globals.inputStreamURLCallback);
	setDestURLCallback(globals.outputStreamURLCallback);
	setVUCallback(VUCallback);

	if (gConfigFile == NULL) {
		gConfigFile = defaultConfigFile;
	}
	setConfigFileName(gConfigFile);
	readConfigFile();

	return 1;
}

// =========================================================================
//
//  ODDCAST DATA: Methods required by Window Creation Services
//
const char *OddcastWnd::getWindowTypeName() { return /**/"Oddcast DSP for Winamp3"/*EDITME*/; }
GUID OddcastWnd::getWindowTypeGuid() { return the->getGUID(); }
void OddcastWnd::setIconBitmaps(ButtonWnd *button) {
  // Note that IDB_TAB_XXXXXX defines come from resource.h and the data is in resource.rc
  button->setBitmaps(the->gethInstance(), IDB_TAB_NORMAL, NULL, IDB_TAB_HILITED, IDB_TAB_SELECTED);
}


OddcastWnd::OddcastWnd() 
: ODDCASTWND_PARENT() 
{  
	int	increment = 18;
	int rightColumn = EXB_COLUMN1_WIDTH + EXB_LEFT_LABEL_OFFSET;

	m_pcurInst = this;

	setName("Oddcast DSP for Winamp3");

	m_connect = new ButtonWnd();
	m_connect->setButtonText("Connect", EXB_BUTTON_FONT_SIZE);
	m_connect->setNotifyId(ODDCAST_CONNECT);
	m_connect->setNotifyWindow(this);
	addChild(m_connect, EXB_LEFT_BUTTON_OFFSET, EXB_TOP_BUTTON_OFFSET,
		EXB_BUTTON_WIDTH, EXB_BUTTON_HEIGHT, 0/*"Invalidate on Resize*/);


	m_config = new ButtonWnd();
	m_config->setButtonText("Config", EXB_BUTTON_FONT_SIZE);
	m_config->setNotifyId(ODDCAST_CONFIG);
	m_config->setNotifyWindow(this);
	addChild(m_config, OC_WINDOW_WIDTH-EXB_BUTTON_WIDTH-5, EXB_TOP_BUTTON_OFFSET,
		EXB_BUTTON_WIDTH, EXB_BUTTON_HEIGHT, 0/*"Invalidate on Resize*/);

	m_vu = new ButtonWnd();
	m_vu->setButtonText("VU on/off", EXB_BUTTON_FONT_SIZE);
	m_vu->setNotifyId(ODDCAST_VU);
	m_vu->setNotifyWindow(this);
	addChild(m_vu, (OC_WINDOW_WIDTH-EXB_BUTTON_WIDTH-5)/2, EXB_TOP_BUTTON_OFFSET,
		EXB_BUTTON_WIDTH, EXB_BUTTON_HEIGHT, 0/*"Invalidate on Resize*/);

	int i = 0;
	/*
	m_titleText = new TextBar();
	m_titleText->setName("Oddcast");
	m_titleText->setTextSize(EXB_TEXT_FONT_SIZE);
	addChild(m_titleText, EXB_LEFT_LABEL_OFFSET + (increment*i), EXB_TOP_LABEL_OFFSET, EXB_COLUMN1_WIDTH, 20);
	i++;
	*/


	m_ServerDescText = new TextBar();
	m_ServerDescText->setName("Server Desc");
	m_ServerDescText->setTextSize(EXB_TEXT_FONT_SIZE);
	addChild(m_ServerDescText, EXB_LEFT_LABEL_OFFSET , EXB_TOP_LABEL_OFFSET + (increment*i), EXB_COLUMN1_WIDTH, 20);
	m_ServerDescValue = new TextBar();
	m_ServerDescValue->setName("");
	m_ServerDescValue->setTextSize(EXB_TEXT_FONT_SIZE);
	addChild(m_ServerDescValue, EXB_LEFT_LABEL_OFFSET + rightColumn, EXB_TOP_LABEL_OFFSET + (increment*i), EXB_COLUMN2_WIDTH, 20);
	i++;
	m_DestinationText = new TextBar();
	m_DestinationText->setName("Destination");
	m_DestinationText->setTextSize(EXB_TEXT_FONT_SIZE);
	addChild(m_DestinationText, EXB_LEFT_LABEL_OFFSET, EXB_TOP_LABEL_OFFSET  + (increment*i), EXB_COLUMN1_WIDTH, 20);
	m_DestinationValue = new TextBar();
	m_DestinationValue->setName("");
	m_DestinationValue->setTextSize(EXB_TEXT_FONT_SIZE);
	addChild(m_DestinationValue, EXB_LEFT_LABEL_OFFSET + rightColumn, EXB_TOP_LABEL_OFFSET + (increment*i), EXB_COLUMN2_WIDTH, 20);
	i++;
	m_MetadataText = new TextBar();
	m_MetadataText->setName("Metadata");
	m_MetadataText->setTextSize(EXB_TEXT_FONT_SIZE);
	addChild(m_MetadataText, EXB_LEFT_LABEL_OFFSET, EXB_TOP_LABEL_OFFSET + (increment*i), EXB_COLUMN1_WIDTH, 20);
	m_MetadataValue = new TextBar();
	m_MetadataValue->setName("");
	m_MetadataValue->setTextSize(EXB_TEXT_FONT_SIZE);
	addChild(m_MetadataValue, EXB_LEFT_LABEL_OFFSET + rightColumn, EXB_TOP_LABEL_OFFSET + (increment*i), EXB_COLUMN2_WIDTH, 20);
	i++;
	m_BPSText = new TextBar();
	m_BPSText->setName("kbps Sent");
	m_BPSText->setTextSize(EXB_TEXT_FONT_SIZE);
	addChild(m_BPSText, EXB_LEFT_LABEL_OFFSET, EXB_TOP_LABEL_OFFSET + (increment*i), EXB_COLUMN1_WIDTH, 20);
	m_BPSValue = new TextBar();
	m_BPSValue->setName("");
	m_BPSValue->setTextSize(EXB_TEXT_FONT_SIZE);
	addChild(m_BPSValue, EXB_LEFT_LABEL_OFFSET + rightColumn, EXB_TOP_LABEL_OFFSET + (increment*i), EXB_COLUMN2_WIDTH, 20);
	i++;
	m_BitrateText = new TextBar();
	m_BitrateText->setName("Bitrate");
	m_BitrateText->setTextSize(EXB_TEXT_FONT_SIZE);
	addChild(m_BitrateText, EXB_LEFT_LABEL_OFFSET, EXB_TOP_LABEL_OFFSET + (increment*i), EXB_COLUMN1_WIDTH, 20);
	m_BitrateValue = new TextBar();
	m_BitrateValue->setName("");
	m_BitrateValue->setTextSize(EXB_TEXT_FONT_SIZE);
	addChild(m_BitrateValue, EXB_LEFT_LABEL_OFFSET + rightColumn, EXB_TOP_LABEL_OFFSET + (increment*i), EXB_COLUMN2_WIDTH, 20);
	i++;
	

	m_StatusText = new TextBar();
	m_StatusText->setName("Status");
	m_StatusText->setTextSize(EXB_TEXT_FONT_SIZE);
	addChild(m_StatusText, EXB_LEFT_LABEL_OFFSET, EXB_TOP_LABEL_OFFSET + (increment*i), EXB_COLUMN1_WIDTH, 20);
	m_StatusValue = new TextBar();
	m_StatusValue->setName("");
	m_StatusValue->setTextSize(EXB_TEXT_FONT_SIZE);
	addChild(m_StatusValue, EXB_LEFT_LABEL_OFFSET + rightColumn, EXB_TOP_LABEL_OFFSET + (increment*i), EXB_COLUMN2_WIDTH, 20);
	i++;

	m_LiveRecordingText = new TextBar();
	m_LiveRecordingText->setName("");
	m_LiveRecordingText->setTextSize(EXB_TEXT_FONT_SIZE);
	addChild(m_LiveRecordingText, EXB_LEFT_LABEL_OFFSET, EXB_TOP_LABEL_OFFSET + (increment*i), EXB_COLUMN2_WIDTH+100, 20);

	ClearTextFields();
	pOddcast = this;
}

OddcastWnd::~OddcastWnd() 
{
	if (getIsConnected()) {
		disconnectFromServer();
		setForceStop(1);
		if (getLiveRecordingSetFlag()) {
			stopRecording();
		}
		setCurrentSongTitle("");
		m_connect->setButtonText("Connect", EXB_BUTTON_FONT_SIZE);
		gAvgRight = 0;
		gAvgLeft = 0;
		invalidate();

	}

/*
	if (m_ServerDescText) {
		delete m_ServerDescText;
	}
	if (m_ServerDescValue) {
		delete m_ServerDescValue;
	}
	if (m_DestinationText) {
		delete m_DestinationText;
	}
	if (m_DestinationValue) {
		delete m_DestinationValue;
	}
	if (m_MetadataText) {
		delete m_MetadataText;
	}
	if (m_MetadataValue) {
		delete m_MetadataValue;
	}
	if (m_BPSText) {
		delete m_BPSText;
	}
	if (m_BPSValue) {
		delete m_BPSValue;
	}
	if (m_BitrateText) {
		delete m_BitrateText;
	}
	if (m_BitrateValue) {
		delete m_BitrateValue;
	}
	if (m_StatusText) {
		delete m_StatusText;
	}
	if (m_StatusValue) {
		delete m_StatusValue;
	}
	if (m_LiveRecordingText) {
		delete m_LiveRecordingText;
	}


	if (m_connect) {
		delete m_connect;
	}
	if (m_config) {
		delete m_config;
	}
	if (m_vu) {
		delete m_vu;
	}
*/
	api->core_delCallback(0, this);
	m_pcurInst = NULL;
}


int OddcastWnd::onInit() {
	int retval = ODDCASTWND_PARENT::onInit();

	setPreferences( SUGGESTED_W, OC_WINDOW_WIDTH ); 
	setPreferences( SUGGESTED_H, OC_WINDOW_HEIGHT ); 
	setPreferences( MAXIMUM_W, OC_WINDOW_WIDTH ); 
	setPreferences( MAXIMUM_H, OC_WINDOW_HEIGHT ); 
	setPreferences( MINIMUM_W, OC_WINDOW_WIDTH ); 
	setPreferences( MINIMUM_H, OC_WINDOW_HEIGHT ); 

	api->core_addCallback(0, this);	// for on status change

	m_hInst = the->gethInstance(); //api->main_gethInstance();
	m_hWnd = gethWnd(); 


//	appcmds_addCmd("RLY", SRB_RELAY); 
	// Add your AppCmdI to the guiobjectwnd 
//	getGuiObject()->guiobject_addAppCmds(this); 

	m_vu_on_off = 0;
	transcode_init();
	setOggEncoderText("Winamp 3 DSP");
	setTimer(myTimer, myTimerDuration);
	return retval;
}
int OddcastWnd::reconnectServer()
{
	if (connectToServer()) {
		initializeencoder();
		return 1;
	}
	return 0;
}

void OddcastWnd::timerCallback(int id)
{
  switch (id)
  {
	case myThirdTimer:
		if (reconnectServer()) {
			killTimer(myThirdTimer);
		}
	  break;
    case myTimer:  
      invalidate();
      break;

    default:
      ODDCASTWND_PARENT::timerCallback(id);
      break;
  };
}

int 
OddcastWnd::corecb_onStarted()
{
	UpdateWhatsPlaying();
	return 1;
}
int 
OddcastWnd::corecb_onTitleChange()
{
	UpdateWhatsPlaying();
	return 1;
}
int 
OddcastWnd::corecb_onTitle2Change()
{
	UpdateWhatsPlaying();
	return 1;
}

void 
OddcastWnd::appcmds_onCommand(int id, const RECT *buttonRect)
{
	char s[255];
	sprintf(s, "%d\n", id);
	OutputDebugString(s);
}


void 
UpdateWhatsPlaying()
{
	char	buf[4097] = "";


	if (getIsConnected()) {
		if(api->core_getCurrent(0) != NULL) {
			memset(buf, '\000', sizeof(buf));
			api->metadb_getMetaData(api->core_getCurrent(0), MT_NAME, buf, 4096, MDT_STRINGZ);
			if (setCurrentSongTitle((char *)buf)) {
				pOddcast->m_MetadataValue->setName((char *)buf);
			}
		}
//		const char *szurl = api->core_getCurrent(0);
	}

	/*
	if (szurl == NULL || strstr(szurl, "http://") == NULL)
	{
		m_titleText->setName(NO_STREAM_MSG);
		return;
	}
	std::string msg = "Press start to rip ";
	msg +=  szurl;
	m_titleText->setName(msg.c_str());
	*/
}


HANDLE	gThreadHandle = 0;
DWORD	gThreadID = 0;

DWORD WINAPI startOddcast(LPVOID lpParameter) 
{

	
	initializeencoder();
	if (connectToServer()) {
		setCurrentSongTitle("");
		UpdateWhatsPlaying();
	}
	else {
		MessageBox(NULL, "Unable to Connect", NULL, MB_OK);
		return(0);
	}
	if (getLiveRecordingSetFlag()) {
		if (!initLiveRecording()) {
			MessageBox(NULL, "Fail to open input device", NULL, MB_OK);
			disconnectFromServer();
			return 0;
		}
		else {
			char	buf[255] = "";
			sprintf(buf, "%dHz/Stereo PCM", getLiveInSamplerate());
			setSourceDescription(buf);
			int songChange = setCurrentSongTitle(getServerDesc());
			startRecording();
		}
	}
	return 1;
}

int OddcastWnd::onPaint(Canvas *canvas) {
  // Make sure there actually is a canvas to paint to.
  PaintBltCanvas pbc;
  if (canvas == NULL) {
    pbc.beginPaint(this);
    canvas = &pbc;
  }

  // Then, call your parent class's onpaint.
  ODDCASTWND_PARENT::onPaint(canvas);

  if (m_vu_on_off) {
	  //
	  //  Get the current client rect for your window.  This is the rectangle
	  //  in which you should be expected to paint thyself.
	  RECT crect = clientRect();

 
	  crect.top = 130;
	  crect.bottom = 145;
	  crect.left = 100;
	  crect.right = 200;

	  // Fill the canvas with white and prepare to blit your little heart out.
	  canvas->fillRect(&crect, RGB(0x00, 0x00, 0x00));

	  int leftVUdata = api->core_getLeftVuMeter(0); // zero is the "main" core.  
	  int rightVUdata = api->core_getRightVuMeter(0); // zero is the "main" core.  

	  leftVUdata = gAvgLeft;
	  rightVUdata = gAvgRight;
	  //
	  //  Then, after we get the VU data, we make a graphical representation of it.
	  //  
	  int h = crect.bottom - crect.top;
	  int w = crect.right - crect.left;

	  int vu_width = w-10;

	  //
	  //  *** Left VU
	  //
	  if (leftVUdata) {
		RECT leftvu;
		// we want to make the rectangles "grow" upward, so we fix our bottom edge first.
	//    leftvu.bottom = crect.bottom - (h / 4);  // One quarter of the window height from the bottom.
		leftvu.bottom = crect.bottom - 10; 
		// and 10 pixels in?
		leftvu.left = crect.left + 2;
		// and 20 pixels wide?
		//leftvu.right = leftvu.left + 20;
		int leftvu_w = (vu_width * leftVUdata) / 256;  // if you tell me to shift 8, I'll say "yes, but YOU know that.  does everyone?"
		//leftvu.top = (leftvu.bottom - leftvu_h) + 2;
		leftvu.right = leftvu.left + leftvu_w + 2;
		// and then we calc our height.
		leftvu.top = crect.bottom - 13;

		// and then calc the colors.
		int green = (leftVUdata < 128) ? 255 : 255 - ((leftVUdata-128) * 2);
		int red = (leftVUdata < 128) ? 255 - ((leftVUdata) * 2) : 255;
		// and then we blit a rect.
		canvas->fillRect(&leftvu, RGB(red, green, 0x00));
	  }

	  //
	  //  *** Right VU
	  //
	  if (rightVUdata) {
		RECT rightvu;
		rightvu.bottom = crect.bottom - 2; 
		// and 10 pixels in?
		rightvu.left = crect.left + 2;
		// and 20 pixels wide?
		//leftvu.right = leftvu.left + 20;
		int rightvu_w = (vu_width * rightVUdata) / 256;  // if you tell me to shift 8, I'll say "yes, but YOU know that.  does everyone?"
		//leftvu.top = (leftvu.bottom - leftvu_h) + 2;
		rightvu.right = rightvu.left + rightvu_w + 2;
		// and then we calc our height.
		rightvu.top = crect.bottom - 5;
		int green = (rightVUdata < 128) ? 255 : 255 - ((rightVUdata-128) * 2);
		int red = (rightVUdata < 128) ? 255 - ((rightVUdata) * 2) : 255;
		canvas->fillRect(&rightvu, RGB(red, green, 0x00));

	  }
  }
  
  return 1;
}
int 
OddcastWnd::handleChildNotify (int msg, int objId, int param1, int param2)
{

	if (msg != ChildNotify::BUTTON_LEFTPUSH)
		return 0;
	int ret;

	switch(objId) 
	{
	case ODDCAST_VU:
		if (m_vu_on_off) {
			m_vu_on_off = 0;
		}
		else {
			m_vu_on_off = 1;
		}
		break;

	case ODDCAST_CONNECT:
		if (getIsConnected()) {
			disconnectFromServer();
			setForceStop(1);
			if (getLiveRecordingSetFlag()) {
				stopRecording();
			}
			setCurrentSongTitle("");
			m_connect->setButtonText("Connect", EXB_BUTTON_FONT_SIZE);
			gAvgRight = 0;
			gAvgLeft = 0;
			invalidate();

		}
		else {
			char	message[1024] = "";
			char	error[1024] = "";
			int ret = 1;
#ifdef WIN32
			if (getWMAFlag()) {
				ret = startWMAServer(message,error);
			}
#endif
			if (ret == 0) {
				MessageBox(NULL,message, NULL, MB_OK);
				MessageBox(NULL,error, NULL, MB_OK);
			}
			else {
				if (startOddcast(NULL)) {
//				gThreadHandle = CreateThread(NULL, 0, startOddcast, NULL, 0, &gThreadID);
					m_connect->setButtonText("Stop", EXB_BUTTON_FONT_SIZE);
				}
			}
		}
		break;
	case ODDCAST_CONFIG:
		if (!getIsConnected()) {
			STARTUPINFO si = {0};
			PROCESS_INFORMATION pi = {0};

			si.cb = sizeof (si);
			si.lpReserved = si.lpDesktop = si.lpTitle = NULL;
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_SHOW;
			si.cbReserved2 = 0;
			si.lpReserved2 = 0;
			DWORD dwError;

			CreateProcess (NULL, "oddcast_editConfig.exe", 
			  NULL, NULL, 
			  TRUE, CREATE_NEW_CONSOLE, 
			  NULL, NULL, 
			  &si, &pi); 

			WaitForSingleObject (pi.hProcess, INFINITE);
			initializeGlobals();
			transcode_init();
		}
		else {
			MessageBox(NULL, "Sorry, but you cannot edit the settings while connected", "Error", MB_OK);
		}
		break;
	}
	return 1;
}

int 
OddcastWnd::childNotify(RootWnd *which, int msg, int param1, int param2)
{
	 // for an interesting reason, it is valid to be given NULL as a which pointer.
	if (which != NULL) 
	{
		if (handleChildNotify(msg, which->getNotifyId(), param1, param2) == -1) 
		{
			return 0;
		}
	}
	return ODDCASTWND_PARENT::childNotify(which, msg, param1, param2);
}

void 
OddcastWnd::ClearTextFields()
{
	getGuiObject()->guiobject_setStatusText("");
}

