#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <Setupapi.h>
#include "mex.h"

#define MAXDATA 20000

int Connected = 0;
char CommPort[50];
HANDLE hCommPort = INVALID_HANDLE_VALUE;
DCB dcbCommPort;
COMMTIMEOUTS CommTimeouts;
int i, port;
char *portname;
SOCKET gpmSock;
WSAData wsaData;
unsigned char accdata[MAXDATA];

#pragma pack(2)
typedef union {
    unsigned char data[22];
    struct {
        short accel[3];
        short temp;
        short gyro[3];
        short compass[3];
        short compass_status;
    } sensor;
} IMU_T;
IMU_T imu;

typedef struct {
    unsigned int h;
    unsigned long long t;
    unsigned int vpp;
    unsigned short adc[4];
    IMU_T imu;
	unsigned short unused[9]; // 64
} ACC_T;

void GetComPorts(char *txt)
{
	int iLB = 0, idx = 0;
	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
	SP_DEVICE_INTERFACE_DATA interfaceData;
	ZeroMemory(&interfaceData, sizeof(interfaceData));
	interfaceData.cbSize = sizeof(interfaceData);

	strcpy_s(txt,1000,"");
	for (int nDevice = 0; nDevice < 2000; nDevice++)
	{
		SP_DEVINFO_DATA devInfoData;
		ZeroMemory(&devInfoData, sizeof(devInfoData));
		devInfoData.cbSize = sizeof(devInfoData);

		BOOL nErr = SetupDiEnumDeviceInfo(hDevInfo,
			nDevice,
			&devInfoData);
		if (nErr == FALSE)
		{
			DWORD nLastErr = GetLastError();
			if (nLastErr == ERROR_NO_MORE_ITEMS)
				break;
		}
		char friendlyName[300];
		char manufName[300];
		if (SetupDiGetDeviceRegistryProperty(hDevInfo,
			&devInfoData,
			SPDRP_FRIENDLYNAME,
			NULL,
			(BYTE *)friendlyName,
			sizeof(friendlyName),
			NULL))
		{
			//mexPrintf("Friendlyname: %s\r\n", friendlyName);
			char *pCom = strstr(friendlyName, "(COM");
			if (pCom)
			{
				if (SetupDiGetDeviceRegistryProperty(hDevInfo,
					&devInfoData,
					SPDRP_MFG,
					NULL,
					(BYTE *)manufName,
					sizeof(manufName),
					NULL))
				{
					//mexPrintf("Manufacturer: %s,, Friendlyname: %s \r\n", manufName, friendlyName);
				}
				if (strlen(txt)>0)
					strcat_s(txt,1000,"|");
				strcat_s(txt,1000,friendlyName);
			}
		}
	}
	
}

BOOL ConnectIP(char *address)
{
	sockaddr_in sa;

	gpmSock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	USHORT port = 2000;

	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(address);
	sa.sin_port = htons(port);
	
	int nResult = connect(gpmSock,(const sockaddr*)&sa,sizeof(sa));
	if (nResult==SOCKET_ERROR)
	{
		int nErr=WSAGetLastError();
		char errMsg[500];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,nErr,0,errMsg,500,NULL);
		mexPrintf("Error: %s\r\n",errMsg);
		return FALSE;
	}
	Connected = 2;
#if 1
	int timeout = 100;
	nResult = setsockopt(gpmSock,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));
	if (nResult==SOCKET_ERROR)
	{
		int nErr=WSAGetLastError();
		char errMsg[500];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,nErr,0,errMsg,500,NULL);
		mexPrintf("Error: %s\r\n",errMsg);
		return FALSE;
	}
#endif
	//int flag = 1; 
	//nResult = setsockopt(gpmSock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
	
	return TRUE;
}

BOOL InitCommPort(int CommPortNum, int baud,
	BYTE ByteSize, BYTE Parity, BYTE StopBits)
{	
	BOOL fSuccess;
	sprintf_s(CommPort, 50, "\\\\.\\COM%i", CommPortNum);
	//sprintf_s(CommPort, 50, "COM%i", CommPortNum);
	hCommPort = CreateFile(CommPort, GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hCommPort == INVALID_HANDLE_VALUE) {
		DWORD dwError = GetLastError();
		// error 2: File Not Found
		mexPrintf("Can't open serial port (%s).  error = %d", CommPort,dwError);
		return FALSE;
	}
	
	fSuccess = GetCommTimeouts(hCommPort, &CommTimeouts);
	if (!fSuccess) {
		int nErr = GetLastError();
		mexPrintf("Couldn't get comm timeouts (%i)\r\n",nErr);
		//return fSuccess;
	}
	/*
	ReadIntervalTimeout 
		The maximum time allowed to elapse between the arrival of two bytes on the 
		communications line, in milliseconds. During a ReadFile operation, the time 
		period begins when the first byte is received. If the interval between the 
		arrival of any two bytes exceeds this amount, the ReadFile operation is 
		completed and any buffered data is returned. A value of zero indicates that 
		interval time-outs are not used. 

		A value of MAXDWORD, combined with zero values for both the ReadTotalTimeoutConstant 
		and ReadTotalTimeoutMultiplier members, specifies that the read operation is 
		to return immediately with the bytes that have already been received, even if
		no bytes have been received.

	ReadTotalTimeoutMultiplier 
		The multiplier used to calculate the total time-out period for read operations, 
		in milliseconds. For each read operation, this value is multiplied by the 
		requested number of bytes to be read.

	ReadTotalTimeoutConstant 
		A constant used to calculate the total time-out period for read operations, 
		in milliseconds. For each read operation, this value is added to the product of 
		the ReadTotalTimeoutMultiplier member and the requested number of bytes. 

		A value of zero for both the ReadTotalTimeoutMultiplier and ReadTotalTimeoutConstant 
		members indicates that total time-outs are not used for read operations.

	WriteTotalTimeoutMultiplier 
		The multiplier used to calculate the total time-out period for write operations, 
		in milliseconds. For each write operation, this value is multiplied by the number 
		of bytes to be written.

	WriteTotalTimeoutConstant 
		A constant used to calculate the total time-out period for write operations, 
		in milliseconds. For each write operation, this value is added to the product 
		of the WriteTotalTimeoutMultiplier member and the number of bytes to be written. 

		A value of zero for both the WriteTotalTimeoutMultiplier and WriteTotalTimeoutConstant 
		members indicates that total time-outs are not used for write operations.

	Remarks
		If an application sets ReadIntervalTimeout and ReadTotalTimeoutMultiplier to MAXDWORD and sets ReadTotalTimeoutConstant to a value greater than zero and less than MAXDWORD, one of the following occurs when the ReadFile function is called:


	If there are any bytes in the input buffer, ReadFile returns immediately with the 
	bytes in the buffer. 
	If there are no bytes in the input buffer, ReadFile waits until a byte arrives 
	and then returns immediately. 
	If no bytes arrive within the time specified by ReadTotalTimeoutConstant, 
	ReadFile times out. 
	*/
	CommTimeouts.ReadIntervalTimeout = 1;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 50;
	CommTimeouts.WriteTotalTimeoutMultiplier = 1;
	CommTimeouts.WriteTotalTimeoutConstant = 1;
	fSuccess = SetCommTimeouts(hCommPort, &CommTimeouts);
	if (!fSuccess) {
		int nErr = GetLastError();
		mexPrintf("Couldn't set comm timeouts (%i)\r\n",nErr);
		//return FALSE;
	}
	
	fSuccess = GetCommState(hCommPort, &dcbCommPort);
	if (!fSuccess) {
		int nErr = GetLastError();
		mexPrintf("Couldn't get comm state (%i)\r\n",nErr);
		//return fSuccess;
	}
	
	dcbCommPort.DCBlength = sizeof(DCB);
	dcbCommPort.BaudRate = baud;
	dcbCommPort.ByteSize = ByteSize;
	dcbCommPort.Parity = Parity;
	dcbCommPort.StopBits = StopBits;
	dcbCommPort.fInX = FALSE;
	dcbCommPort.fOutX = FALSE;
	dcbCommPort.fOutxCtsFlow = FALSE;
	dcbCommPort.fOutxDsrFlow = FALSE;
	dcbCommPort.XonChar = dcbCommPort.XoffChar + 1;
	dcbCommPort.fDtrControl = DTR_CONTROL_ENABLE;
	dcbCommPort.fRtsControl = DTR_CONTROL_ENABLE;
	fSuccess = SetCommState(hCommPort, &dcbCommPort);
	if (!fSuccess) {
		int nErr = GetLastError(); 
		// 87 = ERROR_INVALID_PARAMETER
		mexPrintf("Couldn't set comm state (%i)\r\n",nErr);
		//return FALSE;
	}
	
	
	//mexPrintf("Com port configured!\r\n");
	return TRUE;
}

void cleanup(void)
{
	if ((Connected == 1) && (hCommPort != INVALID_HANDLE_VALUE))
	{	
		CloseHandle(hCommPort);
		hCommPort = INVALID_HANDLE_VALUE;
		Connected = 0;
		mexPrintf("Disconnected.\r\n");
	}
	if (Connected == 2)
	{
		shutdown(gpmSock,1);
		Sleep(100);
		closesocket(gpmSock);
		WSACleanup();
		Connected = 0;
		mexPrintf("Disconnected.\r\n");
	}
}

void mexFunction(int nlhs,mxArray *plhs[],int nrhs,const mxArray *prhs[])
{
	DWORD nc;
	double *out, *in;
	char fcn[100], cmd[100];
	
	char text[1000];
	int baud = 115200;
	ACC_T *msg = (ACC_T *)accdata; 
	

	if (nrhs==0)
	{
		if (Connected == 1)
		{
			if (hCommPort != INVALID_HANDLE_VALUE)
			{	
				CloseHandle(hCommPort);
				hCommPort = INVALID_HANDLE_VALUE;
			}
			Connected = 0;
			mexPrintf("Disconnected.\r\n");
		}
		if (Connected == 2)
		{
			shutdown(gpmSock,1);
			Sleep(100);
			closesocket(gpmSock);
			WSACleanup();
			Connected = 0;
			mexPrintf("Disconnected.\r\n");
		}
		return;
	}
	
	
	// only process COM port listing and initialization when port is closed.
	if (Connected == 0)
	{
		if (mxIsChar(prhs[0]))
		{
			mxGetString(prhs[0],fcn,100);
			portname = strstr(fcn,"COM");
			if (portname)
			{
				port = atoi(portname+3);
				if (port)
				{
					if (nrhs>1)
					{
						in = mxGetPr(prhs[1]);
						baud = (int)in[0];
					}
					if (InitCommPort(port, baud, 8, 0, 1)) {
						FlushFileBuffers(hCommPort);
						Connected = 1;
						mexPrintf("Connected to %s.\r\n",portname);
					}
					else
					{
						if (hCommPort != INVALID_HANDLE_VALUE)
						{	
							CloseHandle(hCommPort);
						}
						hCommPort = INVALID_HANDLE_VALUE;
					}
				}
			} else {
				portname = strstr(fcn,"IP:");
				WORD DLLVersion;
				DLLVersion = MAKEWORD(2,1);
				long answer = WSAStartup(DLLVersion,&wsaData);
				if (portname)
				{
					if (ConnectIP(portname+3)) {
						mexPrintf("Connected to %s.\r\n",portname+3);
						Connected = 2;
					}
				}
			}
			if (strcmp(fcn,"LIST")==0)
			{
				GetComPorts(text);
				if (strlen(text)>0)
					strcat_s(text,1000,"|");
				strcat_s(text,1000,"IP:192.168.1.1");
				plhs[0] = mxCreateString(text);
			}

			mexAtExit(cleanup);

			return;
		}
	}
		
	else
	{
		if (mxIsChar(prhs[0]))
		{
			mxGetString(prhs[0],fcn,100);
			if (strcmp(fcn,"SETBATCH")==0) {
				if (nrhs>=2) {
					int n = (int)mxGetScalar(prhs[1]);
					sprintf_s(cmd,100,"sp5,%i\r\r",n);
					if (Connected==1) {
						WriteFile(hCommPort, cmd, (DWORD)strlen(cmd), &nc, NULL);
						ReadFile(hCommPort, accdata, MAXDATA, &nc, NULL);
					}
					if (Connected==2) {
						send(gpmSock,cmd, (DWORD)strlen(cmd),NULL);
						recv(gpmSock,(char*)accdata,MAXDATA, NULL);
						//mexPrintf("Set batch %i\r\n",n);
					}
					return;
				}
			}
			if (strcmp(fcn,"GETBATCH")==0) {
				if (Connected==1) {
					WriteFile(hCommPort, "vi3\r", 4, &nc, NULL);
					if (ReadFile(hCommPort, accdata, MAXDATA, &nc, NULL)== FALSE)
						nc = 0;
				}
				int resp;
				if (Connected==2) {
					resp = send(gpmSock,"vi3\r", 5,NULL);
					Sleep(50);
					resp = recv(gpmSock,(char*)accdata,MAXDATA, NULL);
					if (resp == SOCKET_ERROR)
					{
						int nErr = WSAGetLastError();// 
						if (nErr == WSAETIMEDOUT)
						{
							resp = send(gpmSock,"vi3\r", 5,NULL);
							Sleep(50);
							resp = recv(gpmSock,(char*)accdata,MAXDATA, NULL);
							if (resp == SOCKET_ERROR)
							{
								nErr = WSAGetLastError();// 
								mexPrintf("Socket Error %i\r\n",nErr);
							}
						}
					} 
				}
				if (resp>0) {
					int n = resp / sizeof(ACC_T);
					plhs[0]=mxCreateDoubleMatrix(n,12,mxREAL);
					out = mxGetPr(plhs[0]);
					int len = sizeof(ACC_T);
					for (int i=0;i<n;i++)
					{
						double AScale = 2.0/0x8000;
						double GScale = 250.0/0x8000*3.14159265359/180.0;
						ACC_T *msg = (ACC_T *)(&accdata[len*i]); 
						out[i+0*n] = (double)0.001*msg->t;
						out[i+1*n] = (double)msg->vpp;
						out[i+2*n] = (double)AScale * msg->imu.sensor.accel[0];
						out[i+3*n] = (double)AScale * msg->imu.sensor.accel[1];
						out[i+4*n] = (double)AScale * msg->imu.sensor.accel[2];
						out[i+5*n] = (double)GScale * msg->imu.sensor.gyro[0];
						out[i+6*n] = (double)GScale * msg->imu.sensor.gyro[1];
						out[i+7*n] = (double)GScale * msg->imu.sensor.gyro[2];
						out[i+8*n] = (double)msg->adc[0];
						out[i+9*n] = (double)msg->adc[1];
						out[i+10*n] = (double)msg->adc[2];
						out[i+11*n] = (double)msg->adc[3];
					}
				}
				return;
			}
			
		}
		if (Connected == 1) {
			WriteFile(hCommPort, "vi0\r", 4, &nc, NULL);
			if (ReadFile(hCommPort, accdata, MAXDATA, &nc, NULL)== FALSE)
				nc = 0;
		}
		if (Connected == 2) {
			send(gpmSock,"vi0\r",5,NULL);
			//Sleep(1);
			nc = recv(gpmSock,(char*)accdata,sizeof(ACC_T), NULL);
		}
		if (nc>0) {
			if (msg->h == 0xAA55AA55)
			{
				double AScale = 2.0/0x8000;
				double GScale = 250.0/0x8000*3.14159265359/180.0;
				plhs[0]=mxCreateDoubleMatrix(1,12,mxREAL);
				out = mxGetPr(plhs[0]);
				out[0] = (double)0.001*msg->t;
				out[1] = (double)msg->vpp;
				out[2] = (double)AScale * msg->imu.sensor.accel[0];
				out[3] = (double)AScale * msg->imu.sensor.accel[1];
				out[4] = (double)AScale * msg->imu.sensor.accel[2];
				out[5] = (double)GScale * msg->imu.sensor.gyro[0];
				out[6] = (double)GScale * msg->imu.sensor.gyro[1];
				out[7] = (double)GScale * msg->imu.sensor.gyro[2];
				out[8] = (double)msg->adc[0];
				out[9] = (double)msg->adc[1];
				out[10] = (double)msg->adc[2];
				out[11] = (double)msg->adc[3];
				return;
			}
		}
		plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
		out = mxGetPr(plhs[0]);
		out[0] = -1;
	}
}