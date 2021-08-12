// GPM_MonitorDlg.cpp : implementation file
//

#include "stdafx.h"
#include <Setupapi.h>
#include <Devguid.h>
#include "GPM_AHRS.h"
#include "GPM_Monitor.h"
#include "GPM_MonitorDlg.h"

#include "build.h"
#include "mydate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


unsigned char fwdata[MAX_FWSIZE];
char usbdata[MAX_FWSIZE];
unsigned char fwfile[500000];

extern UINT __cdecl ProcDiscoverThreadFunction(LPVOID pParam);
extern UINT __cdecl ProcServerThreadFunction(LPVOID pParam);
extern UINT __cdecl ProcSimulinkThreadFunction(LPVOID pParam);

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
	//CommTimeouts = NULL;
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
	ON_BN_CLICKED(IDC_SIMULINKCONNECT, &CGPM_MonitorDlg::OnBnClickedSimulinkconnect)
	ON_BN_CLICKED(IDC_SIMULINK, &CGPM_MonitorDlg::OnBnClickedSimulink)
	ON_CBN_SELCHANGE(IDC_SIMULINKCOMLIST, &CGPM_MonitorDlg::OnSelchangeSimulinkcomlist)
	ON_CBN_SELCHANGE(IDC_VEHSEL, &CGPM_MonitorDlg::OnCbnSelchangeVehSel)
	ON_BN_CLICKED(IDC_DRFLAG, &CGPM_MonitorDlg::OnBnClickeddrFlag)
	ON_BN_CLICKED(IDC_NTRIP, &CGPM_MonitorDlg::OnBnClickedNtrip)
	ON_BN_CLICKED(ID_HELP, &CGPM_MonitorDlg::OnBnClickedHelp)
	ON_BN_CLICKED(IDC_SETUTCTIME, &CGPM_MonitorDlg::OnBnClickedSetutctime)
	ON_CBN_SELCHANGE(IDC_COMLIST, &CGPM_MonitorDlg::OnCbnSelchangeComlist)
	ON_BN_CLICKED(IDC_LOADCONFIG, &CGPM_MonitorDlg::OnBnClickedLoadconfig)
	ON_BN_CLICKED(IDC_SAVECONFIG, &CGPM_MonitorDlg::OnBnClickedSaveconfig)
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
	CString portname = AfxGetApp()->GetProfileString("", "Port", "");
	if (pCOMList->SelectString(0, portname) == CB_ERR)
	{
		pCOMList->SetCurSel(0);
	}
	autoselect = true;
	//CComboBox* pVeh = (CComboBox*)GetDlgItem(IDC_VEHSEL);
	//int selVeh = AfxGetApp()->GetProfileInt("", "vehSelect", 0);
	//pVeh->SetCurSel(selVeh);

	pSvrCOMList = (CComboBox*)GetDlgItem(IDC_SVRCOMLIST);
	GetCOMPorts(pSvrCOMList);
	CEdit* pPort = (CEdit*)GetDlgItem(IDC_SVRPORT);
	SetDlgItemText(IDC_SVRPORT, "2000");
	pPort->EnableWindow(0);
	portname = AfxGetApp()->GetProfileString("", "server", "");
	pSvrCOMList->SelectString(0, portname);
	pUpdate = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	pUpdate->ShowWindow(false);
	pUpdate->SetRange(0, 100);

	pSimulinkCOMList = (CComboBox*)GetDlgItem(IDC_SIMULINKCOMLIST);
	GetCOMPorts(pSimulinkCOMList);
	pPort = (CEdit*)GetDlgItem(IDC_SIMULINKPORT);
	SetDlgItemText(IDC_SIMULINKPORT, "9999");
	pPort->EnableWindow(0);
	SetDlgItemText(IDC_SIMULINKIP, "192.168.140.203");
	portname = AfxGetApp()->GetProfileString("", "simulink_com", "");
	pSimulinkCOMList->SelectString(0, portname);

	pRate = (CSliderCtrl*)GetDlgItem(IDC_RATE);
	pRate->SetRange(1, 10);
	pRate->SetPos(1);

	Connected = 0;
	hCommPort = 0;
	upgrading = false;
	ambootloader = false;
	fw = NULL;
	CButton *pFW = (CButton*)GetDlgItem(IDC_UPDATE);
	pFW->EnableWindow(false);

	theApp.m_pDiscoverThread = AfxBeginThread(&ProcDiscoverThreadFunction, this);
	
	logFile = NULL;

	pAutoConnect = (CButton*)GetDlgItem(IDC_AUTOCONNECT);
	int autoConnect = AfxGetApp()->GetProfileInt("", "autoConnect", 0);
	if (autoConnect) {
		pAutoConnect->SetCheck(BST_CHECKED);
		//OnBnClickedConnect();
	}
	
	pServer = (CButton*)GetDlgItem(IDC_SERVER);
	int tcpServer = AfxGetApp()->GetProfileInt("", "tcpServer", 0);
	if (tcpServer) pServer->SetCheck(BST_CHECKED);
	svrrunning = false;

	pSimulink = (CButton*)GetDlgItem(IDC_SIMULINK);
	int sim = AfxGetApp()->GetProfileInt("", "simulink", 0);
	if (sim) pServer->SetCheck(BST_CHECKED);
	simrunning = false;

	JOY_ID = -1;
	Joystick.dwXpos = 0;
	Joystick.dwYpos = 0;
	Joystick.dwRpos = 0;
	Joystick.dwUpos = 0;
	Joystick.dwVpos = 0;
	Joystick.dwZpos = 0;
	Joystick.dwButtons = 0;

	jx = 32768;
	jy = 32768;
	jr = 32768;
	jb = 0;

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
			char errMsg[500];
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
		if (nResult == SOCKET_ERROR)
		{
			int nErr = GetLastError();
			if (nErr != WSAETIMEDOUT) {
				char errMsg[500];
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
				TRACE1("Error: %s\r\n", errMsg);
			}
			if (nErr == WSAECONNABORTED) {
				OnBnClickedConnect();
			}
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

void CGPM_MonitorDlg::parseSP(char* msg, int len)
{
	char valstr[10];
	unsigned char *c = (unsigned char*)&cfgdata, ic = 0;
	memset(valstr, 0, 10);
	// make sure string starts with "SP#"
	for (int i = 3; i < len; i += 2) {
		strncpy_s(valstr,10, &msg[i], 2);
		sscanf_s(valstr, "%hhX", &c[ic++]);
	}

	noUpdate = true;
	CComboBox* pVeh = (CComboBox*)GetDlgItem(IDC_VEHSEL); 
	if (cfgdata.drVehcile>0) 
		pVeh->SetCurSel(cfgdata.drVehcile-1);
	CButton* pFlag = (CButton*)GetDlgItem(IDC_DRFLAG);
	pFlag->SetCheck(cfgdata.drFlag);
}

char msg[100];
void CGPM_MonitorDlg::OnBnClickedConnect()
{
	char CommPort[100];
	int fSuccess, nResult;
	DCB dcbCommPort;
	CComboBox* pCom = (CComboBox*)GetDlgItem(IDC_COMLIST);
	int com = (int)pCom->GetItemData(pCom->GetCurSel());
	sockaddr_in sa;
	CButton *pFW = (CButton*)GetDlgItem(IDC_UPDATE);

	KillTimer(4);

	if ((com==100) || (com==-1))
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
			int iCom = pCom->GetCurSel();
			if (iCom == -1) {
				GetDlgItemText(IDC_COMLIST, msg, 100);
			} 
			else {
				pCom->GetLBText(iCom, CommPort);
				//pCom->GetDlgItemText(CommPort);
				strcpy_s(msg, 100, &CommPort[6]);
				for (int i = 0; i < 100; i++) if (msg[i] == ')') msg[i] = 0;
			}
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
			upgrading = false;
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
			Sleep(500);
			int nc = 500;
			Recv(msg,&nc);
			if (nc>0) 
				msg[nc] = 0;
			
			if (nc>0)
			{
				int start = 0, end = 0;
				while ((msg[start] <= 13) &&(start<nc-1)) start++;
				end = start;
				while ((msg[end] > 13) && (end<nc-1)) end++;
				msg[end] = 0;
				SetDlgItemText(IDC_VERSION, &msg[start]);

				Send("sp#\r", 4);
				Sleep(100);
				nc = 500;
				Recv(msg, &nc);
				if (nc > 0)
					parseSP(msg, nc);
				SetTimer(1,500,NULL);  
				SetDlgItemText(IDC_CONNECT,"Disconnect");
				pCom->EnableWindow(false);
				pFW->EnableWindow(true);
				ambootloader = false;
			}
			else {
				shutdown(gpmSock, SD_SEND);
				Sleep(100);
				closesocket(gpmSock);
				Connected = 0;
			}
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
			SetDlgItemText(IDC_UPDATE, "Firmware Update");
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
			upgrading = false;
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

				// Get current configuration
				Send("sp#\r", 4);
				Sleep(500);
				nc = 500;
				Recv(msg, &nc);
				if (nc > 0)
					parseSP(msg, nc);

				SetTimer(1, 500, NULL);  
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
		//pCom->GetLBText(pCom->GetCurSel(),portname);
		GetDlgItemText(IDC_COMLIST, portname);
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
				fprintf(logFile,"GPM Monitor %i on %s.\r\nTime, System time, Latitude, Longitude, Heading, Std, PosType, Steer, Speed, Brake, RPM, WS FL, WS FR, WS RL, WS RR\r\n",ilog,datebuf);
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
		if (Connected == 0)
			KillTimer(1);
		else {
			//pRate = (CSliderCtrl*)GetDlgItem(IDC_RATE);
			int rate = pRate->GetPos();
			if (rate>0)
				SetTimer(1, 1000 / rate, NULL);
		}

		if (JOY_ID == -1)
		{
			// initialize joystick
			JOY_ID = JOYSTICKID1;
			Joystick.dwSize = sizeof(JOYINFOEX);
			Joystick.dwFlags = JOY_RETURNALL;
			mResult = joyGetPosEx(JOY_ID, &Joystick);
			if (mResult != JOYERR_NOERROR) {
				JOY_ID = JOYSTICKID2;
				Joystick.dwSize = sizeof(JOYINFOEX);
				Joystick.dwFlags = JOY_RETURNALL;
				mResult = joyGetPosEx(JOY_ID, &Joystick);
			}
			if (mResult == JOYERR_NOERROR) {
				mResult = joyGetDevCaps(JOY_ID, &JoyCaps, sizeof(JoyCaps));
			}
			else { JOY_ID = -1; }
			jb = 0;
			jx = jy = jr = 0x8000;

		}
		else {
			mResult = joyGetPosEx(JOY_ID, &Joystick);
			jx = Joystick.dwXpos;
			jy = Joystick.dwYpos;
			jr = Joystick.dwRpos;
			jb = Joystick.dwButtons;

			// Must hold button 1 to drive
			if ((jb & 1) == 0)
				jx = jy = jr = 0x8000;

			sprintf_s(txt, 500, "Steer: %i", (int)(jx)-0x8000);
			SetDlgItemText(IDC_JOYX, txt);
			sprintf_s(txt, 500, "Speed: %i", (int)(jy)-0x8000);
			SetDlgItemText(IDC_JOYY, txt);
			sprintf_s(txt, 500, "Steer: %X", (int)(jb));
			SetDlgItemText(IDC_JOYB, txt);
		}
	

		memset(inbuf.data, 0, 2000);
		//Send("*GG\r", 3);
		sprintf_s(txt, 500, "MC4,%i,%i,%i\r", ((int)(jx)-0x7FFF), ((int)(jy)-0x7FFF), jb);
		Send(txt, strlen(txt)); 
		
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
			//sprintf_s(txt, 500, "%i", inbuf.rec.posType);
			switch (inbuf.rec.posType) {
			case 1: SetDlgItemText(IDC_POSTYPE, "GPS (1)"); break;
			case 2: SetDlgItemText(IDC_POSTYPE, "GPS (2)"); break;
			case 3: SetDlgItemText(IDC_POSTYPE, "GPS (3)"); break;
			case 4: SetDlgItemText(IDC_POSTYPE, "RTK FIXED"); break;
			case 5: SetDlgItemText(IDC_POSTYPE, "RTK FLOAT"); break;
			default: break;
			}
			sprintf_s(txt, 500, "%i", inbuf.rec.nSats);
			SetDlgItemText(IDC_NSATS, txt);

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

			sprintf_s(txt, 500, "%1.2f", inbuf.rec.vehEm_loc);
			SetDlgItemText(IDC_ENUE, txt);
			sprintf_s(txt, 500, "%1.2f", inbuf.rec.vehNm_loc);
			SetDlgItemText(IDC_ENUN, txt);
			sprintf_s(txt, 500, "%1.1f", inbuf.rec.vehHrad_loc);
			SetDlgItemText(IDC_ENUH, txt);
			sprintf_s(txt, 500, "%1.3f", inbuf.rec.error_Em);
			SetDlgItemText(IDC_ENUDE, txt);
			sprintf_s(txt, 500, "%1.3f", inbuf.rec.error_Nm);
			SetDlgItemText(IDC_ENUDN, txt);
			sprintf_s(txt, 500, "%1.3f", inbuf.rec.error_Hdeg);
			SetDlgItemText(IDC_ENUDH, txt);

			//CButton* pFlag = (CButton*)GetDlgItem(IDC_NTRIP);
			//if (inbuf.rec.WF_State == 25) pFlag->SetCheck(BST_CHECKED); else pFlag->SetCheck(BST_UNCHECKED);

			if (logFile)
			{
				char timebuf[128];
				_strtime_s(timebuf, 128);
				fprintf(logFile, "%s,%f,%1.10lf,%1.10lf,%1.2f,%1.3f,%i,%i,%i,%i,%i,%i,%i,%i,%i\r\n", timebuf,
					0.001*inbuf.rec.time, inbuf.rec.latitude, inbuf.rec.longitude, inbuf.rec.heading,
					inbuf.rec.std, inbuf.rec.posType, inbuf.rec.steer, inbuf.rec.speed, inbuf.rec.brake, inbuf.rec.rpm,
					inbuf.rec.wheelspeed[0], inbuf.rec.wheelspeed[1], inbuf.rec.wheelspeed[2], inbuf.rec.wheelspeed[3]);
			}
		}	
		//Invalidate(0);
		//UpdateWindow();		
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

	if (nIDEvent == 4)  // firmware update
	{
		char rtn[100];
		int nc = 100;
		memset(rtn, 0, 100);
		Recv(rtn, &nc);
		if ((nc >= 1)||(iFW==0))
		{
			wFW = 0;
			if (nc > 1) {
				int _fw, nfw = sscanf_s(rtn, "K%i", &_fw); 
				if (nfw > 0) iFW = _fw * BLK_FWSIZE;
			}
			//sprintf_s(txt, 100, "Upgrading: pkt=%i/%i",iFW/ BLK_FWSIZE,nFW/ BLK_FWSIZE);
			//SetDlgItemText(IDC_GPMTIME, txt);
			pUpdate->SetPos(iFW * 100 / nFW);
			if ((rtn[0] == 'K') || (iFW==0)) { // okay. send next
				if (iFW >= nFW) {  // finished
					KillTimer(4);
					fwdata[0] = 0x55;
					fwdata[1] = 0xAA;
					fwdata[2] = 0xFF;
					fwdata[3] = 0xFF;
					fwdata[4] = crc >> 8;
					fwdata[5] = crc & 0xFF;
					Send((char*)fwdata, BLK_FWSIZE + 0x10);
					Sleep(100);
					SetTimer(1, 1000, NULL);
					SetTimer(2, 1000, NULL);
					upgrading = false;
					pUpdate->ShowWindow(false);
					pUpdate->SetPos(0);
					SetDlgItemText(IDC_UPDATE, "Firmware Update");
					SetDlgItemText(IDC_GPMTIME, "Firmware update complete.");
					OnBnClickedConnect(); // Disconnect, because system reboots
				} else {					
					memcpy(fwdata+0x10, &fwfile[iFW], BLK_FWSIZE);
					fwdata[0] = 0x55;
					fwdata[1] = 0xAA;
					fwdata[2] = (iFW / BLK_FWSIZE) >> 8;
					fwdata[3] = (iFW / BLK_FWSIZE) & 0xFF;
					Send((char*)fwdata, BLK_FWSIZE+0x10);
					iFW += BLK_FWSIZE;
				}
			}; 
			if (rtn[0] == 'R') { // repeat previous packet
				iFW -= 2*BLK_FWSIZE;
				if (iFW < 0) iFW = 0;
				memcpy(fwdata + 0x10, &fwfile[iFW], BLK_FWSIZE);
				//for (int i = 0; i < BLK_FWSIZE; i++) crc += fwdata[i];
				fwdata[0] = 0x55;
				fwdata[1] = 0xAA;
				fwdata[2] = (iFW / BLK_FWSIZE) >> 8;
				fwdata[3] = (iFW / BLK_FWSIZE) & 0xFF;
				Send((char*)fwdata, BLK_FWSIZE+0x10);
				iFW += BLK_FWSIZE;
			}; 
			if (rtn[0] == 'Q') { // exit
				KillTimer(4);
				SetTimer(1, 1000, NULL);
				SetTimer(2, 1000, NULL);
				upgrading = false;
				SetDlgItemText(IDC_UPDATE, "Firmware Update");
				pUpdate->ShowWindow(false);
				//OnBnClickedConnect(); // Disconnect, because system reboots
			};
		}
		else {
			wFW++;
			if (wFW > 5)
			{
				Send((char*)fwdata, BLK_FWSIZE+0x10); // timeout; send again
				wFW = 0;
			}
		}
	}

	if (nIDEvent == 5)
	{
		CButton* pFlag = (CButton*)GetDlgItem(IDC_NTRIP);
		pFlag->EnableWindow(true);
		KillTimer(5);
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
	TerminateThread(theApp.m_pDiscoverThread, 0);
	
	CDialog::OnDestroy();
}

void CGPM_MonitorDlg::OnBnClickedUpdate()
{
	char key = 27;
	char szFile[500];
	char msg[100];
	int nc;

	if (!Connected) 
		return;

	if (upgrading == false)
	{

		KillTimer(1);

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
		memset(fwfile, 255, sizeof(fwfile));
		errno_t nErr = fopen_s(&fw, ofn.lpstrFile, "rb");
		nFW = fread(fwfile, 1, sizeof(fwfile), fw);
		nFW = (nFW / ERASE_BLOCK_SIZE + 1) * ERASE_BLOCK_SIZE;
		fclose(fw);
		crc = 0;
		for (int i = 0; i < nFW; i++) crc += fwfile[i];
		fw = NULL;
		iFW = 0;
		wFW = 0;
		
		
		if (!ambootloader)  // make sure we're not connected to the bootloader
		{
			// Enter "upload firmware" mode of the App
			int n = 0;
			int iproc = 0;
			if (strstr(ofn.lpstrFile, "0512EFF")) iproc = 1;
			if (strstr(ofn.lpstrFile, "1024EFM")) iproc = 2;
			if (strstr(ofn.lpstrFile, "2048EFH")) iproc = 3;
			if (iproc == 0) {
				MessageBox("Firmware not suitable for this module. Make sure that the filename includes the Processor type.", "GPM Update Error", MB_OK | MB_ICONINFORMATION);
				SetTimer(1, 1000 / pRate->GetPos(), NULL);
			}
			else {
				for (n = 0; n < 10; n++) {
					//sprintf_s(msg, 100, "rs3\r");
					sprintf_s(msg, 100, "rs3,%i\r", iproc);
					Send(msg, (int)strlen(msg));
					Sleep(1000);
					nc = 100;
					while (nc == 100)
						Recv(msg, &nc);
					if (nc > 0) {
						if (msg[nc - 1] == 'K') {
							SetTimer(4, 100, NULL);
							upgrading = true;
							SetDlgItemText(IDC_UPDATE, "Cancel");
							SetDlgItemText(IDC_GPMTIME, "Updating");
							pUpdate = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
							pUpdate->ShowWindow(true);
							break;
						}
						if (msg[nc - 1] == 'X') {
							MessageBox("Firmware not suitable for this module.", "GPM Update Error", MB_OK | MB_ICONINFORMATION);
							n = 3;
							break;
						}
					}
				}
				if (upgrading==false) {
					SetTimer(1, 1000 / pRate->GetPos(), NULL);
				}
			}
		}
	}
	else {
		KillTimer(4);
		SetTimer(1, 1000 / pRate->GetPos(), NULL);
		upgrading = false;
		Sleep(100);
		SetDlgItemText(IDC_UPDATE, "Firmware Update");
		pUpdate->ShowWindow(false);
	}
}



void CGPM_MonitorDlg::OnBnClickedAutoconnect()
{
	AfxGetApp()->WriteProfileInt("", "autoConnect", pAutoConnect->GetCheck());
}


void CGPM_MonitorDlg::OnBnClickedServer()
{
	AfxGetApp()->WriteProfileInt("", "tcpServer", pServer->GetCheck());
}

void CGPM_MonitorDlg::OnBnClickedSimulink()
{
	AfxGetApp()->WriteProfileInt("", "simulink", pSimulink->GetCheck());
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
	CString portname;
	pSvrCOMList->GetLBText(pSvrCOMList->GetCurSel(), portname);
	AfxGetApp()->WriteProfileString("", "server", portname);
}


void CGPM_MonitorDlg::OnBnClickedSimulinkconnect()
{
	if (simrunning)
	{
		simrunning = false;
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
		theApp.m_pServerThread = AfxBeginThread(&ProcSimulinkThreadFunction, this);
}




void CGPM_MonitorDlg::OnSelchangeSimulinkcomlist()
{
	CString portname;
	pSimulinkCOMList->GetLBText(pSimulinkCOMList->GetCurSel(), portname);
	AfxGetApp()->WriteProfileString("", "simulink_com", portname);
}




void CGPM_MonitorDlg::OnCbnSelchangeVehSel()
{
	char cmd[20];
	CComboBox* pVeh = (CComboBox*)GetDlgItem(IDC_VEHSEL);
	int iVeh = pVeh->GetCurSel();
	sprintf_s(cmd, 20, "sp32,%i\r", iVeh + 1);
	Send(cmd,strlen(cmd));
	//AfxGetApp()->WriteProfileInt("", "vehSelect", iVeh);
}


void CGPM_MonitorDlg::OnBnClickeddrFlag()
{
	CButton* pFlag = (CButton*)GetDlgItem(IDC_DRFLAG);
	if (pFlag->GetCheck())
		Send("sp30,0\r", 7);
	else
		Send("sp30,1\r", 7);
}


void CGPM_MonitorDlg::OnBnClickedNtrip()
{
	CButton* pFlag = (CButton*)GetDlgItem(IDC_NTRIP);
	if (pFlag->GetCheck())
		Send("wf4\r", 5);
	else
		Send("wf5\r", 4);
	pFlag->EnableWindow(false);
	SetTimer(5, 5000, NULL);
}


void CGPM_MonitorDlg::OnBnClickedHelp()
{
	::HtmlHelp(m_hWnd, "GPM_Monitor.chm", HH_DISPLAY_TOC, 0);
}


void CGPM_MonitorDlg::OnBnClickedSetutctime()
{
	char cmd[20];
	time_t ltime;
	struct tm today;
	time(&ltime);
	_localtime64_s(&today, &ltime);
	sprintf_s(cmd, 20, "st%i,%i,%i\r", today.tm_hour, today.tm_min, today.tm_sec);
	Send(cmd, strlen(cmd));
}


void CGPM_MonitorDlg::OnCbnSelchangeComlist()
{
	autoselect = false;
}


void CGPM_MonitorDlg::OnBnClickedLoadconfig()
{
	char szFile[500], msg[1000];
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Configuration\0*.cfg\0All\0*.*\0";
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
	memset(fwfile, 255, sizeof(fwfile));
	errno_t nErr = fopen_s(&fw, ofn.lpstrFile, "rb");
	if (nErr == 0) {
		int n = fread(&cfgdata, 1, sizeof(cfgdata), fw);
		fclose(fw);
	}

	// now send to the GPS module
	strcpy_s(msg, 1000, "SP#");
	for (int i = 0; i < sizeof(cfgdata); i++) {
		unsigned char* cfgbytes = (unsigned char*)&cfgdata;
		sprintf_s(&msg[3+2*i], 5, "%02X", cfgbytes[i]);
	}
	strcat_s(msg, 1000, "\r");
	Send(msg, strlen(msg));
}


void CGPM_MonitorDlg::OnBnClickedSaveconfig()
{
	char szFile[500];
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Configuration\0*.cfg\0All\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetSaveFileName(&ofn) == false)
	{
		DWORD nErr = CommDlgExtendedError();
		return;
	}

	errno_t nErr = fopen_s(&fw, ofn.lpstrFile, "w");
	if (nErr == 0) {
		int n = fwrite(&cfgdata, 1, sizeof(cfgdata), fw);
		fclose(fw);
	}
}
