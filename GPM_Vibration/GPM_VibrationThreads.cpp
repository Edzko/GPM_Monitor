
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
		char buf[1000];
		char txt[1000];
		int slen = sizeof(sockaddr);
		int n = recvfrom(s, buf, sizeof(buf) - 1, 0, (sockaddr*)&dlg->si_other, &slen);
		if (n > 0)
		{
			buf[n] = 0;
			sprintf_s(txt, 100, "%s (%i.%i.%i.%i)\r\n", buf,dlg->si_other.sin_addr.S_un.S_un_b.s_b1,
				dlg->si_other.sin_addr.S_un.S_un_b.s_b2,
				dlg->si_other.sin_addr.S_un.S_un_b.s_b3,
				dlg->si_other.sin_addr.S_un.S_un_b.s_b4);

			// check to see if IP string is already there
			if (dlg->pCOMList->FindString(0, txt) == CB_ERR) {
				int iList = dlg->pCOMList->AddString(txt);
				// add to list of modules
				dlg->pCOMList->SetItemData(iList, 100);
				TRACE1("Discovered and added IP %s\r\n",txt);
				found = true;
				for (int i = 0; i < 10; i++) 
					if (strcmp(dlg->vm[i].ip, txt) == 0) 
						found = false;
				if (found) {
					int i;
					for (i = 0; i < 10; i++) {
						if (strlen(dlg->vm[i].ip) == 0) {
							strcpy_s(dlg->vm[i].ip, 100, txt);
							break;
						}
					}
					if (dlg->pCOMList->GetCount() == 1) {
						dlg->cim = i; // set as current 
						dlg->pCOMList->SetCurSel(0);
					}
					CString portname = AfxGetApp()->GetProfileString("", "Port", "");
					if (dlg->pCOMList->SelectString(0, portname) >= 0) {
						for (i = 0; i < 10; i++) {
							if (strcmp(dlg->vm[i].ip, portname) == 0) {
								dlg->cim = i; // set as current 
								break;
							}
						}
					}
				}
			}
		}
		found = false;
		Sleep(100);
	}

	//AfxEndThread(0);

	//return false;
}



