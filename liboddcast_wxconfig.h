#ifndef __LIBODDCAST_WXCONFIG_H__
#define __LIBODDCAST_WXCONFIG_H__

#include <wx/tab.h>
#include <wx/notebook.h>

// Tab ids
#define TAB_LAME      1
#define TAB_VORBIS    2
#define TAB_WMA       3

class ConfigLameOptionPanel: public wxPanel
{
public:
    ConfigLameOptionPanel(wxNotebook *notebook, wxWindowID id, int x, int y, int w, int h);
    virtual ~ConfigLameOptionPanel();

    wxStaticText  *m_VBRStatic;
    wxStaticText  *m_VBRModeStatic;
    wxStaticText  *m_QualityStatic;
    wxStaticText  *m_HighpassStatic;
    wxStaticText  *m_LowpassStatic;

	wxCheckBox	  *m_VBRCtrl;
	wxComboBox	  *m_VBRModeCtrl;
	wxTextCtrl	  *m_QualityCtrl;
	wxTextCtrl	  *m_HighpassCtrl;
	wxTextCtrl	  *m_LowpassCtrl;


    void OnSize( wxSizeEvent& event );

private:

    DECLARE_EVENT_TABLE()
};
class ConfigVorbisOptionPanel: public wxPanel
{
public:
    ConfigVorbisOptionPanel(wxNotebook *notebook, wxWindowID id, int x, int y, int w, int h);
    virtual ~ConfigVorbisOptionPanel();

    void OnSize( wxSizeEvent& event );

    wxStaticText  *m_QualityBMStatic;
    wxStaticText  *m_QualityStatic;

	wxComboBox	  *m_QualityBMCtrl;
	wxTextCtrl	  *m_QualityCtrl;

private:

    DECLARE_EVENT_TABLE()
};
class ConfigWMAOptionPanel: public wxPanel
{
public:
    ConfigWMAOptionPanel(wxNotebook *notebook, wxWindowID id, int x, int y, int w, int h);
    virtual ~ConfigWMAOptionPanel();

    void OnSize( wxSizeEvent& event );

    wxStaticText  *m_ListenPortStatic;
    wxStaticText  *m_ClientsStatic;

	wxTextCtrl	  *m_ListenPortCtrl;
	wxTextCtrl	  *m_ClientsCtrl;


private:

    DECLARE_EVENT_TABLE()
};


class ConfigPanel: public wxPanel
{
public:
    ConfigPanel(wxFrame *frame, int x, int y, int w, int h);
    virtual ~ConfigPanel();

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
    

    DECLARE_EVENT_TABLE()
};
class ConfigGeneralPanel: public wxPanel
{
public:
    ConfigGeneralPanel(wxFrame *frame, int x, int y, int w, int h);
    virtual ~ConfigGeneralPanel();

    void OnSize( wxSizeEvent& event );

	wxStaticBox		*m_staticBox;
    wxStaticText  *m_SourceURLStatic;
    wxStaticText  *m_BitrateStatic;
    wxStaticText  *m_BitrateInfoStatic;
    wxStaticText  *m_BitrateMinStatic;
    wxStaticText  *m_BitrateNomStatic;
    wxStaticText  *m_BitrateMaxStatic;
    wxStaticText  *m_SampleRateStatic;
    wxStaticText  *m_ChannelsStatic;
	wxStaticText  *m_EncodeTypeStatic;
	wxTextCtrl	  *m_SourceURLCtrl;
	wxTextCtrl	  *m_BitrateMinCtrl;
	wxTextCtrl	  *m_BitrateNomCtrl;
	wxTextCtrl	  *m_BitrateMaxCtrl;
	wxTextCtrl	  *m_SampleRateCtrl;
	wxComboBox	  *m_ChannelsCtrl;
	wxComboBox	  *m_EncodeTypeCtrl;

private:
    

    DECLARE_EVENT_TABLE()
};

class ConfigEncodePanel: public wxPanel
{
public:
    ConfigEncodePanel(wxFrame *frame, int x, int y, int w, int h);
    virtual ~ConfigEncodePanel();

    void OnSize( wxSizeEvent& event );
	void InitTabView(wxNotebook* notebook, wxWindow* window);
	void Init();
	void TabChanged(wxNotebookEvent *event);
	void TabChanging(wxNotebookEvent *event);


    wxTabbedPanel*      m_panel;
    wxNotebook*     m_notebook;
	wxStaticBox		*m_staticBox;
    wxStaticText  *status_label;
	ConfigLameOptionPanel	*m_LameOptionPanel;
	ConfigVorbisOptionPanel	*m_VorbisOptionPanel;
	ConfigWMAOptionPanel	*m_WMAOptionPanel;
	

private:
    

    DECLARE_EVENT_TABLE()
};
class ConfigServerPanel: public wxPanel
{
public:
    ConfigServerPanel(wxFrame *frame, int x, int y, int w, int h);
    virtual ~ConfigServerPanel();

    void OnSize( wxSizeEvent& event );
	wxStaticBox		*m_staticBox;
    wxStaticText  *m_InputURLStatic;
	wxStaticText  *m_LineInStatic;
    wxStaticText  *m_ServerStatic;
    wxStaticText  *m_PortStatic;
    wxStaticText  *m_EncodePasswordStatic;
    wxStaticText  *m_StreamURLStatic;
    wxStaticText  *m_ServerDescStatic;
    wxStaticText  *m_ServerGenreStatic;
    wxStaticText  *m_MountpointStatic;
    wxStaticText  *m_ReconnectSecondsStatic;
    wxStaticText  *m_ServerTypeStatic;
    wxStaticText  *m_LiveRecDeviceStatic;
    wxStaticText  *m_LiveRecSamplerateStatic;
    wxStaticText  *m_PublicStatic;
    wxStaticText  *m_ReconnectStatic;

	wxTextCtrl	  *m_InputURLCtrl;
	wxCheckBox	  *m_LineInCtrl;
	wxTextCtrl	  *m_ServerCtrl;
	wxTextCtrl	  *m_PortCtrl;
	wxTextCtrl	  *m_EncodePasswordCtrl;
	wxTextCtrl	  *m_StreamURLCtrl;
	wxTextCtrl	  *m_ServerDescCtrl;
	wxTextCtrl	  *m_ServerGenreCtrl;
	wxTextCtrl	  *m_MountpointCtrl;
	wxTextCtrl	  *m_ReconnectSecondsCtrl;
	wxComboBox	  *m_ServerTypeCtrl;
	wxTextCtrl	  *m_LiceRecDeviceCtrl;
	wxTextCtrl	  *m_LiveRecSamplerateCtrl;
	wxCheckBox	  *m_PublicCtrl;
	wxCheckBox	  *m_ReconnectCtrl;


private:

    DECLARE_EVENT_TABLE()
};



class wxOddcastConfigFrame : public wxFrame
{
public:
	ConfigPanel *mainPanel;
	ConfigGeneralPanel *generalPanel;
	ConfigEncodePanel *encodePanel;
	ConfigServerPanel *serverPanel;

	wxButton	*m_OK;
	wxButton	*m_Cancel;
	int			m_outputOnlyFlag;


    wxOddcastConfigFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    void OnSize(wxSizeEvent& event);
    void Draw(wxDC& dc, bool draw_bitmaps = TRUE);
	void Init(int outputOnlyFlag = 0);
	void OnOk();
	void OnCancel();
	int TransferToGlobals();
	void OnEncodeType();
	int	 ValidateConfigValues();

private:
    DECLARE_EVENT_TABLE()
};


#endif

