// GPM_USBinterface.cpp : Defines the initialization routines for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "GPM_USBinterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

CONFIG_DATA cfgdata;

// CGPMUSBinterfaceApp

BEGIN_MESSAGE_MAP(CGPMUSBinterfaceApp, CWinApp)
END_MESSAGE_MAP()


// CGPMUSBinterfaceApp construction

CGPMUSBinterfaceApp::CGPMUSBinterfaceApp()
{
	strcpy_s((char*)cfgdata.pm_name, 20, "pm_edzko");
}


// The one and only CGPMUSBinterfaceApp object

CGPMUSBinterfaceApp theApp;


// CGPMUSBinterfaceApp initialization

BOOL CGPMUSBinterfaceApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


typedef void(_stdcall* LPEXTFUNCRESPOND) (LPCSTR s);
typedef void(_stdcall* LPEXTFUNCBYTES) (unsigned char* s);


extern "C"
{
	__declspec(dllexport) void  __stdcall GetValue(const char* name, LPEXTFUNCRESPOND respond)
	{
		if (strcmp(name,"pm_name")==0) respond((char*)&cfgdata.pm_name);
		if (strcmp(name, "mqtt_topic") == 0) respond((char*)&cfgdata.mqtt_topic);

	}
}
extern "C"
{
	__declspec(dllexport) void __stdcall GetData(const char* str, LPEXTFUNCBYTES respond)
	{
		// Input is in str
		// Put your response in respond()
		respond(&cfgdata.version);
	}
}

extern "C"
{
	__declspec(dllexport) void __stdcall SetData(const char* str, LPEXTFUNCRESPOND respond)
	{
		// Input is in str
		// Put your response in respond()
		respond("HELLO");
	}
}

extern "C"
{
	__declspec(dllexport) void __stdcall parseSP(const char* str, LPEXTFUNCRESPOND respond)
	{
		unsigned char* data = (unsigned char*)&cfgdata.version;
		if (strncmp(str, "SP#", 3) == 0) {
			int i = 3, k = 0;
			while (k < sizeof(cfgdata))
			{
				if (str[i] < 'A') data[k] = 16 * (str[i] - '0'); else data[k] = 16 * (str[i] - 'A' + 10);
				i++;
				if (str[i] < 'A') data[k] += str[i] - '0'; else data[k] += str[i] - 'A' + 10;
				i++;
				k++;
			}

			respond("OK");
		} else
			respond("NOK");
	}
}