// GPM_Monitor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GPM_AHRS.h"
#include "GPM_Monitor.h"
#include "GPM_MonitorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGPM_MonitorApp

BEGIN_MESSAGE_MAP(CGPM_MonitorApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CGPM_MonitorApp construction

CGPM_MonitorApp::CGPM_MonitorApp()
{
	EnableHtmlHelp();

	m_pDiscoverThread = NULL;
	m_pSimulinkThread = NULL;
}


// The one and only CGPM_MonitorApp object

CGPM_MonitorApp theApp;


// CGPM_MonitorApp initialization

BOOL CGPM_MonitorApp::InitInstance()
{

	HWND GPMWND;
	if ((GPMWND = FindWindow("#32770", "Global Positioning Monitor")) != 0) {
		// window exists; another instance of this
		// application is already running -> exit
		// perhaps should refresh:
		SetForegroundWindow(GPMWND);
		return false;
	}

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("TEC"));

	CGPM_MonitorDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
