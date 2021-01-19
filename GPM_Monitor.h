// GPM_Monitor.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CGPM_MonitorApp:
// See GPM_Monitor.cpp for the implementation of this class
//

class CGPM_MonitorApp : public CWinApp
{
public:
	CGPM_MonitorApp();
	CWinThread *m_pDiscoverThread, *m_pServerThread;

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CGPM_MonitorApp theApp;
