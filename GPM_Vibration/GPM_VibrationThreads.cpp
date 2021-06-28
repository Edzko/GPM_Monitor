
#include "stdafx.h"
#include "GPM_Vibration.h"
#include "GPM_VibrationDlg.h"


UINT __cdecl ProcDiscoverThreadFunction(LPVOID pParam);


UINT __cdecl ProcDiscoverThreadFunction(LPVOID pParam)
{
	CGPM_VibrationDlg* dlg = (CGPM_VibrationDlg*)pParam;
	sockaddr_in si_me;
	SOCKET s;
	s = socket(AF_INET, SOCK_DGRAM, 0);   // UDP
	int port = 55555;
	char broadcast = 1;
	bool found = false;
	//setsockopt(s, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

	memset(&si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(port);
	si_me.sin_addr.s_addr = INADDR_ANY;

	int timeout = 500;
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

	bind(s, (sockaddr*)&si_me, sizeof(sockaddr));

	while (true)
	{
		char buf[10000];
		int slen = sizeof(sockaddr);
		if (recvfrom(s, buf, sizeof(buf) - 1, 0, (sockaddr*)&dlg->si_other, &slen) > 0)
		{
			char txt[100];

			sprintf_s(txt, 100, "WiFi (%i.%i.%i.%i)\r\n", dlg->si_other.sin_addr.S_un.S_un_b.s_b1,
				dlg->si_other.sin_addr.S_un.S_un_b.s_b2,
				dlg->si_other.sin_addr.S_un.S_un_b.s_b3,
				dlg->si_other.sin_addr.S_un.S_un_b.s_b4);

			// check to see if IP string is already there
			if (dlg->pCOMList->FindString(0, txt) == CB_ERR) {
				int iList = dlg->pCOMList->AddString(txt);
				dlg->pCOMList->SetItemData(iList, 100);
				found = true;
			}
		}
		Sleep(100);

		if (found) {
			CString portname = AfxGetApp()->GetProfileString("", "Port", "");
			dlg->pCOMList->SelectString(0, portname);

			//CButton* pButton = (CButton*)dlg->GetDlgItem(IDC_CONNECT);
			// automatically connect to the module
			//dlg->PostMessage(WM_COMMAND, MAKEWPARAM(IDC_CONNECT, BN_CLICKED), (LPARAM)pButton->m_hWnd);

			found = false;
		}
	}

	//AfxEndThread(0);

	//return false;
}



