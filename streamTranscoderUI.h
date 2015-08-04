class MyThread : public wxThread
{
public:
    MyThread();

    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();


public:
    size_t   m_count;
	int		m_exit;
    
};

class MainPanel: public wxPanel
{
public:
    MainPanel(wxFrame *frame, int x, int y, int w, int h);
    virtual ~MainPanel();

    void OnSize( wxSizeEvent& event );

	wxStaticBox		*m_staticBox;
    wxStaticText  *inputURL_label;
	wxTextCtrl	  *inputURL;
    wxStaticText  *streamName_label;
	wxTextCtrl	  *streamName;
    wxStaticText  *outputURL_label;
	wxTextCtrl	  *outputURL;
    wxStaticText  *metaData_label;
	wxTextCtrl	  *metaData;

private:
    wxLog *m_logTargetOld;

    DECLARE_EVENT_TABLE()
};
class OutputPanel: public wxPanel
{
public:
    OutputPanel(wxFrame *frame, int x, int y, int w, int h);
    virtual ~OutputPanel();

    void OnSize( wxSizeEvent& event );

	wxStaticBox		*m_staticBox;
    wxStaticText  *status_label;
	wxTextCtrl	  *status;
    wxStaticText  *totalRead_label;
	wxTextCtrl	  *totalRead;
    wxStaticText  *bpsRead_label;
	wxTextCtrl	  *bpsRead;
    wxStaticText  *serverType_label;
	wxTextCtrl	  *serverType;
    wxStaticText  *streamType_label;
	wxTextCtrl	  *streamType;
    wxStaticText  *bitrate_label;
	wxTextCtrl	  *bitrate;

private:
    wxLog *m_logTargetOld;

    DECLARE_EVENT_TABLE()
};

class InputPanel: public wxPanel
{
public:
    InputPanel(wxFrame *frame, int x, int y, int w, int h);
    virtual ~InputPanel();

    void OnSize( wxSizeEvent& event );

	wxStaticBox		*m_staticBox;
    wxStaticText  *status_label;
	wxTextCtrl	  *status;
    wxStaticText  *totalRead_label;
	wxTextCtrl	  *totalRead;
    wxStaticText  *bpsRead_label;
	wxTextCtrl	  *bpsRead;
    wxStaticText  *serverType_label;
	wxTextCtrl	  *serverType;
    wxStaticText  *streamType_label;
	wxTextCtrl	  *streamType;
    wxStaticText  *bitrate_label;
	wxTextCtrl	  *bitrate;


private:
    wxLog *m_logTargetOld;

    DECLARE_EVENT_TABLE()
};
class ButtonPanel: public wxPanel
{
public:
    ButtonPanel(wxFrame *frame, int x, int y, int w, int h);
    virtual ~ButtonPanel();

    void OnSize( wxSizeEvent& event );
	void OnConnect( wxCommandEvent &event );
	void OnVU( wxCommandEvent &event );
    void OnPaint(wxPaintEvent& event);

	wxStaticBox		*m_staticBox;
    wxStaticText  *status_label;
	wxTextCtrl	  *status;
	wxButton		*connectButton;
	wxButton		*vuButton;
	MyThread	*execThread;
	int			m_VU;
	int vuX;
	int vuY;
	int vuWidth;
	int vuHeight;



private:
    wxLog *m_logTargetOld;

    DECLARE_EVENT_TABLE()
};

// Define a new application
class MyApp: public wxApp
{
public:
    MyApp();
    bool OnInit();

	void setMainStatus(char *pValue);
	void setMainStreamURL(char *pValue);
	void setMainStreamName(char *pValue);
	void setMainMetadata(char *pValue);
	void setMainOutputURL(char *pValue);
	void setInputStatus(char *pValue);
	void setInputTotalRead(char *pValue);
	void setInputBPSRead(char *pValue);
	void setInputServerType(char *pValue);
	void setInputStreamType(char *pValue);
	void setInputBitrate(char *pValue);
	void setOutputStatus(char *pValue);
	void setOutputTotalRead(char *pValue);
	void setOutputBPSRead(char *pValue);
	void setOutputServerType(char *pValue);
	void setOutputStreamType(char *pValue);
	void setOutputBitrate(char *pValue);

};

// Define a new frame
class MyTextWindow;
class MyWindow;

class MyFrame: public wxFrame
{
public:
    MainPanel *mainPanel;
	InputPanel *inputPanel;
	OutputPanel *outputPanel;
	ButtonPanel *buttonPanel;

    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    void OnSize(wxSizeEvent& event);
    void Draw(wxDC& dc, bool draw_bitmaps = TRUE);
    


    void EditConfig(wxCommandEvent& event);
    void Quit(wxCommandEvent& event);
    void About(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

// Define a new text subwindow that can respond to drag-and-drop
class MyTextWindow: public wxTextCtrl
{
public:
    MyTextWindow(wxFrame *frame, int x=-1, int y=-1, int width=-1, int height=-1,
               long style=wxTE_MULTILINE):
    wxTextCtrl(frame, -1, "", wxPoint(x, y), wxSize(width, height), style)
    {
    }

};

// Define a new canvas which can receive some events
class MyWindow: public wxWindow
{
public:
    MyWindow(wxFrame *frame, int x, int y, int w, int h, long style = wxRETAINED);
    ~MyWindow();
    void OnPaint(wxPaintEvent& event);

private:    
    DECLARE_EVENT_TABLE()
};


#define LAYOUT_QUIT       100
#define LAYOUT_TEST       101
#define LAYOUT_ABOUT      102
#define LAYOUT_EDIT_CONFIG  103
#define LAYOUT_TEST_SIZER 104
#define LAYOUT_TEST_NB    105

