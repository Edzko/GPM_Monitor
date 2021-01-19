// GPM_MonitorDlg.cpp : implementation file
//

#include "stdafx.h"
#include <Setupapi.h>
#include <Devguid.h>
#include "GPM_Monitor.h"
#include "GPM_MonitorDlg.h"

#include "build.h"
#include "mydate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


char fwdata[MAX_FWSIZE];
char usbdata[MAX_FWSIZE];

UINT __cdecl ProcDiscoverThreadFunction(LPVOID pParam);
UINT __cdecl ProcServerThreadFunction(LPVOID pParam);

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	char txt[100];
	sprintf_s(txt,100,"QCM Monitor Version 4.1.%i",__MY_BUILD);
	SetDlgItemText(IDC_BUILD,txt);

	sprintf_s(txt,100,"Built on %s",__MY_DATE);
	SetDlgItemText(IDC_MYDATE,txt);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CGPM_MonitorDlg dialog

CGPM_MonitorDlg::CGPM_MonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGPM_MonitorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGPM_MonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGPM_MonitorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CONNECT, &CGPM_MonitorDlg::OnBnClickedConnect)
	ON_WM_TIMER()
	
	ON_BN_CLICKED(IDC_RESET, &CGPM_MonitorDlg::OnBnClickedReset)
	ON_WM_DESTROY()
	ON_WM_MOUSEWHEEL()
	
	ON_BN_CLICKED(IDC_UPDATE, &CGPM_MonitorDlg::OnBnClickedUpdate)
	ON_BN_CLICKED(IDC_AUTOCONNECT, &CGPM_MonitorDlg::OnBnClickedAutoconnect)
	ON_BN_CLICKED(IDC_SERVER, &CGPM_MonitorDlg::OnBnClickedServer)
	ON_BN_CLICKED(IDC_SVRCONNECT, &CGPM_MonitorDlg::OnBnClickedSvrconnect)
	ON_CBN_SELCHANGE(IDC_SVRCOMLIST, &CGPM_MonitorDlg::OnCbnSelchangeSvrcomlist)
END_MESSAGE_MAP()


// CGPM_MonitorDlg message handlers

void CGPM_MonitorDlg::GetCOMPorts(CComboBox *pCOM)
{
	int iLB = 0;
	pCOM->ResetContent();
	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL,NULL,NULL,DIGCF_PRESENT | DIGCF_ALLCLASSES);
	SP_DEVICE_INTERFACE_DATA interfaceData;
	ZeroMemory(&interfaceData, sizeof(interfaceData));
	interfaceData.cbSize = sizeof(interfaceData);

	for (int nDevice=0;nDevice<2000;nDevice++)
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
		//char devpath[300];
		if (SetupDiGetDeviceRegistryProperty(hDevInfo, 
						&devInfoData, 
						SPDRP_FRIENDLYNAME, 
						NULL, 
						(BYTE *)friendlyName, 
						sizeof(friendlyName), 
						NULL))
		{
			char *pCom = strstr(friendlyName,"(COM");
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
					TRACE2("Friendlyname: %s, Manufacturer: %s\r\n",friendlyName,manufName);

					//SetupDiGetDeviceRegistryProperty(hDevInfo, 
					//			&devInfoData, 
					//			SPDRP_SERVICE, 
					//			NULL, 
					//			(BYTE *)devpath, 
					//			sizeof(devpath), 
					//			NULL);
					//TRACE1("Address: %s\r\n",devpath);

					int idx = pCOM->AddString(friendlyName);
					int iCom = pCom[4]-'0';
					if (pCom[5]!=')') iCom = 10 * iCom + pCom[5] - '0';
					pCOM->SetItemData(idx,iCom);

					//if (iLB<20) mCOM[iLB] = manufName;
					iLB++;
				}
			}
		}
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
}

BOOL CGPM_MonitorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	

	CComboBox *pBaud = (CComboBox*)GetDlgItem(IDC_BAUD);
	pBaud->ResetContent();
	pBaud->AddString("115200");
	pBaud->AddString("57600");
	pBaud->AddString("38400");
	pBaud->AddString("19200");
	SetDlgItemText(IDC_BAUD,"57600");

	pCOMList = (CComboBox*)GetDlgItem(IDC_COMLIST);
	GetCOMPorts(pCOMList);
	pSvrCOMList = (CComboBox*)GetDlgItem(IDC_SVRCOMLIST);
	GetCOMPorts(pSvrCOMList);
	CEdit *pPort = (CEdit*)GetDlgItem(IDC_SVRPORT);
	SetDlgItemText(IDC_SVRPORT, "2000");
	pPort->EnableWindow(0);

	int iCOM = AfxGetApp()->GetProfileInt("","COM",0);
	if ((iCOM>=0) && (pCOMList->GetCount()>=iCOM)) 
		pCOMList->SetCurSel(iCOM); 
	else pCOMList->SetCurSel(0);
	Connected = 0;
	hCommPort = 0;
	upgrading = false;
	ambootloader = false;
	fw = NULL;
	CButton *pFW = (CButton*)GetDlgItem(IDC_UPDATE);
	pFW->EnableWindow(false);

	theApp.m_pDiscoverThread = AfxBeginThread(&ProcDiscoverThreadFunction, this);

	
	logFile = NULL;

	CString portname = AfxGetApp()->GetProfileString("","Port","");
	pCOMList->SelectString(0,portname);
	/*
	for (int i=0;i<pCOMList->GetCount();i++)
	{
		CString lbportname;
		pCOMList->GetLBText(i,&lbportname);
		if (portname==lbportname)
		{
			pCOMList->SelectString(0,portname);
			break;
		}
	}
	*/

	pAutoConnect = (CButton*)GetDlgItem(IDC_AUTOCONNECT);
	int autoConnect = AfxGetApp()->GetProfileInt("", "autoConnect", 0);
	if (autoConnect) {
		pAutoConnect->SetCheck(BST_CHECKED);
		OnBnClickedConnect();
	}

	pSvrCOMList = (CComboBox*)GetDlgItem(IDC_SVRCOMLIST);
	portname = AfxGetApp()->GetProfileString("", "server", "");
	pSvrCOMList->SelectString(0, portname);
	int icom = pSvrCOMList->GetCurSel();
	svrcom = (int)pSvrCOMList->GetItemData(icom);

	pServer = (CButton*)GetDlgItem(IDC_SERVER);
	int tcpServer = AfxGetApp()->GetProfileInt("", "tcpServer", 0);
	if (tcpServer) pServer->SetCheck(BST_CHECKED);
	svrrunning = false;
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CGPM_MonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGPM_MonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if (Connected>0)
		{
		//CPaintDC dc(this); // device context for painting

		//drawChart(&dc);
		}
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGPM_MonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGPM_MonitorDlg::Send(char *msg, int len)
{
	int fSuccess;
	if ((Connected==2) && (gpmSock))
	{
		int nResult = send(gpmSock,msg,len,0);
		if (nResult==SOCKET_ERROR)
		{
			int nErr=WSAGetLastError();
			char errMsg[500];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,nErr,0,errMsg,500,NULL);
			TRACE1("Error: %s\r\n",errMsg);
		}
	}
	if ((Connected==1) && (hCommPort))
	{
		DWORD nc;
		fSuccess = WriteFile(hCommPort,msg,len,&nc,NULL);
		if (fSuccess == 0) { 
			int nErr = GetLastError();
			char errMsg[250];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
			TRACE1("Error: %s\r\n", errMsg);
		}
	}
}

void CGPM_MonitorDlg::Recv(char *msg, int *len)
{
	if ((Connected==2) && (gpmSock))
	{
		
		int nResult = recv(gpmSock,msg,*len,0);
		if (nResult==SOCKET_ERROR)
		{
			int nErr = GetLastError();
			char errMsg[500];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,nErr,0,errMsg,500,NULL);
			TRACE1("Error: %s\r\n",errMsg);
		}
		*len = nResult;
	} 
	else

	if ((Connected==1) && (hCommPort))
	{
		DWORD nc;
		int nResult = ReadFile(hCommPort,msg,*len,(LPDWORD)&nc,NULL);
		if (nResult>=0) 
			*len = nc;
	}
	else *len = 0;
}

void CGPM_MonitorDlg::OnBnClickedConnect()
{
	char CommPort[100], msg[100];
	int fSuccess, nResult;
	DCB dcbCommPort;
	CComboBox* pCom = (CComboBox*)GetDlgItem(IDC_COMLIST);
	int com = (int)pCom->GetItemData(pCom->GetCurSel());
	sockaddr_in sa;
	CButton *pFW = (CButton*)GetDlgItem(IDC_UPDATE);

	KillTimer(4);

	if (com==100)
	{
		if (Connected==2)
		{
			shutdown(gpmSock,SD_SEND);
			Sleep(100);
			closesocket(gpmSock);
			Connected = 0;
			pCom->EnableWindow(true);
			SetDlgItemText(IDC_CONNECT,"Connect");
			SetDlgItemText(IDC_VERSION, "");
			pFW->EnableWindow(false);
			if (fw) fclose(fw);
			fw = NULL;
		} else {
			gpmSock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			USHORT port = 2000;
			pCom->GetLBText(pCom->GetCurSel(),CommPort);
			strcpy_s(msg,100,&CommPort[6]);
			for (int i=0;i<100;i++) if (msg[i]==')') msg[i]=0;
			sa.sin_family = AF_INET;
			sa.sin_addr.s_addr = inet_addr(msg);
			sa.sin_port = htons(port);
			
			nResult = connect(gpmSock,(const sockaddr*)&sa,sizeof(sa));
			if (nResult==SOCKET_ERROR)
			{
				int nErr=WSAGetLastError();
				char errMsg[500];
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,nErr,0,errMsg,500,NULL);
				TRACE1("Error: %s\r\n",errMsg);
			}
			Connected = 2;

			int timeout = 100;
			nResult = setsockopt(gpmSock,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));
			if (nResult==SOCKET_ERROR)
			{
				int nErr=WSAGetLastError();
				char errMsg[500];
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,nErr,0,errMsg,500,NULL);
				TRACE1("Error: %s\r\n",errMsg);
			}

			nResult = recv(gpmSock,msg,100,0);

			char msg[500];
			Send("?v\r",3);
			Sleep(100);
			int nc = 500;
			Recv(msg,&nc);
			TRACE1("Error: %s\r\n",msg);
			if (nc>0)
			{
				int start = 0, end = 0;
				while (msg[start] <= 13) start++;
				end = start;
				while (msg[end] > 13) end++;
				msg[end] = 0;
				SetDlgItemText(IDC_VERSION, &msg[start]);
			}
			SetTimer(1,1000,NULL);  // Joystick timer, half rate for AC4790
			SetDlgItemText(IDC_CONNECT,"Disconnect");
			pCom->EnableWindow(false);
			pFW->EnableWindow(true);
		}
	}
	else
	{
		if (Connected!=0)
		{
			Connected = 0;
			if (hCommPort)
			{
				CancelIo(hCommPort);
				CloseHandle(hCommPort);
			}
			if (logFile)
			{
				fclose(logFile);
				logFile = NULL;
			}
			SetDlgItemText(IDC_CONNECT,"Connect");
			SetDlgItemText(IDC_VERSION, "");
			SetDlgItemText(IDC_UPDATE, "Update");
			KillTimer(1);
			KillTimer(2);
			KillTimer(4);
			pCom->EnableWindow(true);
			pFW->EnableWindow(false);
			if (fw) fclose(fw);
			fw = NULL;
		} else {
			sprintf_s(CommPort,20,"\\\\.\\COM%i",com);

			hCommPort = CreateFile(CommPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,	FILE_ATTRIBUTE_NORMAL, NULL);
			if (hCommPort == INVALID_HANDLE_VALUE) {
				DWORD dwError = GetLastError();
				char errMsg[500];
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,dwError,0,errMsg,500,NULL);
				TRACE1("Error: %s\r\n",errMsg);
				strcpy_s((char*)msg,100,"Error: ");
				strcat_s((char*)msg, 100, errMsg);
				SetDlgItemText(IDC_VERSION, msg);
				return;
			}
			fSuccess = GetCommState(hCommPort, &dcbCommPort);
			if (!fSuccess) return;
			int baudRate = GetDlgItemInt(IDC_BAUD);
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
			if (!fSuccess) return;

			// modify COM port settings for polling
			CommTimeouts.ReadIntervalTimeout = 10;
			CommTimeouts.ReadTotalTimeoutMultiplier = 0;
			CommTimeouts.ReadTotalTimeoutConstant = 100;
			CommTimeouts.WriteTotalTimeoutMultiplier = 1;
			CommTimeouts.WriteTotalTimeoutConstant = 0;
			fSuccess = SetCommTimeouts(hCommPort, &CommTimeouts);
			
			Connected = 1;
	//		pCom->GetLBText(pCom->GetCurSel(),portName);
			SetDlgItemText(IDC_CONNECT,"Disconnect");
			pCom->EnableWindow(false);

			char msg[500];
			int nc=500;
			Send("?v\r",3);
			Sleep(100);
			Recv(msg,&nc);
			if (nc > 0)
			{
				int start = 0, end = 0;
				while (msg[start] <= 13) start++;
				end = start;
				while (msg[end] > 13) end++;
				msg[end] = 0;
				SetDlgItemText(IDC_VERSION, &msg[start]);
				SetTimer(1, 1000, NULL);  // Joystick timer, half rate for AC4790
				ambootloader = false;
				pFW->EnableWindow(true);
			}
			else 
				ambootloader = true;
		}
	}
	if (Connected>0)
	{
		CString portname;
		pCom->GetLBText(pCom->GetCurSel(),portname);
		AfxGetApp()->WriteProfileString("","port",portname);
		
	
// Open log file
		int ilog = 0;
		for (ilog=0;ilog<10000;ilog++)
		{
			sprintf_s(logFilename,100,"c:\\temp\\GPM_%04i.csv",ilog);
			nResult = fopen_s(&logFile,logFilename,"r");
			if (nResult == ENOENT) break; // stop searching when the file does not exist
			if (nResult != 0) break; // any other error
			fclose(logFile);
		}
		if (nResult == ENOENT)
		{
			nResult = fopen_s(&logFile,logFilename,"wbc");
			if (nResult == 0)
			{
				// success
				// print Header
				char datebuf[128];
				_strdate_s( datebuf, 128 );
				fprintf(logFile,"GPM Monitor %i on %s.\r\nTime, System time, Latitude, Longitude, Heading, Std, PosType, Steer, Speed, Brake, RPM\r\n",ilog,datebuf);
				SetTimer(2,60000,NULL);  // close and re-open once per minute, so that no data is lost when GPM monitor closes.
			} else {
				TRACE1("Error opening logfile (%i)\r\n",nResult);
			}
		}
		
	}
}

void CGPM_MonitorDlg::OnTimer(UINT_PTR nIDEvent)
{
	int nResult = 0;
	char txt[500];
	GPM_T inbuf;
	int nc = sizeof(GPM_T);

	if (nIDEvent==1)
	{
		memset(inbuf.data, 0, sizeof(GPM_T));
		Send("G", 1);
		Recv((char*)inbuf.data, &nc);		
		if (nc > 0)
		{			
			int h = (int)(inbuf.rec.time / (1000 * 60 * 60));
			int m = (int)((inbuf.rec.time - h * 3600000) / 60000);
			int s = (int)(inbuf.rec.time - h * 3600000 - m * 60000) / 1000;
			int ms = (int)(inbuf.rec.time - h * 3600000 - m * 60000 - s * 1000);
			sprintf_s(txt, 100, "Time: %02i:%02i:%02i.%03i", h, m, s, ms);
			SetDlgItemText(IDC_GPMTIME, txt);
			sprintf_s(txt, 500, "%1.10lf", inbuf.rec.latitude);
			SetDlgItemText(IDC_LATITUDE, txt);
			sprintf_s(txt, 500, "%1.10lf", inbuf.rec.longitude);
			SetDlgItemText(IDC_LONGITUDE, txt);
			sprintf_s(txt, 500, "%1.2f", inbuf.rec.heading);
			SetDlgItemText(IDC_HEADING, txt);
			sprintf_s(txt, 500, "%1.3f", inbuf.rec.std);
			SetDlgItemText(IDC_STD, txt);
			sprintf_s(txt, 500, "%i", inbuf.rec.posType);
			SetDlgItemText(IDC_POSTYPE, txt);
			sprintf_s(txt, 500, "%i", inbuf.rec.steer);
			SetDlgItemText(IDC_STEER, txt);
			sprintf_s(txt, 500, "%i", inbuf.rec.speed);
			SetDlgItemText(IDC_SPEED, txt);
			sprintf_s(txt, 500, "%i", inbuf.rec.brake);
			SetDlgItemText(IDC_BRAKE, txt);
			sprintf_s(txt, 500, "%i", inbuf.rec.rpm);
			SetDlgItemText(IDC_RPM, txt);
			sprintf_s(txt, 500, "%i", inbuf.rec.gear);
			SetDlgItemText(IDC_GEAR, txt);
			sprintf_s(txt, 500, "%1.2f", inbuf.rec.Vpp);
			SetDlgItemText(IDC_VPP, txt);

			if (logFile)
			{
				char timebuf[128];
				_strtime_s(timebuf, 128);
				fprintf(logFile, "%s,%f,%1.10lf,%1.10lf,%1.2f,%1.3f,%i,%i,%i,%i,%i\r\n", timebuf,
					0.001*inbuf.rec.time, inbuf.rec.latitude, inbuf.rec.longitude, inbuf.rec.heading,
					inbuf.rec.std, inbuf.rec.posType, inbuf.rec.steer, inbuf.rec.speed, inbuf.rec.brake, inbuf.rec.rpm);
			}
		}	
		Invalidate(0);
		UpdateWindow();		
	}

	if (nIDEvent==2)
	{
		if (logFile)
		{
			fclose(logFile);
			Sleep(100);
			fopen_s(&logFile,logFilename,"abc");
		}
	}

	if (nIDEvent == 4)
	{
		char rtn[100];
		int nc = 100;
		memset(rtn, 0, 100);
		Recv(rtn, &nc);
		if ((nc >= 1)||(iFW==0))
		{
			wFW = 0;
			sprintf_s(txt, 100, "Upgrading: pkt=%i/%i",iFW/ BLK_FWSIZE,nFW/ BLK_FWSIZE);
			SetDlgItemText(IDC_GPMTIME, txt);
			if ((rtn[nc-1] == 'K')||(iFW==0)) { // okay. send next
				if (iFW > nFW) {
					KillTimer(4);
					SetTimer(1, 1000, NULL);
					SetTimer(2, 1000, NULL);
					// reboot to reflash
					//Send("RS0\r", 4);
					MessageBox("Reset firmware to re-program device.", "GPM Update", MB_ICONINFORMATION | MB_OK);
					fclose(fw);
					fw = NULL;
					sprintf_s(rtn, 100, "CRC=%i", crc);
					SetDlgItemText(IDC_CRC, rtn);
				} else {
					iFW += BLK_FWSIZE;
					fread_s(fwdata, MAX_FWSIZE, 1, BLK_FWSIZE, fw);
					for (int i = 0; i < BLK_FWSIZE; i++) crc += fwdata[i];
					//memset(fwdata, 'A', BLK_FWSIZE);
					//for (int i=0;i< BLK_FWSIZE;i++)
					//	sprintf_s(&usbdata[2 * i], 10, "%02X", fwdata[i]);
					//Send(usbdata, BLK_FWSIZE*2);
					Send(fwdata, BLK_FWSIZE);
				}
			}; 
			if (rtn[nc-1] == 'R') { // repeat last packet
			}; 
			if (rtn[nc-1] == 'Q') { // exit
				KillTimer(4);
				SetTimer(1, 1000, NULL);
				SetTimer(2, 1000, NULL);
			};
		}
		else {
			wFW++;
			if (wFW > 20)
			{
				Send(usbdata, BLK_FWSIZE*2); // timeout; send again
				Send(fwdata, BLK_FWSIZE); // timeout; send again
				wFW = 0;
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}


void CGPM_MonitorDlg::OnBnClickedReset()
{
	if (Connected)
	{
		Send("rs0\r", 4);
		Sleep(100);
		OnBnClickedConnect();
	}
}

void CGPM_MonitorDlg::OnDestroy()
{
	if (logFile)
	{
		fclose(logFile);
		logFile = NULL;
	}
	if ((Connected==2) && (gpmSock))		
	{
		shutdown(gpmSock,SD_SEND);
		Sleep(100);
		closesocket(gpmSock);
	}
	if ((Connected==1) && (hCommPort))
	{
		CancelIo(hCommPort);
		CloseHandle(hCommPort);
	}
	
	CDialog::OnDestroy();
}

void CGPM_MonitorDlg::OnBnClickedUpdate()
{
	char key = 27;
	char szFile[500];
	
	if (!Connected) 
		return;

	if (upgrading == false)
	{
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = m_hWnd;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "Firmware\0*.bin\0All\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileName(&ofn) == false)
		{
			DWORD nErr = CommDlgExtendedError();
			return;
		}
		errno_t nErr = fopen_s(&fw, ofn.lpstrFile, "rb");
		fseek(fw, 0L, SEEK_END);
		nFW = ftell(fw);
		rewind(fw);
		iFW = 0;
		wFW = 0;
		crc = 0;
		KillTimer(2);
		KillTimer(1);
		upgrading = true;
		if (!ambootloader)
		{
			// Enter "upload firmware" mode of the App
			Send("rs3\r", 4);
			Sleep(100);
		}
		SetTimer(4, 10, NULL);
		SetDlgItemText(IDC_UPDATE, "Cancel");
	}
	else {
		KillTimer(4);
		SetTimer(1, 1000, NULL);
		SetTimer(2, 1000, NULL);
		upgrading = false;
		Sleep(100);
		Send(&key, 1);// exit upgrade mode.
		SetDlgItemText(IDC_UPDATE, "Update");
		if (fw) fclose(fw);
		fw = NULL;
	}
}


UINT __cdecl ProcDiscoverThreadFunction(LPVOID pParam)
{
	CGPM_MonitorDlg*  dlg = (CGPM_MonitorDlg*)pParam;
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

	bind(s, (sockaddr *)&si_me, sizeof(sockaddr));

	while (!found)
	{
		char buf[10000];
		int slen = sizeof(sockaddr);
		if (recvfrom(s, buf, sizeof(buf) - 1, 0, (sockaddr *)&dlg->si_other, &slen) > 0)
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

	AfxEndThread(0);
	return false;
}

void CGPM_MonitorDlg::OnBnClickedAutoconnect()
{
	AfxGetApp()->WriteProfileInt(NULL, "autoConnect", pAutoConnect->GetCheck());
}


void CGPM_MonitorDlg::OnBnClickedServer()
{
	AfxGetApp()->WriteProfileInt(NULL, "tcpServer", pServer->GetCheck());
}


// see https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code
UINT __cdecl ProcServerThreadFunction(LPVOID pParam)
{
	CGPM_MonitorDlg*  dlg = (CGPM_MonitorDlg*)pParam;

	char port[50];
	struct addrinfo *result = NULL;
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
					recvbuf[iResult] = 0;
					TRACE("Bytes received from client: %d\n", iResult);
					// Send to GPM

					fSuccess = WriteFile(hCommPort, recvbuf, iResult - 1, &nc, NULL);
					if (fSuccess == 0) {
						int nErr = GetLastError();
						char errMsg[250];
						FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
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
					char errMsg[250];
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

void CGPM_MonitorDlg::OnBnClickedSvrconnect()
{
	if (svrrunning)
	{
		svrrunning = false;
		/*
		Sleep(100);

		SetDlgItemText(IDC_SVRCONNECT, "Connect");
		pSvrCOMList->EnableWindow(true);
		svrrunning = false;

		//theApp.m_pServerThread->Delete(); 
		::TerminateThread(theApp.m_pServerThread->m_hThread, 0);
		CloseHandle(theApp.m_pServerThread->m_hThread);
		*/
	}
	else
		theApp.m_pServerThread = AfxBeginThread(&ProcServerThreadFunction, this);
	
}


void CGPM_MonitorDlg::OnCbnSelchangeSvrcomlist()
{
	int icom = pSvrCOMList->GetCurSel();
	svrcom = (int)pSvrCOMList->GetItemData(icom);

	CString portname;
	pSvrCOMList->GetLBText(pSvrCOMList->GetCurSel(), portname);
	AfxGetApp()->WriteProfileString("", "server", portname);
}
