/* oddcastwnd.h - oddsock@oddsock.org
 * Heavily copied from :
 * srippernd.c - jonclegg@yahoo.com
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

#ifndef _ODDCASTWND_H
#define _ODDCASTWND_H

//#include "bfc/virtualwnd.h"
#include "common/contwnd.h"
#include "bfc/appcmds.h"
#include "bfc/virtualwnd.h"
#include "bfc/wnds/buttwnd.h"
#include "studio/corecb.h"
#include <string>

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
#include "liboddcast_wxconfig.h"

#define ODDCASTWND_PARENT ContWnd	//VirtualWnd

class TextBar;

class OddcastWnd : public ODDCASTWND_PARENT, 
						  public CoreCallbackI,
						  public AppCmdsI
{
public:
	OddcastWnd();
	virtual ~OddcastWnd();

	static const char *getWindowTypeName();
	static GUID getWindowTypeGuid();
	static void setIconBitmaps(ButtonWnd *button);    

	virtual int onInit();
    virtual int  onPaint(Canvas *canvas);
	virtual void timerCallback(int id);
	int reconnectServer();

	TextBar *m_titleText;
	TextBar *m_ServerDescText;
	TextBar *m_DestinationText;
	TextBar *m_MetadataText;
	TextBar *m_BPSText;
	TextBar *m_BitrateText;
	TextBar *m_StatusText;
	TextBar *m_LiveRecordingText;

	TextBar *m_titleValue;
	TextBar *m_ServerDescValue;
	TextBar *m_DestinationValue;
	TextBar *m_MetadataValue;
	TextBar *m_BPSValue;
	TextBar *m_BitrateValue;
	TextBar *m_StatusValue;

private:
	int handleChildNotify (int msg, int objId, int param1, int param2);
	virtual int childNotify(RootWnd *which, int msg, int param1, int param2);
	virtual int corecb_onStarted();
	virtual int corecb_onTitleChange();
	virtual int corecb_onTitle2Change();
	virtual void appcmds_onCommand(int id, const RECT *buttonRect);

	void ClearTextFields();

	static OddcastWnd* m_pcurInst;

	wxOddcastConfigFrame *configFrame;

	ButtonWnd *m_connect;
	ButtonWnd *m_config;
	ButtonWnd *m_vu;

	int			m_vu_on_off;


	HINSTANCE m_hInst;
	HWND m_hWnd;

};


#endif _ODDCASTWND_H
