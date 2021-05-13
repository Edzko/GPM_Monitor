// GPM_UpdateLite.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma comment(lib,"Ws2_32.lib")
#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#define MAX_FWSIZE (1500)
#define BLK_FWSIZE (0x200)
#define ERASE_BLOCK_SIZE (16384UL)  // 0x4000

int main(int argc, char* argv[])
{
	char ip[50] = "";
	USHORT port = 2000;
	WSADATA wsaData = { 0 };
	int iResult = 0;
	sockaddr_in sa;
	wchar_t errMsg[500];
	char fwfilename[1000];
	unsigned char fwdata[MAX_FWSIZE];
	unsigned char fwfile[500000];

    if (argc == 1) {
        printf("Syntax: GPM Update Lite [<ip_address>] <firmware.bin>\r\n");
        return 0;
    }

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		wprintf(L"WSAStartup failed: %d\n", iResult);
		return 1;
	}

	if (argc > 2) {
		strcpy_s(ip, 50, argv[1]);
		strcpy_s(fwfilename, 1000, argv[2]);
	} else {      // if only two arguments, then no IP was provided, and we discover
		sockaddr_in si_me;
		sockaddr_in si_other;
		SOCKET s;
		s = socket(AF_INET, SOCK_DGRAM, 0);   // UDP
		int port = 55555;
		char broadcast = 1;
		bool found = false;

		memset(&si_me, 0, sizeof(si_me));
		si_me.sin_family = AF_INET;
		si_me.sin_port = htons(port);
		si_me.sin_addr.s_addr = INADDR_ANY;

		int timeout = 500;
		setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

		bind(s, (sockaddr*)&si_me, sizeof(sockaddr));

		printf("Discovering ...");

		// discover for up to 10 seconds
		for (int iD=0;iD<100;iD++)
		{
			char buf[10000];
			int slen = sizeof(sockaddr);
			if (recvfrom(s, buf, sizeof(buf) - 1, 0, (sockaddr*)&si_other, &slen) > 0)
			{
				sprintf_s(ip, 50, "%i.%i.%i.%i", si_other.sin_addr.S_un.S_un_b.s_b1,
					si_other.sin_addr.S_un.S_un_b.s_b2,
					si_other.sin_addr.S_un.S_un_b.s_b3,
					si_other.sin_addr.S_un.S_un_b.s_b4);
				

				found = true;
				break;
			}
			Sleep(100);
		}
		if (!found) {
			printf(" No GPM module found.\r\n");
			WSACleanup();
			return 1;
		}

		printf(" done. Found %s\r\n",ip);
		strcpy_s(fwfilename, 1000, argv[1]);
    }

	// now connect
	SOCKET gpmSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(ip);
	sa.sin_port = htons(port);

	int nResult = connect(gpmSock, (const sockaddr*)&sa, sizeof(sa));
	if (nResult == SOCKET_ERROR)
	{
		int nErr = WSAGetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
		printf("Error: %ws\r\n", errMsg);
	}
	int timeout = 100;
	nResult = setsockopt(gpmSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
	if (nResult == SOCKET_ERROR)
	{
		int nErr = WSAGetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
		printf("Error: %ws\r\n", errMsg);
	}
	nResult = recv(gpmSock, ip, 100, 0);  // empty any possible residual buffer

	nResult = send(gpmSock, "?v\r", 3, 0);
	if (nResult == SOCKET_ERROR)
	{
		int nErr = WSAGetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
		printf("Error: %ws\r\n", errMsg);
	}
	Sleep(100);
	char msg[500];
	int nc = 500;
	nc = recv(gpmSock, msg, nc, 0);
	if (nc == SOCKET_ERROR)
	{
		int nErr = GetLastError();
		if (nErr != WSAETIMEDOUT) {
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
			printf("Error: %ws\r\n", errMsg);
		}
	}
	
	if (nc > 0)
	{
		int start = 0, end = 0;
		while ((msg[start] <= 13) && (start < nc - 1)) start++;
		end = start;
		while ((msg[end] > 13) && (end < nc - 1)) end++;
		msg[end] = 0;
		printf("GPM version: %s\r\n",&msg[start]);
	}
	else {
		shutdown(gpmSock, SD_SEND);
		Sleep(100);
		closesocket(gpmSock);

		printf("Could not connect.\r\n");
		WSACleanup();
		return 1;
	}

	// Now start updating firmware
	char key = 27;
	
	FILE* fw;
	errno_t nErr = fopen_s(&fw, fwfilename, "rb");
	if (nErr != 0) {
		printf("Could not open file.\r\n");
		WSACleanup();
		return 1;
	}
	int nFW = (int)fread(fwfile, 1, sizeof(fwfile), fw);
	nFW = (nFW / ERASE_BLOCK_SIZE + 1) * ERASE_BLOCK_SIZE;
	if (fw) fclose(fw);
	unsigned short crc = 0;
	for (int i = 0; i < nFW; i++) crc += fwfile[i];
	fw = NULL;
	int iFW = 0;
	int wFW = 0;
	
	// Enter "upload firmware" mode of the App
	int n = 0;
	for (n = 0; n < 3; n++) {
		nResult = send(gpmSock, "rs3\r", 4, 0);
		if (nResult == SOCKET_ERROR)
		{
			int nErr = WSAGetLastError();

			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
			printf("Error: %ws\r\n", errMsg);
		}
		Sleep(100);
		nc = 100;
		while (nc == 100) {
			nc = recv(gpmSock, msg, nc, 0);
			if (nc == SOCKET_ERROR)
			{
				int nErr = GetLastError();
				if (nErr != WSAETIMEDOUT) {
					FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
					printf("Error: %ws\r\n", errMsg);
				}
			}
		}
		if (msg[nc - 1] == 'K') break;
	}
	if (msg[nc - 1] != 'K') {   //acknowledge firmware upload
		printf("Could not enter firmware upload mode\r\n");
		WSACleanup();
		return 0;
	}

	while (true) {
		char rtn[100];
		int nc = 100;
		memset(rtn, 0, 100);
		Sleep(10);
		nc = recv(gpmSock, rtn, nc, 0);
		if (nc == SOCKET_ERROR)
		{
			int nErr = GetLastError();
			if (nErr != WSAETIMEDOUT) {
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
				printf("Error: %ws\r\n", errMsg);
			}
		}
		if ((nc >= 1) || (iFW == 0))
		{
			wFW = 0;
			printf("Upgrading: pkt=%i/%i\r", iFW / BLK_FWSIZE, nFW / BLK_FWSIZE);
			if ((rtn[0] == 'K') || (iFW == 0)) { // okay. send next
				if (iFW >= nFW) {  // finished
					fwdata[0] = 0x55;
					fwdata[1] = 0xAA;
					fwdata[2] = 0xFF;
					fwdata[3] = 0xFF;
					fwdata[4] = crc >> 8;
					fwdata[5] = crc & 0xFF;
					nResult = send(gpmSock, (char*)fwdata, BLK_FWSIZE + 0x10, 0);
					if (nResult == SOCKET_ERROR)
					{
						int nErr = WSAGetLastError();
						FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
						printf("Error: %ws\r\n", errMsg);
					}
					break; // done
				}
				else {
					memcpy(fwdata + 0x10, &fwfile[iFW], BLK_FWSIZE);
					fwdata[0] = 0x55;
					fwdata[1] = 0xAA;
					fwdata[2] = (iFW / BLK_FWSIZE) >> 8;
					fwdata[3] = (iFW / BLK_FWSIZE) & 0xFF;
					nResult = send(gpmSock, (char*)fwdata, BLK_FWSIZE + 0x10, 0);
					if (nResult == SOCKET_ERROR)
					{
						int nErr = WSAGetLastError();
						FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
						printf("Error: %ws\r\n", errMsg);
					}
					iFW += BLK_FWSIZE;
				}
			};
			if (rtn[0] == 'R') { // repeat previous packet
				iFW -= 2 * BLK_FWSIZE;
				if (iFW < 0) iFW = 0;
				memcpy(fwdata + 0x10, &fwfile[iFW], BLK_FWSIZE);
				//for (int i = 0; i < BLK_FWSIZE; i++) crc += fwdata[i];
				fwdata[0] = 0x55;
				fwdata[1] = 0xAA;
				fwdata[2] = (iFW / BLK_FWSIZE) >> 8;
				fwdata[3] = (iFW / BLK_FWSIZE) & 0xFF;
				nResult = send(gpmSock, (char*)fwdata, BLK_FWSIZE + 0x10, 0);
				if (nResult == SOCKET_ERROR)
				{
					int nErr = WSAGetLastError();
					FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
					printf("Error: %ws\r\n", errMsg);
				}
				iFW += BLK_FWSIZE;
			};
			if (rtn[0] == 'Q') { // exit
				break;
			};
		}
		else {
			wFW++;
			if (wFW > 10)
			{
				nResult = send(gpmSock, (char*)fwdata, BLK_FWSIZE + 0x10, 0);
				if (nResult == SOCKET_ERROR)
				{
					int nErr = WSAGetLastError();
					FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
					printf("Error: %ws\r\n", errMsg);
				}
				wFW = 0;
			}
		}
	}

	shutdown(gpmSock, SD_SEND);
	Sleep(100);
	closesocket(gpmSock);

	printf("Firmware update complete\rn");
	WSACleanup();

	return 0;
}


