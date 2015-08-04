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

#include <wx/config.h>
#include <wx/confbase.h>
#include <wx/fileconf.h>
#ifdef WIN32
#include <wx/msw/regconf.h>
#include <wx/msw/iniconf.h> 
#endif

#if !wxUSE_CONSTRAINTS
#error You must set wxUSE_CONSTRAINTS to 1 in setup.h!
#endif

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #define USE_XPM
#endif

#ifdef USE_XPM
    #include "transcoder.xpm"
#endif

#include <ctype.h>

//#include "wx/statline.h"

#include "streamTranscoderUI.h"
#include "liboddcast_wxconfig.h"


#include "log.h"
#include "transcurl.h"
#include "liboddcast.h"
#include "transcode.h"

#define ID_CONNECT_BUTTON	100
#define ID_VU_BUTTON	101

transcodeGlobals	globals;
char	*gConfigFile = NULL;

// Declare two frames
MyFrame   *frame = (MyFrame *) NULL;
wxOddcastConfigFrame *configFrame;

wxButton	*gConnectButton = NULL;

wxMenuBar *menu_bar = (wxMenuBar *) NULL;

IMPLEMENT_APP(MyApp)
DECLARE_APP(MyApp)

int	gWxIsRunning = 0;
int	gAvgLeft = 0;
int	gAvgRight = 0;

void stMessage(char *message)
{
	wxMessageDialog	mess(NULL, message, "Message", wxOK);
	mess.ShowModal();

}
void VUCallback(int leftMax, int rightMax) {
//	wxRect updateRect(frame->buttonPanel->vuX, frame->buttonPanel->vuY, frame->buttonPanel->vuWidth, frame->buttonPanel->vuHeight);

	gAvgLeft = leftMax;
	gAvgRight = rightMax;


//	frame->buttonPanel->Refresh(FALSE, &updateRect);
	

}

void mainStatusCallback(void *pValue) {
	// pValue is a string
	MyApp	*pApp;

	pApp = &wxGetApp();

	pApp->setMainStatus((char *)pValue);
}
void inputStatusCallback(void *pValue) {
	// pValue is a string
	MyApp	*pApp;

	pApp = &wxGetApp();

	pApp->setInputStatus((char *)pValue);

}
void readBytesCallback(void *pValue) {
	// pValue is a long
	static	long	startTime = 0;	
	static	long	endTime = 0;	
	static	long	bytesReadInterval = 0;
	char	kBPSstr[25] = "";

	long	bytesRead = (long)pValue;

	MyApp	*pApp;

	pApp = &wxGetApp();

	if (bytesRead == -1) {
		sprintf(kBPSstr, "");
		pApp->setInputTotalRead((char *)kBPSstr);
		globals.totalBytesRead = 0;
		pApp->setInputBPSRead((char *)kBPSstr);
		startTime = 0;
		return;
	}

	if (startTime == 0) {
		startTime = time(&startTime);
		bytesReadInterval = 0;
	}
	bytesReadInterval += bytesRead;
	globals.totalBytesRead += bytesRead;
	endTime = time(&endTime);
	if ((endTime - startTime) > 5) {
		int bytespersec = bytesReadInterval/(endTime - startTime);
		long kBPS = (bytespersec * 8)/1000;
		sprintf(kBPSstr, "%d Kbps", kBPS);
		pApp->setInputBPSRead((char *)kBPSstr);
		startTime = 0;
	}

	sprintf(kBPSstr, "%d bytes", globals.totalBytesRead);
	pApp->setInputTotalRead((char *)kBPSstr);
}
void inputServerTypeCallback(void *pValue) {
	// pValue is a string
	MyApp	*pApp;

	pApp = &wxGetApp();
	pApp->setInputServerType((char *)pValue);
}
void inputStreamTypeCallback(void *pValue) {
	// pValue is a string
	MyApp	*pApp;

	pApp = &wxGetApp();
	pApp->setInputStreamType((char *)pValue);
}
void inputStreamURLCallback(void *pValue) {
	// pValue is a string
	MyApp	*pApp;

	pApp = &wxGetApp();
	pApp->setMainStreamURL((char *)pValue);
}
void inputStreamNameCallback(void *pValue) {
	// pValue is a string
	MyApp	*pApp;

	pApp = &wxGetApp();
	pApp->setMainStreamName((char *)pValue);
}
void inputMetadataCallback(void *pValue) {
	// pValue is a string
	MyApp	*pApp;

	pApp = &wxGetApp();
	pApp->setMainMetadata((char *)pValue);
}
void inputBitrateCallback(void *pValue) {
	// pValue is a string
	MyApp	*pApp;

	pApp = &wxGetApp();
	pApp->setInputBitrate((char *)pValue);
}
void outputStatusCallback(void *pValue) {
	// pValue is a string
	MyApp	*pApp;

	pApp = &wxGetApp();
	pApp->setOutputStatus((char *)pValue);
}
void writeBytesCallback(void *pValue) {
	// pValue is a long
	static	long	startTime = 0;	
	static	long	endTime = 0;	
	static	long	bytesWrittenInterval = 0;
	char	kBPSstr[25] = "";

	long	bytesWritten = (long)pValue;
	MyApp	*pApp;

	pApp = &wxGetApp();

	if (bytesWritten == -1) {
		sprintf(kBPSstr, "");
		pApp->setOutputTotalRead((char *)kBPSstr);
		globals.totalBytesWritten = 0;
		pApp->setOutputBPSRead((char *)kBPSstr);
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
		sprintf(kBPSstr, "%d Kbps", kBPS);
		pApp->setOutputBPSRead((char *)kBPSstr);
		startTime = 0;
	}

	sprintf(kBPSstr, "%d bytes", globals.totalBytesWritten);
	pApp->setOutputTotalRead((char *)kBPSstr);
}
void outputServerTypeCallback(void *pValue) {
	// pValue is a string
	MyApp	*pApp;

	pApp = &wxGetApp();
	pApp->setOutputServerType((char *)pValue);
}
void outputStreamTypeCallback(void *pValue) {
	// pValue is a string
	MyApp	*pApp;

	pApp = &wxGetApp();
	pApp->setOutputStreamType((char *)pValue);
}
void outputBitrateCallback(void *pValue) {
	// pValue is a string
	MyApp	*pApp;

	pApp = &wxGetApp();
	pApp->setOutputBitrate((char *)pValue);
}
void outputStreamURLCallback(void *pValue) {
	// pValue is a string
	MyApp	*pApp;

	pApp = &wxGetApp();
	pApp->setMainOutputURL((char *)pValue);
}

int transcode_init(int argc, char **argv)
{
	char	*serverURL = NULL;
	char	*defaultConfigFile = "transcoder.cfg";
	int 	c = 0;
	int	printConfig = 0;
	
	memset(&globals, '\000', sizeof(globals));

	// Setup Callbacks for all the interaction
	globals.mainStatusCallback = mainStatusCallback;
	globals.inputStatusCallback = inputStatusCallback;
	globals.inputStreamURLCallback = inputStreamURLCallback;
	globals.readBytesCallback = readBytesCallback;
	globals.inputServerTypeCallback = inputServerTypeCallback;
	globals.inputStreamTypeCallback = inputStreamTypeCallback;
	globals.inputStreamURLCallback = inputStreamURLCallback;
	globals.inputStreamNameCallback = inputStreamNameCallback;
	globals.inputMetadataCallback = inputMetadataCallback;
	globals.inputBitrateCallback = inputBitrateCallback;
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
	if (printConfig) {
		printConfigFileValues();
	}

	if (globals.gdumpData) {
		globals.outFromServer = fopen("fromServer.mp3", "wb");
		globals.outToEncoder = fopen("toEncoder.pcm", "wb");
		if (getOggFlag()) {
			globals.outToServer = fopen("toServer.ogg", "wb");
		}
		else {
			globals.outToServer = fopen("toServer.mp3", "wb");
		}
		setDumpData(globals.gdumpData);
		setoutToServer(globals.outToServer);
	}

	
	return 1;
}

void MyApp::setMainStatus(char *pValue)
{
	wxString	value(pValue);

	frame->buttonPanel->status_label->SetLabel(value);
}

void MyApp::setMainStreamURL(char *pValue)
{
	wxString	value(pValue);

	frame->mainPanel->inputURL->SetValue(value);
}
void MyApp::setMainStreamName(char *pValue)
{
	wxString	value(pValue);

	frame->mainPanel->streamName->SetValue(value);
}
void MyApp::setMainMetadata(char *pValue)
{
	wxString	value(pValue);

	frame->mainPanel->metaData->SetValue(value);
}
void MyApp::setMainOutputURL(char *pValue)
{
	wxString	value(pValue);

	frame->mainPanel->outputURL->SetValue(value);
}
void MyApp::setInputStatus(char *pValue)
{
	wxString	value(pValue);

	frame->inputPanel->status->SetValue(value);
}
void MyApp::setInputTotalRead(char *pValue)
{
	wxString	value(pValue);

	frame->inputPanel->totalRead->SetValue(value);
}
void MyApp::setInputBPSRead(char *pValue)
{
	wxString	value(pValue);

	frame->inputPanel->bpsRead->SetValue(value);
}
void MyApp::setInputServerType(char *pValue)
{
	wxString	value(pValue);

	frame->inputPanel->serverType->SetValue(value);
}
void MyApp::setInputStreamType(char *pValue)
{
	wxString	value(pValue);

	frame->inputPanel->streamType->SetValue(value);
}
void MyApp::setInputBitrate(char *pValue)
{
	wxString	value(pValue);

	frame->inputPanel->bitrate->SetValue(value);
}
void MyApp::setOutputStatus(char *pValue)
{
	wxString	value(pValue);

	frame->outputPanel->status->SetValue(value);
}
void MyApp::setOutputTotalRead(char *pValue)
{
	wxString	value(pValue);

	frame->outputPanel->totalRead->SetValue(value);
}
void MyApp::setOutputBPSRead(char *pValue)
{
	wxString	value(pValue);

	frame->outputPanel->bpsRead->SetValue(value);
}
void MyApp::setOutputServerType(char *pValue)
{
	wxString	value(pValue);

	frame->outputPanel->serverType->SetValue(value);
}
void MyApp::setOutputStreamType(char *pValue)
{
	wxString	value(pValue);

	frame->outputPanel->streamType->SetValue(value);
}
void MyApp::setOutputBitrate(char *pValue)
{
	wxString	value(pValue);

	frame->outputPanel->bitrate->SetValue(value);
}
MyApp::MyApp()
{
}

bool MyApp::OnInit()
{
  // Create the main frame window
	long basePanelHeight = 300;
	long basePanelWidth = 300;

	long	mainPanelWidth = basePanelWidth*2;
	long	mainPanelHeight = basePanelHeight/2;
	long	mainPanelX = 0;
	long	mainPanelY = 0;

	long	inputPanelWidth = basePanelWidth;
	long	inputPanelHeight = basePanelHeight;
	long	inputPanelX = 0;
	long	inputPanelY = mainPanelHeight;

	long	outputPanelWidth = basePanelWidth;
	long	outputPanelHeight = basePanelHeight;
	long	outputPanelX = basePanelWidth;
	long	outputPanelY = mainPanelHeight;

	long	buttonPanelWidth = basePanelWidth*2;
	long	buttonPanelHeight = (long)(mainPanelHeight/1.5);
	long	buttonPanelX = 0;
	long	buttonPanelY = outputPanelY + basePanelHeight;

	long initialWindowWidth = (mainPanelWidth)+6;
	long initialWindowHeight = (mainPanelHeight + inputPanelHeight + buttonPanelHeight)+64;


	int	lastX = 0;
	int lastY = 0;

	wxConfig *config = new wxConfig("streamTranscoder");

	wxString str;

	if ( config->Read("LastX", &str) ) {
		lastX = atoi(str.c_str());
	}
	if ( config->Read("LastY", &str) ) {
		lastY = atoi(str.c_str());
	}
	delete(config);

	frame = new MyFrame((MyFrame *) NULL, (char *) "Stream Transcoder", lastX, lastY, initialWindowWidth, initialWindowHeight);

#ifdef WIN32
	frame->SetIcon( wxICON(transcoder) );
#endif

	frame->SetAutoLayout(TRUE);

	// Give it a status line
	frame->CreateStatusBar(2, 0);

	// Make a menubar
	wxMenu *file_menu = new wxMenu;

	file_menu->Append(LAYOUT_EDIT_CONFIG, "E&dit Config",      "Edit Configuration");

	file_menu->AppendSeparator();
	file_menu->Append(LAYOUT_ABOUT, "About",                "About");
	file_menu->AppendSeparator();
	file_menu->Append(LAYOUT_QUIT, "E&xit",                "Quit program");

	menu_bar = new wxMenuBar;

	menu_bar->Append(file_menu, "&File");

	// Associate the menu bar with the frame
	frame->SetMenuBar(menu_bar);

  // Make a panel

	wxColor	colorRed(255,0,0);
	wxColor	colorGreen(0,255,0);
	wxColor	colorBlue(0,0,255);


	frame->mainPanel = new MainPanel(frame, mainPanelX, mainPanelY, mainPanelWidth, mainPanelHeight );
	frame->mainPanel->SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
	frame->inputPanel = new InputPanel(frame, inputPanelX, inputPanelY, inputPanelWidth, inputPanelHeight );
	frame->inputPanel->SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
	frame->outputPanel = new OutputPanel(frame, outputPanelX, outputPanelY, outputPanelWidth, outputPanelHeight );
	frame->outputPanel->SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
	frame->buttonPanel = new ButtonPanel(frame, buttonPanelX, buttonPanelY, buttonPanelWidth, buttonPanelHeight );
	frame->buttonPanel->SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));

	frame->Show(TRUE);

	frame->SetStatusText("Stream Transcoder");

	transcode_init(0, NULL);
	SetTopWindow(frame);
	return TRUE;
}

//-----------------------------------------------------------------
//  MyFrame
//-----------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h), wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION)
{
  mainPanel = (MainPanel *) NULL;
  inputPanel = (InputPanel *) NULL;
  outputPanel = (OutputPanel *) NULL;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(LAYOUT_EDIT_CONFIG, MyFrame::EditConfig)
  EVT_MENU(LAYOUT_QUIT, MyFrame::Quit)
  EVT_MENU(LAYOUT_ABOUT, MyFrame::About)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MainPanel, wxPanel)
END_EVENT_TABLE()
BEGIN_EVENT_TABLE(InputPanel, wxPanel)
END_EVENT_TABLE()
BEGIN_EVENT_TABLE(OutputPanel, wxPanel)
END_EVENT_TABLE()
BEGIN_EVENT_TABLE(ButtonPanel, wxPanel)
EVT_PAINT(ButtonPanel::OnPaint)
EVT_BUTTON    (ID_CONNECT_BUTTON,          ButtonPanel::OnConnect)
EVT_BUTTON    (ID_VU_BUTTON,          ButtonPanel::OnVU)
END_EVENT_TABLE()

void MyFrame::EditConfig(wxCommandEvent& WXUNUSED(event) )
{
	configFrame = new wxOddcastConfigFrame(this, "Edit Config", 50,0, 480, 670);
	configFrame->Init();
/*
#ifdef WIN32
	wxExecute("notepad transcoder.cfg", TRUE);
	transcode_init(0, NULL);
#else
	wxExecute("vi transcoder.cfg", TRUE);
	transcode_init(0, NULL);
#endif
	*/
	
}

void MyFrame::Quit(wxCommandEvent& WXUNUSED(event) )
{
	int	lastX = 0;
	int lastY = 0;

	wxConfig *config = new wxConfig("streamTranscoder");

	wxString str;

	char	lastXstr[25] = "0";
	char	lastYstr[25] = "0";


	this->GetPosition(&lastX, &lastY);

	if ((lastX < 4000) && (lastX >= 0)) {
		sprintf(lastXstr, "%d", lastX);
	}
	if ((lastY < 3000) && (lastY >= 0)) {
		sprintf(lastYstr, "%d", lastY);
	}

	str = lastXstr;
	config->Write("LastX", str);
	str = lastYstr;
	config->Write("LastY", str);

	delete(config);

    this->Close(TRUE);
}
void MyFrame::About(wxCommandEvent& WXUNUSED(event) )
{

	stMessage("streamTranscoder by oddsock\nVersion 0.1.3");
}


//-----------------------------------------------------------------
//  MyWindow
//-----------------------------------------------------------------

BEGIN_EVENT_TABLE(MyWindow, wxWindow)
    EVT_PAINT(MyWindow::OnPaint)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyWindow::MyWindow(wxFrame *frame, int x, int y, int w, int h, long style):
 wxWindow(frame, -1, wxPoint(x, y), wxSize(w, h), style)
{
}

MyWindow::~MyWindow()
{
}

// Define the repainting behaviour
void MyWindow::OnPaint(wxPaintEvent& WXUNUSED(event) )
{
  wxPaintDC dc(this);
  wxPen pen(wxColour(0,128,0), 5, wxSOLID);

  dc.SetPen(pen);

  dc.DrawLine(100, 100, 150, 100);

}

MainPanel::MainPanel( wxFrame *frame, int x, int y, int w, int h )
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

	int i = 0;
	inputURL_label = new wxStaticText(this, -1, "Input URL", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);
	i++;
	streamName_label = new wxStaticText(this, -1, "Stream Name", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);
	i++;
	metaData_label = new wxStaticText(this, -1, "MetaData", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);
	i++;
	outputURL_label = new wxStaticText(this, -1, "Output URL", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);

	initialX = 150;
	long textWidth = 400;
	long textHeight = 25;

	i = 0;
	inputURL = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxTE_READONLY);
	i++;
	streamName = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxTE_READONLY);
	i++;
	metaData = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxTE_READONLY);
	i++;
	outputURL = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxTE_READONLY);



}
InputPanel::InputPanel( wxFrame *frame, int x, int y, int w, int h )
       : wxPanel( frame, -1, wxPoint(x, y), wxSize(w, h) )
{
	wxLayoutConstraints *c = new wxLayoutConstraints;
	m_staticBox = new wxStaticBox( this, -1, "Source Server Info", wxPoint(0,0), wxSize(w,h));
	c->centreX.SameAs    (this, wxCentreX);
	c->left.SameAs       (this, wxLeft);
	c->top.SameAs        (this, wxTop);
	c->width.PercentOf   (this, wxWidth, 90);
	c->height.PercentOf  (this, wxHeight, 40);

	m_staticBox->SetConstraints(c);

	long textRowSize = 30;
	long initialX = 15;
	long initialY = 25;

	int i = 0;
	status_label = new wxStaticText(this, -1, "Status", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);
	i++;
	totalRead_label = new wxStaticText(this, -1, "Total Read", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);
	i++;
	bpsRead_label = new wxStaticText(this, -1, "BPS Read", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);
	i++;
	serverType_label = new wxStaticText(this, -1, "Server Type", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);
	i++;
	streamType_label = new wxStaticText(this, -1, "Stream Type", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);
	i++;
	bitrate_label = new wxStaticText(this, -1, "Bitrate", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);

	initialX = 80;
	long textWidth = 200;
	long textHeight = 25;

	i = 0;
	status = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxTE_READONLY);
	i++;
	totalRead = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxTE_READONLY);
	i++;
	bpsRead = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxTE_READONLY);
	i++;
	serverType = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxTE_READONLY);
	i++;
	streamType = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxTE_READONLY);
	i++;
	bitrate = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxTE_READONLY);

}
OutputPanel::OutputPanel( wxFrame *frame, int x, int y, int w, int h )
       : wxPanel( frame, -1, wxPoint(x, y), wxSize(w, h) )
{
	wxLayoutConstraints *c = new wxLayoutConstraints;
	m_staticBox = new wxStaticBox( this, -1, "Destination Server Info", wxPoint(0,0), wxSize(w,h));
	c->centreX.SameAs    (this, wxCentreX);
	c->left.SameAs       (this, wxLeft);
	c->top.SameAs        (this, wxTop);
	c->width.PercentOf   (this, wxWidth, 90);
	c->height.PercentOf  (this, wxHeight, 40);

	m_staticBox->SetConstraints(c);
	long textRowSize = 30;
	long initialX = 15;
	long initialY = 25;

	int i = 0;
	status_label = new wxStaticText(this, -1, "Status", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);
	i++;
	totalRead_label = new wxStaticText(this, -1, "Total Written", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);
	i++;
	bpsRead_label = new wxStaticText(this, -1, "BPS Written", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);
	i++;
	serverType_label = new wxStaticText(this, -1, "Server Type", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);
	i++;
	streamType_label = new wxStaticText(this, -1, "Stream Type", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);
	i++;
	bitrate_label = new wxStaticText(this, -1, "Bitrate", wxPoint(initialX, initialY+(textRowSize*i)), wxDefaultSize, wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);

	initialX = 80;
	long textWidth = 200;
	long textHeight = 25;

	i = 0;
	status = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxTE_READONLY);
	i++;
	totalRead = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxTE_READONLY);
	i++;
	bpsRead = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxTE_READONLY);
	i++;
	serverType = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxTE_READONLY);
	i++;
	streamType = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxTE_READONLY);
	i++;
	bitrate = new wxTextCtrl(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxTE_READONLY);
}

void ButtonPanel::OnPaint(wxPaintEvent& WXUNUSED(event) )
{

	int leftVUdata = gAvgLeft;
	int rightVUdata = gAvgRight;


	wxPaintDC dc(this);

	return;

	wxPen blackpen(wxColour(0,0,0), 5, wxSOLID);
	wxBrush blackbrush(wxColour(0,0,0), wxSOLID);
	wxBrush greenbrush(wxColour(0,128,0), wxSOLID);
	int lgreen = (leftVUdata < 128) ? 255 : 255 - ((leftVUdata-128) * 2);
	int lred = (leftVUdata < 128) ? 255 - ((leftVUdata) * 2) : 255;
	int rgreen = (rightVUdata < 128) ? 255 : 255 - ((rightVUdata-128) * 2);
	int rred = (rightVUdata < 128) ? 255 - ((rightVUdata) * 2) : 255;
	wxPen leftpen(wxColour(lred,lgreen,0), 5, wxSOLID);
	wxPen rightpen(wxColour(rred,rgreen,0), 5, wxSOLID);
	int leftVUWidth = (vuWidth * leftVUdata) / 256;
	int rightVUWidth = (vuWidth * rightVUdata) / 256;

	dc.SetPen(blackpen);
	dc.SetBrush(blackbrush);

	dc.DrawRectangle(vuX, vuY, vuWidth, vuHeight);
	if (m_VU) {
		dc.SetPen(leftpen);
		dc.DrawLine(vuX, vuY+5, vuX+leftVUWidth, vuY+5);
		dc.SetPen(rightpen);
		dc.DrawLine(vuX, vuY+15, vuX+rightVUWidth, vuY+15);
	}

}

void ButtonPanel::OnVU( wxCommandEvent &event )
{
	m_VU = !m_VU;
	Refresh(FALSE, NULL);

}
void ButtonPanel::OnConnect( wxCommandEvent &event )
{
//	wxMessageDialog	connectit(this, "Connecting!!!", "Message", wxOK);
//	connectit.ShowModal();

	wxBusyCursor	wxWait;

	gConnectButton = connectButton;

	if (getWMAFlag()) {
		stopWMAEncoding();
	}
	if (gWxIsRunning) {
		setForceStop(1);
		setStopTranscoder(1);
		if (getLiveRecordingSetFlag()) {
			stopRecording();
			disconnectFromServer();
		}
		// wxSleep(1);

		//execThread->Delete();

		execThread = NULL;

		globals.mainStatusCallback((char *)"Stopped");
		globals.inputStatusCallback((char *)"Disconnected");
		globals.readBytesCallback((void *)-1);
		globals.inputServerTypeCallback((char *)"");
		globals.inputStreamTypeCallback((char *)"");
		globals.inputStreamNameCallback((char *)"");
		globals.inputMetadataCallback((char *)"");
		globals.inputBitrateCallback((char *)"");
		globals.writeBytesCallback((void *)-1);

		connectButton->SetLabel("Connect");
		gWxIsRunning = 0;
	}
	else {
		setStopTranscoder(0);
		reset_transcoder();

		char	message[1024] = "";
		char	error[1024] = "";

		int ret = 1;


#ifdef WIN32
		if (getWMAFlag()) {
			ret = startWMAServer(message,error);
		}
#endif
		if (ret == 0) {
			outputStatusCallback((void *)message);
			globals.mainStatusCallback(error);

		}
		else {

			
			execThread = new MyThread();

			if ( execThread->Create() != wxTHREAD_NO_ERROR ) {
				wxLogError("Can't create thread!");
			}
			else {
				connectButton->SetLabel("Disconnect");
				gWxIsRunning = 1;
				execThread->Run();
			}
		}
		
	}

}
ButtonPanel::ButtonPanel( wxFrame *frame, int x, int y, int w, int h )
       : wxPanel( frame, -1, wxPoint(x, y), wxSize(w, h) )
{
	wxLayoutConstraints *c = new wxLayoutConstraints;

	//m_staticBox = new wxStaticBox( this, -1, "Main Server", wxPoint(10,10), wxSize(initialPanelHeight-20,initialPanelWidth-20) );
//	m_staticBox = new wxStaticBox( this, -1, "Main Server", wxPoint(0,0), wxSize(w,h));

	c->centreX.SameAs    (this, wxCentreX);
	c->left.SameAs       (this, wxLeft);
	c->top.SameAs        (this, wxTop);
	c->width.PercentOf   (this, wxWidth, 90);
	c->height.PercentOf  (this, wxHeight, 40);
//	m_staticBox->SetConstraints(c);

	vuX = 400;
	vuY = 57;
	vuWidth = 100;
	vuHeight = 20;

	// Initialize to On
	m_VU = 1;
	int frameWidth = 0;
	int frameSize = 0;

	frame->GetSize(&frameWidth, &frameSize);

	long textRowSize = 30;
	long initialX = 10;
	long initialY = 25;

	long textWidth = 600;
	long textHeight = 25;
	long buttonWidth = 75;

	int i = 0;
	status_label = new wxStaticText(this, -1, "", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight), wxALIGN_LEFT /*| wxST_NO_AUTORESIZE*/);
	i++;

//	connectButton = new wxButton(this, ID_CONNECT_BUTTON, "&Connect", wxPoint(initialX, initialY+(textRowSize*i)), wxSize(textWidth, textHeight) );
		connectButton = new wxButton(this, ID_CONNECT_BUTTON, "&Connect", wxPoint((frameWidth/2)-(buttonWidth/2), initialY+(textRowSize*i)), wxSize(buttonWidth, 25) );
//		vuButton = new wxButton(this, ID_VU_BUTTON, "Turn Encoding VU On/Off", wxPoint((frameWidth/2)-(buttonWidth/2)+105, initialY+(textRowSize*i)+22), wxSize(buttonWidth*2, 20) );
		execThread = NULL;

}
MainPanel::~MainPanel()
{
}
InputPanel::~InputPanel()
{
}
OutputPanel::~OutputPanel()
{
}
ButtonPanel::~ButtonPanel()
{
}

MyThread::MyThread()
        : wxThread()
{
    m_count = 0;
	m_exit = 0;
}

void MyThread::OnExit()
{
	m_exit = 1;
}

void *MyThread::Entry()
{
	
	if (getLiveRecordingSetFlag()) {
		if (globals.inputStatusCallback) {
			globals.inputStatusCallback((void *)"Input device opened...");
		}

		if (globals.inputStatusCallback) {
			globals.inputStatusCallback((void *)"Recording from linein");
		}
		if (globals.inputServerTypeCallback) {
			globals.inputStatusCallback((void *)"Line-In");
		}
		if (globals.inputStreamTypeCallback) {
			globals.inputStreamTypeCallback((void *)"PCM");
		}
		if (globals.inputStreamTypeCallback) {
			globals.inputStreamTypeCallback((void *)"PCM");
		}
		char	buf[255] = "";
		sprintf(buf, "%dHz/Stereo PCM", getLiveInSamplerate());

		if (globals.inputBitrateCallback) {
			globals.inputBitrateCallback(buf);
		}
		int songChange = setCurrentSongTitle(getServerDesc());
		setSourceDescription(buf);

		initializeencoder();
		if (connectToServer()) {
			setCurrentSongTitle("");
		}
		else {
			stMessage("Unable to Connect");
			return(0);
		}
		if (!initLiveRecording()) {
			
			stMessage("Fail to open input device");
			disconnectFromServer();
			return 0;
		}
		else {
			char	buf[255] = "";
			sprintf(buf, "%dHz/Stereo PCM", getLiveInSamplerate());
			setSourceDescription(buf);
			int songChange = setCurrentSongTitle(getServerDesc());
			inputStatusCallback((void *)"Recording...");
			startRecording();
		}
		return NULL;
	}
	else {

		int ret = transcode_main();
		
		gWxIsRunning = 0;
		gConnectButton->SetLabel("Connect");
	}

//	wxMessageDialog	connectit2(NULL, "End Thread!!!", "Message", wxOK);
//	connectit2.ShowModal();
	//this->Exit((void *)1);
    return NULL;
}
