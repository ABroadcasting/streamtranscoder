#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/sizer.h"
#include <wx/dynarray.h>

#if !wxUSE_CONSTRAINTS
#error You must set wxUSE_CONSTRAINTS to 1 in setup.h!
#endif

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #define USE_XPM
#endif

#include <ctype.h>

//#include "wx/statline.h"
#include <wx/msgdlg.h>

#include "liboddcast_wxconfig.h"


#include "log.h"
#include "transcurl.h"
#include "liboddcast.h"
#include "transcode.h"

#define ID_CONNECT_BUTTON	100
#define ID_CONNECT_BUTTON	100
#define ID_NOTEBOOK			101
#define ID_LAME_OPTIONS		102
#define ID_VORBIS_OPTIONS	103
#define ID_WMA_OPTIONS		104
#define ID_OK_BUTTON		105
#define ID_CANCEL_BUTTON		106
#define ID_ENCODE_TYPE      107


#define LONG_TYPE 1
#define STRING_TYPE 2


extern transcodeGlobals	globals;
extern oddcastGlobals g;

//WX_DEFINE_ARRAY(int, wxArrayInt);
//WX_DEFINE_ARRAY(wxString, wxArrayString);

void setCheckBoxCtrl(wxCheckBox *pCtrl, int value)
{


	if (value) {
		pCtrl->SetValue(true);
	}
	else {
		pCtrl->SetValue(false);
	}
	return;
					
}

long getTextCtrl(wxTextCtrl *pCtrl, int type, void *value)
{
	wxString	stringValue;
	char		tmpBuf[1024] = "";
	long		tmpLong = 0;

	stringValue = pCtrl->GetValue();

	switch (type) {
			case LONG_TYPE:
				tmpLong = atol(stringValue.c_str());
				return tmpLong;
				break;
			case STRING_TYPE:
				memset(tmpBuf, '\000', sizeof(tmpBuf));
				strcpy(tmpBuf, stringValue.c_str());
				strcpy((char *)value, tmpBuf);
				break;
			default:
				break;
	}
	return 0;
					
}
void setTextCtrl(wxTextCtrl *pCtrl, int type, void *value)
{
	wxString	stringValue;
	char		tmpBuf[255] = "";


	switch (type) {
			case LONG_TYPE:
				sprintf(tmpBuf, "%d", (long)value);
				stringValue = tmpBuf;
				break;
			case STRING_TYPE:
				stringValue = (char *)value;
				break;
			default:
				stringValue = "";
				break;
	}
	pCtrl->SetValue(stringValue);
	return;
					
}

long getCheckBox(wxCheckBox *pCtrl)
{
	return pCtrl->GetValue();

}

long getComboBox(wxComboBox *pCtrl, int type, void *value)
{
	wxString	stringValue;
	char		tmpBuf[1024] = "";
	long		tmpLong = 0;

	stringValue = pCtrl->GetValue();
	stringValue = pCtrl->GetStringSelection();


	switch (type) {
			case LONG_TYPE:
				tmpLong = atol(stringValue.c_str());
				return tmpLong;
				break;
			case STRING_TYPE:
				memset(tmpBuf, '\000', sizeof(tmpBuf));
				strcpy(tmpBuf, stringValue.c_str());
				strcpy((char *)value, tmpBuf);
				break;
			default:
				break;
	}
	return 0;
					
}

void setComboBoxString(wxComboBox *pCtrl, wxArrayString *pInitArray, void *value)
{
	wxString	stringValue;
	char		tmpBuf[255] = "";
	int			selected = 0;
	wxString string2 = (char *)value;

	stringValue = "";
	for (int i=0;i<pInitArray->GetCount();i++) {
		stringValue = pInitArray->Item(i);
		if (stringValue == string2) {
			selected = i;
		}
		if (stringValue) {
			pCtrl->Append(stringValue);
		}
	}
	stringValue = (char *)value;
	pCtrl->SetSelection(selected);
	pCtrl->SetValue(stringValue);
	return;
					
}
void setComboBoxInt(wxComboBox *pCtrl, wxArrayInt *pInitArray, int value)
{
	wxString	stringValue;
	char		tmpBuf[255] = "";
	int			selected = 0;

	stringValue = "";
	for (int i=0;i<pInitArray->GetCount();i++) {
		int		localInt = 0;

		localInt = pInitArray->Item(i);
		if (localInt == value) {
			selected = i;
		}
		sprintf(tmpBuf, "%d", (long)localInt);
		stringValue = tmpBuf;
		pCtrl->Append(stringValue);

	}
	sprintf(tmpBuf, "%d", (long)value);
	stringValue = tmpBuf;

	pCtrl->SetSelection(selected);
	pCtrl->SetValue(stringValue);
	return;
					
}
void wxMessage(char *message)
{
	wxMessageDialog	mess(NULL, message, "Message", wxOK);
	mess.ShowModal();

}
//-----------------------------------------------------------------
//  wxOddcastConfigFrame
//-----------------------------------------------------------------

// Define my frame constructor
wxOddcastConfigFrame::wxOddcastConfigFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h), wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION)
{

	mainPanel = (ConfigPanel *) NULL;
	generalPanel = (ConfigGeneralPanel *) NULL;
	encodePanel = (ConfigEncodePanel *) NULL;
	serverPanel = (ConfigServerPanel *) NULL;

}

int	 wxOddcastConfigFrame::ValidateConfigValues() {
	char	gSourceURL[255] = "";
	char	gServer[255] = "";
	char	gPort[255] = "";
	char	gPassword[255] = "";
	char	gServGenre[255] = "";
	char	gMountpoint[255] = "";
	char	gServURL[255] = "";
	char	gServDesc[255] = "";
	char	gReconnectSec = 0;
	char	gServerType[255] = "";
	char	gAdvRecDevice[255] = "";
	int		gLiveInSamplerate;
	int		gPubServ = 0;
	int		gAutoReconnect = 0;
	int		currentBitrate = 0;
	int		currentBitrateMin = 0;
	int		currentBitrateMax = 0;
	int		currentSamplerate = 0;
	int		currentChannels = 0;
	char	gEncodeType[255] = "";
	int		cbrflag = 0;
	char	VBR_mode[255] = "";
	int 	quality = 0;
	int		highpassfreq = 0;
	int 	lowpassfreq = 0;
	char	gOggBitQual[255] = "";
	char	gOggQuality[255] = "";
	int		OggQuality = 0;
	int		gWMAPort = 0;
	int		gWMAClients = 0;
	int		gLiveRecordingFlag = 0;

	// Get all the fields into local values
	if (!m_outputOnlyFlag) {
		getTextCtrl(serverPanel->m_InputURLCtrl, STRING_TYPE, (void *)gSourceURL);
	}
	gLiveRecordingFlag = getCheckBox(serverPanel->m_LineInCtrl);
	getTextCtrl(serverPanel->m_ServerCtrl, STRING_TYPE, (void *)gServer);
	getTextCtrl(serverPanel->m_PortCtrl, STRING_TYPE, (void *)gPort);
	getTextCtrl(serverPanel->m_EncodePasswordCtrl, STRING_TYPE, (void *)gPassword);
	getTextCtrl(serverPanel->m_ServerGenreCtrl, STRING_TYPE, (void *)gServGenre);
	getTextCtrl(serverPanel->m_MountpointCtrl, STRING_TYPE, (void *)gMountpoint);
	getTextCtrl(serverPanel->m_StreamURLCtrl, STRING_TYPE, (void *)gServURL);
	getTextCtrl(serverPanel->m_ServerDescCtrl, STRING_TYPE, (void *)gServDesc);
	gReconnectSec = getTextCtrl(serverPanel->m_ReconnectSecondsCtrl, LONG_TYPE, (void *)NULL);
	getComboBox(serverPanel->m_ServerTypeCtrl, STRING_TYPE, gServerType);
	gLiveInSamplerate = getTextCtrl(serverPanel->m_LiveRecSamplerateCtrl, LONG_TYPE, (void *)NULL);
	gPubServ = getCheckBox(serverPanel->m_PublicCtrl);
	gAutoReconnect = getCheckBox(serverPanel->m_ReconnectCtrl);

	// General Panel
	currentBitrate = getTextCtrl(generalPanel->m_BitrateNomCtrl, LONG_TYPE, (void *)NULL);
	currentBitrateMin = getTextCtrl(generalPanel->m_BitrateMinCtrl, LONG_TYPE, (void *)NULL);
	currentBitrateMax = getTextCtrl(generalPanel->m_BitrateMaxCtrl, LONG_TYPE, (void *)NULL);
	currentSamplerate = getTextCtrl(generalPanel->m_SampleRateCtrl, LONG_TYPE, (void *)NULL);
	currentChannels = getComboBox(generalPanel->m_ChannelsCtrl, LONG_TYPE, (void *)NULL);
	getComboBox(generalPanel->m_EncodeTypeCtrl, STRING_TYPE, gEncodeType);

	// Encode Panel - LAME
	int vbr = getCheckBox(encodePanel->m_LameOptionPanel->m_VBRCtrl);
	cbrflag = !vbr;
	getComboBox(encodePanel->m_LameOptionPanel->m_VBRModeCtrl, STRING_TYPE, VBR_mode);
	quality = getTextCtrl(encodePanel->m_LameOptionPanel->m_QualityCtrl, LONG_TYPE, (void *)NULL);
	// Encode Panel - Vorbis
	getComboBox(encodePanel->m_VorbisOptionPanel->m_QualityBMCtrl, STRING_TYPE, gOggBitQual);
	getTextCtrl(encodePanel->m_VorbisOptionPanel->m_QualityCtrl, STRING_TYPE, (void *)gOggQuality);
	OggQuality = atoi(gOggQuality);
#ifdef WIN32
	// Encode Panel - WMA
	gWMAPort = getTextCtrl(encodePanel->m_WMAOptionPanel->m_ListenPortCtrl, LONG_TYPE, (void *)NULL);
	gWMAClients = getTextCtrl(encodePanel->m_WMAOptionPanel->m_ClientsCtrl, LONG_TYPE, (void *)NULL);
#else
	highpassfreq = getTextCtrl(encodePanel->m_LameOptionPanel->m_HighpassCtrl, LONG_TYPE, (void *)NULL);
	lowpassfreq = getTextCtrl(encodePanel->m_LameOptionPanel->m_LowpassCtrl, LONG_TYPE, (void *)NULL);
	getTextCtrl(serverPanel->m_LiceRecDeviceCtrl, STRING_TYPE, (void *)gAdvRecDevice);
#endif

	if (currentBitrate <= 0) {
		wxMessage("You must supply a bitrate");
		return(0);
	}
	if (currentSamplerate <= 0) {
		wxMessage("You must supply a samplerate");
		return(0);
	}
	if (currentChannels <= 0) {
		wxMessage("You must supply the number of channels");
		return(0);
	}
	if (strlen(gEncodeType) <= 0) {
		wxMessage("You must supply a value for encoder type");
		return(0);
	}
	if (strlen(gServerType) <= 0) {
		wxMessage("You must supply a value for server type");
		return(0);
	}
	if (strlen(gServer) <= 0) {
		wxMessage("You must supply a value for destination server");
		return(0);
	}
	if (strlen(gPort) <= 0) {
		wxMessage("You must supply a value for destination port");
		return(0);
	}
	if (strlen(gPassword) <= 0) {
		wxMessage("You must supply a value for encoding password");
		return(0);
	}

	if (!strcmp(gServerType, "Icecast")) {
		if (strlen(gMountpoint) <= 0) {
			wxMessage("You must specify a mountpoint when sending to an icecast server");
			return(0);
		}
	}
	if (!strcmp(gServerType, "Icecast2")) {
		if (strlen(gMountpoint) <= 0) {
			wxMessage("You must specify a mountpoint when sending to an icecast2 server");
			return(0);
		}
	}
#ifdef WIN32
	if (!strcmp(gEncodeType, "WMA")) {
		if (gWMAPort <= 0) {
			wxMessage("You must specify a listen port when encoding using WMA");
			return(0);
		}
		if (gWMAClients <= 0) {
			wxMessage("You must specify a number of max clients when encoding using WMA");
			return(0);
		}
		if (strcmp(gServerType, "WMA")) {
			wxMessage("You must specify a server type of 'WMA' when encoding using WMA");
			return(0);
		}
	}
#endif
	if (!strcmp(gEncodeType, "OGG")) {

//		if (!strcmp(gOggBitQual, "Bitrate Management")) {
//			wxMessage("It is not recommended to use bitrate management with vorbis.  There are currently known memory leaks with this mode..Use at your own risk!");
//		}
		if ((OggQuality < -1) || (OggQuality > 10)) {
			wxMessage("Ogg vorbis quality must be between -1 and 10");
			return(0);
		}
		if (strcmp(gServerType, "Icecast2")) {
			wxMessage("You must specify a server type of 'Icecast2' when encoding using Vorbis");
			return(0);
		}
		if (strlen(gMountpoint) <= 5) {
			wxMessage("You must specify a mountpoint ending in .ogg when encoding using Vorbis");
			return(0);
		}
		if (gMountpoint[0] != '/') {
			wxMessage("You must specify a mountpoint that begins with a '/' when encoding using Vorbis");
			return(0);
		}
		if (strcmp(gMountpoint+strlen(gMountpoint)-4, ".ogg")) {
			wxMessage("You must specify a mountpoint ending in .ogg when encoding using Vorbis");
			return(0);
		}
	}
	if (!strcmp(gEncodeType, "LAME")) {

		if ((quality <= 0) || (quality > 10)) {
			wxMessage("Ogg vorbis quality must be between 1 and 10 (1 being the highest quality)");
			return(0);
		}
		if (!((!strcmp(gServerType, "Icecast2")) || (!strcmp(gServerType, "Icecast")) || (!strcmp(gServerType, "Shoutcast")))) {
			wxMessage("You must specify a server type of 'Shoutcast', 'Icecast', or 'Icecast2' when encoding using LAME");
			return(0);
		}
	}
	if (m_outputOnlyFlag) {
		if (gLiveRecordingFlag) {
			if (gLiveInSamplerate <= 0) {
				wxMessage("You must specify a samplerate for live recording");
				return(0);
			}
		}
	}
	return(1);
}

int wxOddcastConfigFrame::TransferToGlobals() {
	
	if (!ValidateConfigValues()) {
		return(0);
	}


	// Server Panel
	if (!m_outputOnlyFlag) {
		getTextCtrl(serverPanel->m_InputURLCtrl, STRING_TYPE, (void *)g.gSourceURL);
	}
	g.gLiveRecordingFlag = getCheckBox(serverPanel->m_LineInCtrl);
	getTextCtrl(serverPanel->m_ServerCtrl, STRING_TYPE, (void *)g.gServer);
	getTextCtrl(serverPanel->m_PortCtrl, STRING_TYPE, (void *)g.gPort);
	getTextCtrl(serverPanel->m_EncodePasswordCtrl, STRING_TYPE, (void *)g.gPassword);
	getTextCtrl(serverPanel->m_ServerGenreCtrl, STRING_TYPE, (void *)g.gServGenre);
	getTextCtrl(serverPanel->m_MountpointCtrl, STRING_TYPE, (void *)g.gMountpoint);
	getTextCtrl(serverPanel->m_StreamURLCtrl, STRING_TYPE, (void *)g.gServURL);
	getTextCtrl(serverPanel->m_ServerDescCtrl, STRING_TYPE, (void *)g.gServDesc);
	g.gReconnectSec = getTextCtrl(serverPanel->m_ReconnectSecondsCtrl, LONG_TYPE, (void *)NULL);
	getComboBox(serverPanel->m_ServerTypeCtrl, STRING_TYPE, g.gServerType);
#ifndef WIN32
	getTextCtrl(serverPanel->m_LiceRecDeviceCtrl, STRING_TYPE, (void *)g.gAdvRecDevice);
#endif
	g.gLiveInSamplerate = getTextCtrl(serverPanel->m_LiveRecSamplerateCtrl, LONG_TYPE, (void *)NULL);
	g.gPubServ = getCheckBox(serverPanel->m_PublicCtrl);
	g.gAutoReconnect = getCheckBox(serverPanel->m_ReconnectCtrl);

	// General Panel
	g.currentBitrate = getTextCtrl(generalPanel->m_BitrateNomCtrl, LONG_TYPE, (void *)NULL);
	g.currentBitrateMin = getTextCtrl(generalPanel->m_BitrateMinCtrl, LONG_TYPE, (void *)NULL);
	g.currentBitrateMax = getTextCtrl(generalPanel->m_BitrateMaxCtrl, LONG_TYPE, (void *)NULL);
	g.currentSamplerate = getTextCtrl(generalPanel->m_SampleRateCtrl, LONG_TYPE, (void *)NULL);
	g.currentChannels = getComboBox(generalPanel->m_ChannelsCtrl, LONG_TYPE, (void *)NULL);
	getComboBox(generalPanel->m_EncodeTypeCtrl, STRING_TYPE, g.gEncodeType);

	// Encode Panel - LAME
	int vbr = getCheckBox(encodePanel->m_LameOptionPanel->m_VBRCtrl);
	g.gLAMEOptions.cbrflag = !vbr;
	getComboBox(encodePanel->m_LameOptionPanel->m_VBRModeCtrl, STRING_TYPE, g.gLAMEOptions.VBR_mode);
	g.gLAMEOptions.quality = getTextCtrl(encodePanel->m_LameOptionPanel->m_QualityCtrl, LONG_TYPE, (void *)NULL);
#ifndef WIN32
	g.gLAMEOptions.highpassfreq = getTextCtrl(encodePanel->m_LameOptionPanel->m_HighpassCtrl, LONG_TYPE, (void *)NULL);
	g.gLAMEOptions.lowpassfreq = getTextCtrl(encodePanel->m_LameOptionPanel->m_LowpassCtrl, LONG_TYPE, (void *)NULL);
#endif

	// Encode Panel - Vorbis
	getComboBox(encodePanel->m_VorbisOptionPanel->m_QualityBMCtrl, STRING_TYPE, g.gOggBitQual);
	getTextCtrl(encodePanel->m_VorbisOptionPanel->m_QualityCtrl, STRING_TYPE, (void *)g.gOggQuality);

#ifdef WIN32
	// Encode Panel - WMA
	g.gWMAPort = getTextCtrl(encodePanel->m_WMAOptionPanel->m_ListenPortCtrl, LONG_TYPE, (void *)NULL);
	g.gWMAClients = getTextCtrl(encodePanel->m_WMAOptionPanel->m_ClientsCtrl, LONG_TYPE, (void *)NULL);
#endif

	return 1;
}

void wxOddcastConfigFrame::OnEncodeType() {
	char	encoderType[255] = "";

	

	getComboBox(generalPanel->m_EncodeTypeCtrl, STRING_TYPE, encoderType);	
	if (!strcmp(encoderType, "OGG")) {
		encodePanel->m_notebook->SetSelection(1);
		serverPanel->m_ServerTypeCtrl->SetValue("Icecast2");
	}
	if (!strcmp(encoderType, "LAME")) {
		encodePanel->m_notebook->SetSelection(0);
	}
#ifdef WIN32
	if (!strcmp(encoderType, "WMA")) {
		encodePanel->m_notebook->SetSelection(2);
		serverPanel->m_ServerTypeCtrl->SetValue(encoderType);
	}
#endif
}

void wxOddcastConfigFrame::OnOk() {

	if (!TransferToGlobals()) {
		;
	}
	else {
		writeConfigFile();
		readConfigFile();
		Destroy();
	}
}
void wxOddcastConfigFrame::OnCancel() {
	Destroy();
}

void wxOddcastConfigFrame::Init(int outputOnlyFlag) {
	int initialy = 5;
	int initialx = 5;
	int separator = 10;
	int panelWidth = 460;
	int	buttonWidth = 75;
	int generalHeight = 115;
	int encodeHeight = 150;
	int serverHeight = 330;

	m_outputOnlyFlag = outputOnlyFlag;

	SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));

	int i = 0;
	generalPanel = new ConfigGeneralPanel(this, initialx, initialy, panelWidth, generalHeight);
	i++;
	encodePanel = new ConfigEncodePanel(this, initialx, (generalHeight+separator), panelWidth, encodeHeight);
	i++;
	serverPanel = new ConfigServerPanel(this, initialx, (generalHeight+encodeHeight+separator), panelWidth, serverHeight);

	m_OK = new wxButton(this, ID_OK_BUTTON, "Ok", wxPoint(panelWidth/4, (generalHeight+encodeHeight+separator+serverHeight+separator)), wxSize(buttonWidth, 25) );
	m_Cancel = new wxButton(this, ID_CANCEL_BUTTON, "Cancel", wxPoint((panelWidth/4)*2, (generalHeight+encodeHeight+separator+serverHeight+separator)), wxSize(buttonWidth, 25) );

	encodePanel->Init();
	Show(TRUE);
}

BEGIN_EVENT_TABLE(wxOddcastConfigFrame, wxFrame)
EVT_BUTTON    (ID_OK_BUTTON,          wxOddcastConfigFrame::OnOk)
EVT_BUTTON    (ID_CANCEL_BUTTON,          wxOddcastConfigFrame::OnCancel)
EVT_COMBOBOX  (ID_ENCODE_TYPE, wxOddcastConfigFrame::OnEncodeType) 
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(ConfigPanel, wxPanel)
END_EVENT_TABLE()
BEGIN_EVENT_TABLE(ConfigGeneralPanel, wxPanel)
END_EVENT_TABLE()
BEGIN_EVENT_TABLE(ConfigEncodePanel, wxPanel)
EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK, ConfigEncodePanel::TabChanged)
EVT_NOTEBOOK_PAGE_CHANGING(ID_NOTEBOOK, ConfigEncodePanel::TabChanging)
END_EVENT_TABLE()
BEGIN_EVENT_TABLE(ConfigServerPanel, wxPanel)
END_EVENT_TABLE()
BEGIN_EVENT_TABLE(ConfigLameOptionPanel, wxPanel)
END_EVENT_TABLE()
BEGIN_EVENT_TABLE(ConfigVorbisOptionPanel, wxPanel)
END_EVENT_TABLE()
BEGIN_EVENT_TABLE(ConfigWMAOptionPanel, wxPanel)
END_EVENT_TABLE()



ConfigLameOptionPanel::ConfigLameOptionPanel( wxNotebook *notebook, wxWindowID id, int x, int y, int w, int h )
       : wxPanel(notebook, id)
{
	long textRowSize = 30;
	long startX = 55;
	long initialX = startX;
	long initialY = 25;
	long textWidth = 150;
	long textHeight = 25;
	long columnWidth = 75;


	int i = 0;
	m_VBRStatic = new wxStaticText(this, -1, "VBR", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
	m_VBRModeStatic = new wxStaticText(this, -1, "VBR Mode", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
	m_QualityStatic = new wxStaticText(this, -1, "Quality", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;

#ifndef WIN32
	i = 0;
	initialX = startX + columnWidth + textWidth + 20;
	m_HighpassStatic = new wxStaticText(this, -1, "Highpass Filter", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
	m_LowpassStatic = new wxStaticText(this, -1, "Lowpass Filter", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
#endif


	initialX = startX + columnWidth ;

	i = 0;
	m_VBRCtrl = new wxCheckBox(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)-(textRowSize/4)), wxSize(textWidth/2, textHeight), 0);
	i++;
	m_VBRModeCtrl = new wxComboBox(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)-(textRowSize/4)), wxSize(textWidth/2, textHeight), 0);
	i++;
	m_QualityCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)-(textRowSize/4)), wxSize(textWidth/2, textHeight), 0);
	i++;
	
#ifndef WIN32
	i = 0;
	initialX = startX + columnWidth + textWidth + 20 + columnWidth;

	m_HighpassCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)-(textRowSize/4)), wxSize(textWidth/2, textHeight), 0);
	i++;
	m_LowpassCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)-(textRowSize/4)), wxSize(textWidth/2, textHeight), 0);
	i++;
#endif

	setCheckBoxCtrl(m_VBRCtrl, !g.gLAMEOptions.cbrflag);
	wxArrayString l_array;

	l_array.Add("vbr_rh");
	l_array.Add("vbr_mtrh");
	l_array.Add("vbr_abr");
	setComboBoxString(m_VBRModeCtrl, &l_array, g.gLAMEOptions.VBR_mode);
	setTextCtrl(m_QualityCtrl, LONG_TYPE, (void *)g.gLAMEOptions.quality);
#ifndef WIN32
	setTextCtrl(m_HighpassCtrl, LONG_TYPE, (void *)g.gLAMEOptions.highpassfreq);
	setTextCtrl(m_LowpassCtrl, LONG_TYPE, (void *)g.gLAMEOptions.lowpassfreq);
#endif


}
ConfigVorbisOptionPanel::ConfigVorbisOptionPanel( wxNotebook *notebook, wxWindowID id, int x, int y, int w, int h )
       : wxPanel(notebook, id)
{
	long textRowSize = 30;
	long startX = 55;
	long initialX = startX;
	long initialY = 25;
	long textWidth = 300;
	long textHeight = 25;
	long columnWidth = 75;


	int i = 0;
	m_QualityBMStatic = new wxStaticText(this, -1, "Encoding Type", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
	m_QualityStatic = new wxStaticText(this, -1, "Quality Level", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;

	initialX = startX + columnWidth ;

	i = 0;
	m_QualityBMCtrl = new wxComboBox(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)-(textRowSize/4)), wxSize(textWidth/2, textHeight), 0);
	i++;
	m_QualityCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)-(textRowSize/4)), wxSize(textWidth/2, textHeight), 0);
	i++;

	wxArrayString l_array;

	l_array.Add("Quality");
	l_array.Add("Bitrate Management");
	setComboBoxString(m_QualityBMCtrl, &l_array, g.gOggBitQual);
	setTextCtrl(m_QualityCtrl, STRING_TYPE, (void *)g.gOggQuality);

}
ConfigWMAOptionPanel::ConfigWMAOptionPanel( wxNotebook *notebook, wxWindowID id, int x, int y, int w, int h )
       : wxPanel(notebook, id)
{
	long textRowSize = 30;
	long startX = 55;
	long initialX = startX;
	long initialY = 25;
	long textWidth = 100;
	long textHeight = 25;
	long columnWidth = 75;


	int i = 0;
	m_ListenPortStatic = new wxStaticText(this, -1, "Listen Port", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
	m_ClientsStatic = new wxStaticText(this, -1, "Num Clients", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;

	initialX = startX + columnWidth ;

	i = 0;
	m_ListenPortCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)-(textRowSize/4)), wxSize(textWidth/2, textHeight), 0);
	i++;
	m_ClientsCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)-(textRowSize/4)), wxSize(textWidth/2, textHeight), 0);
	i++;

	setTextCtrl(m_ListenPortCtrl, LONG_TYPE, (void *)g.gWMAPort);
	setTextCtrl(m_ClientsCtrl, LONG_TYPE, (void *)g.gWMAClients);

}


ConfigPanel::ConfigPanel( wxFrame *frame, int x, int y, int w, int h )
       : wxPanel( frame, -1, wxPoint(x, y), wxSize(w, h) )
{
	wxLayoutConstraints *c = new wxLayoutConstraints;

	//m_staticBox = new wxStaticBox( this, -1, "Main Server", wxPoint(10,10), wxSize(initialPanelHeight-20,initialPanelWidth-20) );
	m_staticBox = new wxStaticBox( this, -1, "Main Server", wxPoint(0,0), wxSize(w,h));

	c->centreX.SameAs    (this, wxCentreX);
	c->left.SameAs       (this, wxLeft);
	c->top.SameAs        (this, wxTop);
	c->width.PercentOf   (this, wxWidth, 90);
	c->height.PercentOf  (this, wxHeight, 40);
	m_staticBox->SetConstraints(c);


	long textRowSize = 30;
	long initialX = 50;
	long initialY = 25;


}
ConfigGeneralPanel::ConfigGeneralPanel( wxFrame *frame, int x, int y, int w, int h )
       : wxPanel( frame, -1, wxPoint(x, y), wxSize(w, h) )
{
	wxLayoutConstraints *c = new wxLayoutConstraints;
	m_staticBox = new wxStaticBox( this, -1, "General Settings", wxPoint(0,0), wxSize(w,h));
	c->centreX.SameAs    (this, wxCentreX);
	c->left.SameAs       (this, wxLeft);
	c->top.SameAs        (this, wxTop);
	c->width.PercentOf   (this, wxWidth, 90);
	c->height.PercentOf  (this, wxHeight, 40);

	m_staticBox->SetConstraints(c);

	long textRowSize = 30;
	long startX = 25;
	long initialX = startX;
	long initialY = 25;
	long textWidth = 75;
	long textHeight = 25;
	long columnWidth = 100;


	int i = 0;
	m_BitrateStatic = new wxStaticText(this, -1, "Bitrate (kbps)", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	m_BitrateNomStatic = new wxStaticText(this, -1, "Avg", wxPoint(initialX+columnWidth, initialY+(textRowSize*i)-15), wxDefaultSize, wxALIGN_LEFT );
	m_BitrateMinStatic = new wxStaticText(this, -1, "Min", wxPoint(initialX+columnWidth+50, initialY+(textRowSize*i)-15), wxDefaultSize, wxALIGN_LEFT );
	m_BitrateMaxStatic = new wxStaticText(this, -1, "Max", wxPoint(initialX+columnWidth+100, initialY+(textRowSize*i)-15), wxDefaultSize, wxALIGN_LEFT );
	m_BitrateInfoStatic = new wxStaticText(this, -1, "Min/Max used only for \nVBR (LAME) or Bitrate \nManagement (Vorbis)", wxPoint(initialX+columnWidth+150, initialY+(textRowSize*i)-15), wxDefaultSize, wxALIGN_LEFT );
	i++;
	m_SampleRateStatic = new wxStaticText(this, -1, "Sample Rate", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
	m_ChannelsStatic = new wxStaticText(this, -1, "Channels", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;


	i = 2;
	initialX = startX + columnWidth + textWidth + 20;
	m_EncodeTypeStatic = new wxStaticText(this, -1, "Encoder Type", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );

	initialX = startX + columnWidth ;
	i = 0;
	m_BitrateNomCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth/2, textHeight), 0);
	m_BitrateMinCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX+50, initialY+(textRowSize*i)), wxSize(textWidth/2, textHeight), 0);
	m_BitrateMaxCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX+100, initialY+(textRowSize*i)), wxSize(textWidth/2, textHeight), 0);
	i++;
	m_SampleRateCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), 0);
	i++;
	m_ChannelsCtrl = new wxComboBox(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), 0);
	i++;

	i = 2;
	initialX = startX + columnWidth + textWidth + 20 + columnWidth;
	m_EncodeTypeCtrl = new wxComboBox(this, ID_ENCODE_TYPE, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), 0);


	setTextCtrl(m_BitrateNomCtrl, LONG_TYPE, (void *)g.currentBitrate);
	setTextCtrl(m_BitrateMinCtrl, LONG_TYPE, (void *)g.currentBitrateMin);
	setTextCtrl(m_BitrateMaxCtrl, LONG_TYPE, (void *)g.currentBitrateMax);
	setTextCtrl(m_SampleRateCtrl, LONG_TYPE, (void *)g.currentSamplerate);
	wxArrayInt l_array;

	l_array.Add(1);
	l_array.Add(2);
	setComboBoxInt(m_ChannelsCtrl, &l_array, g.currentChannels);
	
	wxArrayString l_array2;

	l_array2.Add("OGG");
	l_array2.Add("LAME");
#ifdef WIN32
	l_array2.Add("WMA");
#endif
	setComboBoxString(m_EncodeTypeCtrl, &l_array2, g.gEncodeType);

}
ConfigEncodePanel::ConfigEncodePanel( wxFrame *frame, int x, int y, int w, int h )
       : wxPanel( frame, -1, wxPoint(x, y), wxSize(w, h),wxTAB_TRAVERSAL|wxCLIP_CHILDREN )
{
	wxLayoutConstraints *c = new wxLayoutConstraints;

	m_notebook = new wxNotebook(this, ID_NOTEBOOK);
	InitTabView(m_notebook, this);

	Init();
	if (!strcmp(g.gEncodeType, "OGG")) {
		m_notebook->SetSelection(1);
	}
	if (!strcmp(g.gEncodeType, "LAME")) {
		m_notebook->SetSelection(0);
	}
#ifdef WIN32
	if (!strcmp(g.gEncodeType, "WMA")) {
		m_notebook->SetSelection(2);
	}
#endif
}

void ConfigEncodePanel::TabChanged(wxNotebookEvent *event)
{
	char	buf[255] = "";
	sprintf(buf, "Tab %d clicked", event->GetSelection());
	//wxMessage(buf);
}
void ConfigEncodePanel::TabChanging(wxNotebookEvent *event)
{
//	wxMessage("Tab" + event->GetSelection() + " Clicked");
}
void ConfigEncodePanel::Init()
{
//    m_panel = new wxTabbedPanel(this, -1);
  
	// Note, omit the wxTAB_STYLE_COLOUR_INTERIOR, so we will guarantee a match
	// with the panel background, and save a bit of time.
}

ConfigServerPanel::ConfigServerPanel( wxFrame *frame, int x, int y, int w, int h )
       : wxPanel( frame, -1, wxPoint(x, y), wxSize(w, h) )
{
	wxOddcastConfigFrame *pOddcastFrame = (wxOddcastConfigFrame *)frame;
	wxLayoutConstraints *c = new wxLayoutConstraints;

	c->centreX.SameAs    (this, wxCentreX);
	c->left.SameAs       (this, wxLeft);
	c->top.SameAs        (this, wxTop);
	c->width.PercentOf   (this, wxWidth, 90);
	c->height.PercentOf  (this, wxHeight, 40);

	int frameWidth = 0;
	int frameSize = 0;

	frame->GetSize(&frameWidth, &frameSize);


	long textRowSize = 30;
	long startX = 25;
	long initialX = startX;
	long initialY = 25;
	long textWidth = 100;
	long textHeight = 25;
	long columnWidth = 100;

	m_staticBox = new wxStaticBox( this, -1, "Server Settings", wxPoint(0,0), wxSize(w,h));
	c->centreX.SameAs    (this, wxCentreX);
	c->left.SameAs       (this, wxLeft);
	c->top.SameAs        (this, wxTop);
	c->width.PercentOf   (this, wxWidth, 90);
	c->height.PercentOf  (this, wxHeight, 40);

	m_staticBox->SetConstraints(c);


	int i = 0;
	if (!pOddcastFrame->m_outputOnlyFlag) {
		m_InputURLStatic = new wxStaticText(this, -1, "Input URL", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
		i++;
	}
#ifdef WIN32
#define RECMSG "Record from Windows Recording Device"
#else
#define RECMSG "Record from /dev/dsp"
#endif
	m_LineInStatic = new wxStaticText(this, -1, RECMSG, wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth+100, textHeight), wxALIGN_LEFT );
	i++;
	m_ServerTypeStatic = new wxStaticText(this, -1, "Server Type", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
	m_ServerStatic = new wxStaticText(this, -1, "Server", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
	m_PortStatic = new wxStaticText(this, -1, "Port", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
	m_EncodePasswordStatic = new wxStaticText(this, -1, "Encoder Password", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
	m_ServerGenreStatic = new wxStaticText(this, -1, "Genre", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
	m_MountpointStatic = new wxStaticText(this, -1, "Mountpoint", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
	m_StreamURLStatic = new wxStaticText(this, -1, "Stream URL", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
	m_ServerDescStatic = new wxStaticText(this, -1, "Server Desc", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;

	i = 2;
	initialX = startX + columnWidth + textWidth + 20;

	m_ReconnectSecondsStatic = new wxStaticText(this, -1, "Reconnect Secs", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
#ifndef WIN32
	m_LiveRecDeviceStatic = new wxStaticText(this, -1, "Live Rec Device", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
#endif
	m_LiveRecSamplerateStatic = new wxStaticText(this, -1, "Live Rec Samplerate", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
	m_PublicStatic = new wxStaticText(this, -1, "Public Server", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;
	m_ReconnectStatic = new wxStaticText(this, -1, "Reconnect", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT );
	i++;


	initialX = startX + columnWidth ;

	i = 0;
	if (!pOddcastFrame->m_outputOnlyFlag) {
		m_InputURLCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth+200, textHeight), 0);
		i++;
	}
	m_LineInCtrl = new wxCheckBox(this, -1, "", wxPoint(initialX+100, initialY+(textRowSize*i)-5), wxSize(textWidth, textHeight), 0);
	i++;
	m_ServerTypeCtrl = new wxComboBox(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), 0);
	i++;
	m_ServerCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), 0);
	i++;
	m_PortCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), 0);
	i++;
	m_EncodePasswordCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), 0);
	i++;
	m_ServerGenreCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), 0);
	i++;
	m_MountpointCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), 0);
	i++;
	m_StreamURLCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth+150, textHeight), 0);
	i++;
	m_ServerDescCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth+150, textHeight), 0);
	i++;

	i = 2;
	initialX = startX + columnWidth + textWidth + 20 + columnWidth;

	m_ReconnectSecondsCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), 0);
	i++;
#ifndef WIN32
	m_LiceRecDeviceCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), 0);
	i++;
#endif
	m_LiveRecSamplerateCtrl = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), 0);
	i++;
	m_PublicCtrl = new wxCheckBox(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), 0);
	i++;
	m_ReconnectCtrl = new wxCheckBox(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), 0);
	i++;


	
	if (!pOddcastFrame->m_outputOnlyFlag) {
		setTextCtrl(m_InputURLCtrl, STRING_TYPE, (void *)g.gSourceURL);
	}
	setCheckBoxCtrl(m_LineInCtrl, g.gLiveRecordingFlag);

	wxArrayString s_array;

	s_array.Add("Icecast");
	s_array.Add("Icecast2");
	s_array.Add("Shoutcast");
#ifdef WIN32
	s_array.Add("WMA");
#endif
	setComboBoxString(m_ServerTypeCtrl, &s_array, g.gServerType);

	setTextCtrl(m_ServerCtrl, STRING_TYPE, (void *)g.gServer);
	setTextCtrl(m_PortCtrl, STRING_TYPE, (void *)g.gPort);
	setTextCtrl(m_EncodePasswordCtrl, STRING_TYPE, (void *)g.gPassword);
	setTextCtrl(m_ServerGenreCtrl, STRING_TYPE, (void *)g.gServGenre);
	setTextCtrl(m_MountpointCtrl, STRING_TYPE, (void *)g.gMountpoint);
	setTextCtrl(m_StreamURLCtrl, STRING_TYPE, (void *)g.gServURL);
	setTextCtrl(m_ServerDescCtrl, STRING_TYPE, (void *)g.gServDesc);
	setTextCtrl(m_ReconnectSecondsCtrl, LONG_TYPE, (void *)g.gReconnectSec);
#ifndef WIN32
	setTextCtrl(m_LiceRecDeviceCtrl, STRING_TYPE, (void *)g.gAdvRecDevice);
#endif
	setTextCtrl(m_LiveRecSamplerateCtrl, LONG_TYPE, (void *)g.gLiveInSamplerate);
	setCheckBoxCtrl(m_PublicCtrl, g.gPubServ);
	setCheckBoxCtrl(m_ReconnectCtrl, g.gAutoReconnect);
	

}
ConfigPanel::~ConfigPanel()
{
}
ConfigGeneralPanel::~ConfigGeneralPanel()
{
}
ConfigEncodePanel::~ConfigEncodePanel()
{
}
ConfigServerPanel::~ConfigServerPanel()
{
}

ConfigLameOptionPanel::~ConfigLameOptionPanel()
{
}
ConfigVorbisOptionPanel::~ConfigVorbisOptionPanel()
{
}
ConfigWMAOptionPanel::~ConfigWMAOptionPanel()
{
}

void ConfigEncodePanel::InitTabView(wxNotebook *notebook, wxWindow *window) 
{
	/*
  view->CalculateTabWidth(3, TRUE);

  if (!view->AddTab(TAB_LAME,        wxString("Lame")))
    return;
  if (!view->AddTab(TAB_VORBIS,        wxString("OggVorbis")))
    return;
  if (!view->AddTab(TAB_WMA,  wxString("WMA")))
    return;
*/
	int cw, ch;
    GetClientSize(& cw, & ch);

//	int tabHeight = view->GetTotalTabHeight();
		int tabHeight = 100;

	wxRect rect;
	rect.x = 4;
	rect.y = tabHeight + 4;
	rect.width = cw - 8;
	rect.height = ch - 4 - rect.y - 30; // 30 for buttons

  // Add some panels
  m_LameOptionPanel = new ConfigLameOptionPanel(notebook, ID_LAME_OPTIONS, 0,0,0,0);
  m_VorbisOptionPanel = new ConfigVorbisOptionPanel(notebook, ID_VORBIS_OPTIONS, 0,0,0,0);
#ifdef WIN32
  m_WMAOptionPanel = new ConfigWMAOptionPanel(notebook, ID_WMA_OPTIONS, 0,0,0,0);
#endif


  notebook->AddPage(m_LameOptionPanel, "Lame", TRUE);
  notebook->AddPage(m_VorbisOptionPanel, "OggVorbis", TRUE);
#ifdef WIN32
  notebook->AddPage(m_WMAOptionPanel, "WMA", TRUE);
#endif
/*
  view->AddTabWindow(TAB_LAME, m_LameOptionPanel);
  view->AddTabWindow(TAB_VORBIS, m_VorbisOptionPanel);
  view->AddTabWindow(TAB_WMA, m_WMAOptionPanel);
  view->SetTabSelection(TAB_LAME);
  */
}

