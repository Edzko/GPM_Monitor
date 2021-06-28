// GPM_Vibration.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CGPM_VibrationApp:
// See GPM_Vibration.cpp for the implementation of this class
//

class CGPM_VibrationApp : public CWinApp
{
public:
	CGPM_VibrationApp();
	CWinThread *m_pDiscoverThread, *m_pServerThread, *m_pSimulinkThread;

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CGPM_VibrationApp theApp;
