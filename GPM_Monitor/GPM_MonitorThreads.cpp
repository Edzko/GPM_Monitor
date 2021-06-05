
#include "stdafx.h"
#include "GPM_AHRS.h"
#include "GPM_Monitor.h"
#include "GPM_MonitorDlg.h"


UINT __cdecl ProcDiscoverThreadFunction(LPVOID pParam);
UINT __cdecl ProcServerThreadFunction(LPVOID pParam);
UINT __cdecl ProcSimulinkThreadFunction(LPVOID pParam);


UINT __cdecl ProcDiscoverThreadFunction(LPVOID pParam)
{
	CGPM_MonitorDlg* dlg = (CGPM_MonitorDlg*)pParam;
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

	while (!found)
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
			int iList = dlg->pCOMList->AddString(txt);
			dlg->pCOMList->SetItemData(iList, 100);

			found = true;
		}
		Sleep(100);
	}

	CString portname = AfxGetApp()->GetProfileString("", "Port", "");
	dlg->pCOMList->SelectString(0, portname);
	CButton* pButton = (CButton*)dlg->GetDlgItem(IDC_CONNECT);
	if (dlg->pAutoConnect->GetCheck())
		dlg->PostMessage(WM_COMMAND, MAKEWPARAM(IDC_CONNECT, BN_CLICKED), (LPARAM)pButton->m_hWnd);

	AfxEndThread(0);
	return false;
}

// see https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code
UINT __cdecl ProcServerThreadFunction(LPVOID pParam)
{
	CGPM_MonitorDlg* dlg = (CGPM_MonitorDlg*)pParam;

	char port[50];
	
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	int iResult;
	int iSendResult;
	char recvbuf[512];
	int recvbuflen = 512;
	SOCKET clSock;
	char svrbuf[512];
	int svrlen;
	char CommPort[100];
	int fSuccess;
	DCB dcbCommPort;
	COMMTIMEOUTS CommTimeouts;

	//int com = (int)pCom->GetItemData(pCom->GetCurSel());

	GetDlgItemText(dlg->m_hWnd, IDC_SVRPORT, port, 50);
	GetDlgItemText(dlg->m_hWnd, IDC_SVRCOMLIST, CommPort, 100);
	char* pcom = strstr(CommPort, "COM");
	if (pcom == NULL) {
		AfxEndThread(0);
		return 0;
	}
	dlg->svrcom = atoi(pcom + 3);
	// Open COM server port

	sprintf_s(CommPort, 20, "\\\\.\\COM%i", dlg->svrcom);

	HANDLE hCommPort = CreateFile(CommPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hCommPort == INVALID_HANDLE_VALUE) {
		DWORD dwError = GetLastError();
		char errMsg[500];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, dwError, 0, errMsg, 500, NULL);
		TRACE1("Error: %s\r\n", errMsg);
		AfxEndThread(0);
		return 0;
	}
	fSuccess = GetCommState(hCommPort, &dcbCommPort);
	if (!fSuccess) {
		AfxEndThread(0);
		return 0;
	}
	int baudRate = 57600;
	dcbCommPort.DCBlength = sizeof(DCB);
	dcbCommPort.BaudRate = baudRate;
	dcbCommPort.ByteSize = 8;
	dcbCommPort.Parity = NOPARITY;
	dcbCommPort.StopBits = ONESTOPBIT;
	dcbCommPort.fInX = FALSE;
	dcbCommPort.fOutX = FALSE;
	dcbCommPort.fOutxCtsFlow = FALSE;
	dcbCommPort.fOutxDsrFlow = FALSE;
	dcbCommPort.fDtrControl = DTR_CONTROL_ENABLE;
	dcbCommPort.fRtsControl = RTS_CONTROL_ENABLE;
	fSuccess = SetCommState(hCommPort, &dcbCommPort);
	if (!fSuccess) {
		AfxEndThread(0);
		return 0;
	}

	// modify COM port settings for polling
	CommTimeouts.ReadIntervalTimeout = 10;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 100;
	CommTimeouts.WriteTotalTimeoutMultiplier = 1;
	CommTimeouts.WriteTotalTimeoutConstant = 0;
	fSuccess = SetCommTimeouts(hCommPort, &CommTimeouts);

	// Setup Network Server
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, port, &hints, &result);
	if (iResult != 0) {
		TRACE1("getaddrinfo failed with error: %d\n", iResult);
		AfxEndThread(0);
		return 0;
	}
	SOCKET svrSock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (svrSock == INVALID_SOCKET) {
		TRACE1("socket failed with error: %ld\n", WSAGetLastError());
		AfxEndThread(0);
		return 0;
	}
	int timeout = 100;
	iResult = setsockopt(svrSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

	u_long iMode = 1;
	iResult = ioctlsocket(svrSock, FIONBIO, &iMode);
	if (iResult == SOCKET_ERROR) {
		TRACE1("ioctl blocking call failed with error: %d\n", WSAGetLastError());
		closesocket(svrSock);
		AfxEndThread(0);
		return 0;
	}

	iResult = bind(svrSock, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		TRACE1("bind failed with error: %d\n", WSAGetLastError());
		closesocket(svrSock);
		AfxEndThread(0);
		return 0;
	}
	freeaddrinfo(result);
	iResult = listen(svrSock, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		TRACE1("listen failed with error: %d\n", WSAGetLastError());
		closesocket(svrSock);
	}

	svrlen = 0;
	dlg->svrrunning = true;
	dlg->SetDlgItemText(IDC_SVRCONNECT, "Disconnect");
	dlg->pSvrCOMList->EnableWindow(false);

	while (dlg->svrrunning)
	{
		clSock = accept(svrSock, NULL, NULL);
		if (clSock == INVALID_SOCKET) {
			int nErr = WSAGetLastError();
			if (nErr == WSAEWOULDBLOCK) Sleep(100);
			else {
				TRACE1("accept failed with error: %d\n", nErr);
				closesocket(clSock);
				WSACleanup();
				dlg->svrrunning = false;
				dlg->SetDlgItemText(IDC_SVRCONNECT, "Connect");
				dlg->pSvrCOMList->EnableWindow(true);
				AfxEndThread(0);
				return 0;
			}
		}


		// Receive until the peer shuts down the connection
		if (clSock != INVALID_SOCKET) {
			do {
				DWORD nc;
				iResult = recv(clSock, recvbuf, recvbuflen, 0);
				if (iResult > 0) {
					if (iResult >= 512) iResult = 511;
					recvbuf[iResult] = 0;
					TRACE("Bytes received from client: %d\n", iResult);
					// Send to GPM

					fSuccess = WriteFile(hCommPort, recvbuf, iResult, &nc, NULL);
					if (fSuccess == 0) {
						int nErr = GetLastError();
						char errMsg[250];
						FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 250, NULL);
						TRACE1("Error sending to GPM: %s\r\n", errMsg);
					}
				}
				else if (iResult == 0) {
					TRACE("Connection closing...\n");
					break;
				}
				else {
					int nErr = WSAGetLastError();
					if ((nErr != WSAEWOULDBLOCK) && (nErr != WSAETIMEDOUT))
					{
						printf("Recv from Client failed with error: %d\n", nErr);
						closesocket(clSock);
						break;
					}
				}

				iResult = ReadFile(hCommPort, svrbuf, 512, (LPDWORD)&nc, NULL);
				if (iResult == 0) {
					int nErr = GetLastError();
					char errMsg[500];
					FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
					TRACE1("Error receiving from GPM: %s\r\n", errMsg);
					iResult = 1;
				}
				if (nc > 0)
				{
					iSendResult = send(clSock, svrbuf, nc, 0);
					if (iSendResult == SOCKET_ERROR) {
						TRACE1("Send to Client failed with error: %d\n", WSAGetLastError());
						closesocket(clSock);
						break;
					}
					TRACE1("Bytes sent to Client: %d\n", iSendResult);
				}

				//Sleep(10);

			} while ((iResult != 0) && (dlg->svrrunning));

			// shutdown the connection since we're done
			iResult = shutdown(clSock, SD_SEND);
			if (iResult == SOCKET_ERROR) {
				TRACE1("shutdown failed with error: %d\n", WSAGetLastError());
				closesocket(clSock);

			}
			// cleanup
			closesocket(clSock);
		}
	}
	// No longer need server socket
	CancelIo(hCommPort);
	CloseHandle(hCommPort);
	closesocket(svrSock);
	dlg->SetDlgItemText(IDC_SVRCONNECT, "Connect");
	dlg->pSvrCOMList->EnableWindow(true);
	dlg->svrrunning = false;
	AfxEndThread(0);
	return 0;
}

UINT __cdecl ProcSimulinkThreadFunction(LPVOID pParam)
{
	CGPM_MonitorDlg* dlg = (CGPM_MonitorDlg*)pParam;

	char port[50];
	struct sockaddr_in sa;
	int iResult;
	int iSendResult;
	char recvbuf[512];
	int recvbuflen = 512;
	char svrbuf[512];
	char CommPort[100];
	int fSuccess;
	DCB dcbCommPort;
	COMMTIMEOUTS CommTimeouts;

	GetDlgItemText(dlg->m_hWnd, IDC_SVRPORT, port, 50);

	// Open COM server port
	sprintf_s(CommPort, 20, "\\\\.\\COM%i", dlg->svrcom);
	HANDLE hCommPort = CreateFile(CommPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hCommPort == INVALID_HANDLE_VALUE) {
		DWORD dwError = GetLastError();
		char errMsg[500];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, dwError, 0, errMsg, 500, NULL);
		TRACE1("Error: %s\r\n", errMsg);
		AfxEndThread(0);
		return 0;
	}
	fSuccess = GetCommState(hCommPort, &dcbCommPort);
	if (!fSuccess) {
		AfxEndThread(0);
		return 0;
	}
	int baudRate = 57600;
	dcbCommPort.DCBlength = sizeof(DCB);
	dcbCommPort.BaudRate = baudRate;
	dcbCommPort.ByteSize = 8;
	dcbCommPort.Parity = NOPARITY;
	dcbCommPort.StopBits = ONESTOPBIT;
	dcbCommPort.fInX = FALSE;
	dcbCommPort.fOutX = FALSE;
	dcbCommPort.fOutxCtsFlow = FALSE;
	dcbCommPort.fOutxDsrFlow = FALSE;
	dcbCommPort.fDtrControl = DTR_CONTROL_ENABLE;
	dcbCommPort.fRtsControl = RTS_CONTROL_ENABLE;
	fSuccess = SetCommState(hCommPort, &dcbCommPort);
	if (!fSuccess) {
		AfxEndThread(0);
		return 0;
	}

	// modify COM port settings for polling
	CommTimeouts.ReadIntervalTimeout = 10;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 100;
	CommTimeouts.WriteTotalTimeoutMultiplier = 1;
	CommTimeouts.WriteTotalTimeoutConstant = 0;
	fSuccess = SetCommTimeouts(hCommPort, &CommTimeouts);

	// Activate stream
	DWORD nc;
	fSuccess = WriteFile(hCommPort, "UB10\r", 5, &nc, NULL);
	if (fSuccess == 0) {
		int nErr = GetLastError();
		char errMsg[250];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 250, NULL);
		TRACE1("Error sending to GPM: %s\r\n", errMsg);
	}


	SOCKET simSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (simSock == INVALID_SOCKET) {
		TRACE1("socket failed with error: %ld\n", WSAGetLastError());
		AfxEndThread(0);
		return 0;
	}
	int timeout = 100;
	iResult = setsockopt(simSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

	u_long iMode = 1;
	iResult = ioctlsocket(simSock, FIONBIO, &iMode);
	if (iResult == SOCKET_ERROR) {
		TRACE1("ioctl blocking call failed with error: %d\n", WSAGetLastError());
		closesocket(simSock);
		AfxEndThread(0);
		return 0;
	}

	char ip[20];
	dlg->GetDlgItemText(IDC_SIMULINKIP, ip, 20);
	dlg->GetDlgItemText(IDC_SIMULINKPORT, port, 50);
	
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY); // inet_addr(msg);
	sa.sin_port = htons(atoi(port));

	iResult = bind(simSock, (struct sockaddr*)&sa, sizeof(sa));
	if (iResult == SOCKET_ERROR) {
		TRACE1("bind failed with error: %d\n", WSAGetLastError());
		closesocket(simSock);
		AfxEndThread(0);
		return 0;
	}

	sa.sin_addr.s_addr = inet_addr(ip);
	int salen = 0;
	dlg->simrunning = true;
	dlg->SetDlgItemText(IDC_SVRCONNECT, "Disconnect");
	dlg->pSimulinkCOMList->EnableWindow(false);

	clock_t t0 = clock();

	while (dlg->simrunning)
	{
		
		iResult = recvfrom(simSock, recvbuf, recvbuflen, 0, (struct sockaddr *) &sa, &salen);
		if (iResult > 0) {
			if (iResult >= 512) iResult = 511;
			recvbuf[iResult] = 0;
			TRACE("Bytes received from client: %d\n", iResult);
			// Send to GPM

			fSuccess = WriteFile(hCommPort, recvbuf, iResult - 1, &nc, NULL);
			if (fSuccess == 0) {
				int nErr = GetLastError();
				char errMsg[250];
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 250, NULL);
				TRACE1("Error sending to GPM: %s\r\n", errMsg);
			}
		}
		else if (iResult == 0) {
			TRACE("Connection closing...\n");
			break;
		}
		else {
			int nErr = WSAGetLastError();
			if ((nErr != WSAEWOULDBLOCK) && (nErr != WSAETIMEDOUT))
			{
				printf("Recv from Client failed with error: %d\n", nErr);
				closesocket(simSock);
				break;
			}
		}

		iResult = ReadFile(hCommPort, svrbuf, 512, (LPDWORD)&nc, NULL);
		if (iResult == 0) {
			int nErr = GetLastError();
			char errMsg[500];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
			TRACE1("Error receiving from GPM: %s\r\n", errMsg);
			iResult = 1;
		}
		if (nc > 0)
		{
			clock_t t = clock();
			dlg->sim_dt = (int)(1000 * (t - t0) / CLOCKS_PER_SEC);
			t0 = t;
			dlg->sim_cnt++;
			dlg->sim_n = nc;


			iSendResult = sendto(simSock, svrbuf, nc, 0, (struct sockaddr*) &sa, sizeof(sa));
			if (iSendResult == SOCKET_ERROR) {
				TRACE1("Send to Client failed with error: %d\n", WSAGetLastError());
				closesocket(simSock);
				break;
			}
			TRACE1("Bytes sent to Client: %d\n", iSendResult);
		}
	}
	// No longer need server socket
	CancelIo(hCommPort);
	CloseHandle(hCommPort);
	closesocket(simSock);
	dlg->SetDlgItemText(IDC_SIMULINKCONNECT, "Connect");
	dlg->pSimulinkCOMList->EnableWindow(true);
	dlg->simrunning = false;
	AfxEndThread(0);
	return 0;
}
