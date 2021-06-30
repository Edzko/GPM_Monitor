// GPM_VibrationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GPM_Vibration.h"
#include "GPM_VibrationDlg.h"

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

// CGPM_VibrationDlg dialog

CGPM_VibrationDlg::CGPM_VibrationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGPM_VibrationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//CommTimeouts = NULL;
}

void CGPM_VibrationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGPM_VibrationDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RESET, &CGPM_VibrationDlg::OnBnClickedReset)
	ON_WM_DESTROY()
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_UPDATE, &CGPM_VibrationDlg::OnBnClickedUpdate)
	ON_BN_CLICKED(ID_HELP, &CGPM_VibrationDlg::OnBnClickedHelp)
	ON_EN_CHANGE(IDC_FREQ, &CGPM_VibrationDlg::OnEnChangeFreq)
	ON_EN_CHANGE(IDC_GAIN, &CGPM_VibrationDlg::OnEnChangeGain)
	ON_CBN_SELCHANGE(IDC_AXIS, &CGPM_VibrationDlg::OnCbnSelchangeAxis)
	ON_CBN_SELCHANGE(IDC_SCALE, &CGPM_VibrationDlg::OnCbnSelchangeScale)
	ON_CBN_SELCHANGE(IDC_FFTWNDFCN, &CGPM_VibrationDlg::OnCbnSelchangeFftwndfcn)
	ON_CBN_SELCHANGE(IDC_DWNSAMPLES, &CGPM_VibrationDlg::OnCbnSelchangeDwnsamples)
	ON_CBN_SELCHANGE(IDC_SAMPLESIZE, &CGPM_VibrationDlg::OnCbnSelchangeSamplesize)
	ON_BN_CLICKED(IDC_KEY, &CGPM_VibrationDlg::OnBnClickedKey)
	ON_BN_CLICKED(IDC_POSTNOW, &CGPM_VibrationDlg::OnBnClickedPostnow)
	ON_CBN_SELCHANGE(IDC_COMLIST, &CGPM_VibrationDlg::OnCbnSelchangeComlist)
END_MESSAGE_MAP()


// CGPM_VibrationDlg message handlers


BOOL CGPM_VibrationDlg::OnInitDialog()
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

	

	pCOMList = (CComboBox*)GetDlgItem(IDC_COMLIST);
	CString portname = AfxGetApp()->GetProfileString("", "Port", "");
	if (pCOMList->SelectString(0, portname) == CB_ERR)
	{
		pCOMList->SetCurSel(0);
	}
	cim = -1;  // invalid 
	CString project = AfxGetApp()->GetProfileString("", "Project", "");
	SetDlgItemText(IDC_PROJECT, project);
	CString device = AfxGetApp()->GetProfileString("", "Device", "");
	SetDlgItemText(IDC_DEVICE, device);
	CString topic = AfxGetApp()->GetProfileString("", "Topic", "");
	SetDlgItemText(IDC_TOPIC, topic);

	for (int i = 0; i < 10; i++)
	{
		vm[i].gpmSock = NULL;
		vm[i].Connected = 0;
		vm[i].upgrading = false;
		vm[i].ambootloader = false;
		strcpy_s(vm[i].ip, 100, "");
	}
	//CComboBox* pVeh = (CComboBox*)GetDlgItem(IDC_VEHSEL);
	//int selVeh = AfxGetApp()->GetProfileInt("", "vehSelect", 0);
	//pVeh->SetCurSel(selVeh);

	pUpdate = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	pUpdate->ShowWindow(false);
	pUpdate->SetRange(0, 100);

	strcpy_s(jwt, 1000, "");

	pRate = (CSliderCtrl*)GetDlgItem(IDC_RATE);
	pRate->SetRange(1, 4);
	pRate->SetPos(1);
	iAxis = 0;
	myupdate = false;
	fw = NULL;
	CButton *pFW = (CButton*)GetDlgItem(IDC_UPDATE);
	pFW->EnableWindow(false);

	theApp.m_pDiscoverThread = AfxBeginThread(&ProcDiscoverThreadFunction, this);
	SetTimer(1, 500, NULL);

	logFile = NULL;

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CGPM_VibrationDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGPM_VibrationDlg::OnPaint()
{
	int im = 0;
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
		int im = pCOMList->GetCurSel();
		char vmname[100];
		if (im >= 0) {
			pCOMList->GetLBText(im, vmname);
			for (int i = 0; i < 10; i++)
				if (strcmp(vmname, vm[i].ip) == 0) {

					CPaintDC dc(this); // device context for painting

					drawChart(&dc);
				}
		}
		CDialog::OnPaint();
	}
}

void CGPM_VibrationDlg::drawChart(CDC* dc)
{
	RECT rc;
	GetClientRect(&rc);
	POINT p[128];
	int im = 0;
	HBRUSH hOldBrush, hBkg = CreateSolidBrush(RGB(255, 255, 255));
	hOldBrush = (HBRUSH)dc->SelectObject(hBkg);
	dc->Rectangle(149, rc.bottom - 19, 150 + 3 * 128, rc.bottom - 20 - 256);
	for (int i = 0; i < 128; i++) {
		p[i].x = 150 + 3*i;
		p[i].y = rc.bottom - 20 - vm[cim].data[i];
	}
	HPEN hOldPen, hPen;
	switch (iAxis) {
	case 0:
		hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		break;
	case 1:
		hPen = CreatePen(PS_SOLID, 1, RGB(0, 200, 0));
		break;
	case 2:
		hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 255));
		break;
	default:
		hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	}
	hOldPen = (HPEN)dc->SelectObject(hPen);
	dc->Polyline(p, 128);
	dc->SelectObject(hOldBrush);
	dc->SelectObject(hOldPen);
	DeleteObject(hPen);
	DeleteObject(hBkg);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGPM_VibrationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGPM_VibrationDlg::Send(int im, char *msg, int len)
{
	if ((vm[im].Connected==2) && (vm[im].gpmSock))
	{
		int nResult = send(vm[im].gpmSock,msg,len,0);
		if (nResult==SOCKET_ERROR)
		{
			int nErr=WSAGetLastError();
			char errMsg[500];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,nErr,0,errMsg,500,NULL);
			TRACE1("Error: %s\r\n",errMsg);
		}
	}
	
}

void CGPM_VibrationDlg::Recv(int im, char *msg, int *len)
{
	if ((vm[im].Connected==2) && (vm[im].gpmSock))
	{
		
		int nResult = recv(vm[im].gpmSock,msg,*len,0);
		if (nResult == SOCKET_ERROR)
		{
			int nErr = GetLastError();
			if (nErr != WSAETIMEDOUT) {
				char errMsg[500];
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErr, 0, errMsg, 500, NULL);
				TRACE1("Error: %s\r\n", errMsg);
			}
		}
		*len = nResult;
	} 
	
	else *len = 0;
}

void CGPM_VibrationDlg::UpdateGUI(int im)
{
	char valstr[10];
	myupdate = true;
	
	CComboBox* p = (CComboBox*)GetDlgItem(IDC_SAMPLESIZE);
	switch (vm[im].cfgdata.vibpnts) {
	case 2048: p->SetCurSel(0); break;
	case 1024: p->SetCurSel(1); break;
	case 512: p->SetCurSel(2); break;
	}
	
	p = (CComboBox*)GetDlgItem(IDC_SCALE);
	if (vm[im].cfgdata.fftlog) p->SetCurSel(0);
	else p->SetCurSel(1);

	p = (CComboBox*)GetDlgItem(IDC_DWNSAMPLES);
	p->SetCurSel((int)(vm[im].cfgdata.fftsamps) - 1);
	
	p = (CComboBox*)GetDlgItem(IDC_FFTWNDFCN);
	p->SetCurSel(vm[im].cfgdata.fftWnd - 1);

	_itoa_s(vm[im].cfgdata.fftfreq, valstr, 10);
	SetDlgItemText(IDC_FREQ, valstr);

	_itoa_s((int)(1000.0 * vm[im].cfgdata.fftscale), valstr, 10);
	SetDlgItemText(IDC_GAIN, valstr);
	
	myupdate = false;
}

void CGPM_VibrationDlg::parseSP(int im, char* msg, int len)
{
	char valstr[10];
	unsigned char *c = (unsigned char*)&vm[im].cfgdata, ic = 0;
	memset(valstr, 0, 10);
	// make sure string starts with "SP#"
	for (int i = 3; i < len; i += 2) {
		strncpy_s(valstr,10, &msg[i], 2);
		sscanf_s(valstr, "%hhX", &c[ic++]);
	}
	UpdateGUI(im);
	TRACE0("Received Configurations.");
	noUpdate = true;
}

void CGPM_VibrationDlg::Disconnect(int im)
{
	if ((vm[im].Connected == 2) && (vm[im].gpmSock))
	{
		shutdown(vm[im].gpmSock, SD_SEND);
		Sleep(100);
		closesocket(vm[im].gpmSock);
		vm[im].gpmSock = NULL;
		vm[im].Connected = 0;
		strcpy_s(vm[im].ip, 100, "");
	}
}

/// <summary>
/// Connect to Cloud
/// </summary>
/// https://www.tutorialspoint.com/python/python_command_line_arguments.htm
/// https://stackoverflow.com/questions/9776857/curl-simple-https-request-returns-nothing-c

void CGPM_VibrationDlg::Connect(int im)
{
	char msg[500];
	int nResult;
	sockaddr_in sa;
	CButton *pFW = (CButton*)GetDlgItem(IDC_UPDATE);
	KillTimer(4);

	
	vm[im].gpmSock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	USHORT port = 2000;
	strcpy_s(msg, 100, &vm[im].ip[6]);
	for (int i = 0; i < 100; i++) if (msg[i] == ')') msg[i] = 0;
	
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(msg);
	sa.sin_port = htons(port);
			
	nResult = connect(vm[im].gpmSock,(const sockaddr*)&sa,sizeof(sa));
	if (nResult==SOCKET_ERROR)
	{
		int nErr=WSAGetLastError();
		char errMsg[500];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,nErr,0,errMsg,500,NULL);
		TRACE1("Error: %s\r\n",errMsg);
	}
	vm[im].Connected = 2;
	vm[im].upgrading = false;
	int timeout = 100;
	nResult = setsockopt(vm[im].gpmSock,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));
	if (nResult==SOCKET_ERROR)
	{
		int nErr=WSAGetLastError();
		char errMsg[500];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,nErr,0,errMsg,500,NULL);
		TRACE1("Error: %s\r\n",errMsg);
	}

	nResult = recv(vm[im].gpmSock,msg,100,0);

	Send(im,"?v\r",3);
	Sleep(100);
	int nc = 500;
	Recv(im, msg,&nc);
	TRACE1("Error: %s\r\n",msg);
	if (nc>0)
	{
		msg[nc-1] = 0;
		strcpy_s(vm[im].version, 100, msg);
		int start = 0, end = 0;
		while ((msg[start] <= 13) &&(start<nc-1)) start++;
		end = start;
		while ((msg[end] > 13) && (end<nc-1)) end++;
		msg[end] = 0;
		SetDlgItemText(IDC_VERSION, &msg[start]);

		TRACE1("Connected. Version:\r\n%s",msg);

		vm[im].cfgdata.version = 0;

		pFW->EnableWindow(true);
		vm[im].ambootloader = false;
	}
	//else 
	//{
	//	shutdown(vm[im].gpmSock, SD_SEND);
	//	Sleep(100);
	//	closesocket(vm[im].gpmSock);
	//	vm[im].Connected = 0;
	//	TRACE0("Disconnected.");
	//}
		

	if ((vm[im].Connected>0) && (logFile==NULL))
	{
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
				fprintf(logFile,"Vibration Monitor %i on %s.\r\nTime, System time, Latitude, Longitude, Heading, Std, PosType, Steer, Speed, Brake, RPM, WS FL, WS FR, WS RL, WS RR\r\n",ilog,datebuf);
				SetTimer(2,60000,NULL);  // close and re-open once per minute, so that no data is lost when GPM monitor closes.
			} else {
				TRACE1("Error opening logfile (%i)\r\n",nResult);
			}
		}
		
	}
}

void CGPM_VibrationDlg::ProcessPeriodicVM(int ivm)
{
	int nResult = 0;
	char txt[1000];
	int nc = 1000;
	int im = 0;

	if (strlen(vm[ivm].ip) == 0)
		return;

	if (vm[ivm].Connected == 0)
	{
		Connect(ivm);
		return;
	}

	if (vm[ivm].cfgdata.version == 0)
	{
		Send(ivm, "sp#\r", 4);
		Sleep(100);
		nc = 500;
		Recv(ivm, txt, &nc);
		if (nc > 0)
			parseSP(ivm, txt, nc);
		return;
	}
	if (iAxis < 3)
		sprintf_s(txt, 1000, "VI7,%i\r",iAxis);
	else
		sprintf_s(txt, 1000, "VI7\r");
	Send(ivm, txt, strlen(txt));

	Recv(ivm,(char*)vm[ivm].data, &nc);
	if (nc > 0)
	{
		if (nc >= 128) {
			TRACE0("Received data\r\n");
			Invalidate(false);
			UpdateWindow();
		}
		//CButton* pFlag = (CButton*)GetDlgItem(IDC_NTRIP);
		//if (inbuf.rec.WF_State == 25) pFlag->SetCheck(BST_CHECKED); else pFlag->SetCheck(BST_UNCHECKED);

		if (logFile)
		{
			//char timebuf[128];
			//_strtime_s(timebuf, 128);
			//fprintf(logFile, "%s,%f,%1.10lf,%1.10lf,%1.2f,%1.3f,%i,%i,%i,%i,%i,%i,%i,%i,%i\r\n", timebuf,
			//	0.001*inbuf.rec.time, inbuf.rec.latitude, inbuf.rec.longitude, inbuf.rec.heading,
			//	inbuf.rec.std, inbuf.rec.posType, inbuf.rec.steer, inbuf.rec.speed, inbuf.rec.brake, inbuf.rec.rpm,
			//	inbuf.rec.wheelspeed[0], inbuf.rec.wheelspeed[1], inbuf.rec.wheelspeed[2], inbuf.rec.wheelspeed[3]);
		}
	}
	else {
		// disconnect ?
	}

}

void CGPM_VibrationDlg::OnTimer(UINT_PTR nIDEvent)
{
	int nResult = 0;
	int nc = 1000;
	int im = 0;

	if (nIDEvent==1)
	{
		//pRate = (CSliderCtrl*)GetDlgItem(IDC_RATE);
		int rate = pRate->GetPos();
		if (rate>0)
			SetTimer(1, 1000 / rate, NULL);

		for (int ivm = 0; ivm < 10; ivm++)
			ProcessPeriodicVM(ivm);
	
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
		Recv(cim, rtn, &nc);
		if ((nc >= 1)||(iFW==0))
		{
			wFW = 0;
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
					Send(cim, (char*)fwdata, BLK_FWSIZE + 0x10);
					Sleep(100);
					SetTimer(1, 1000, NULL);
					SetTimer(2, 1000, NULL);
					vm[im].upgrading = false;
					pUpdate->ShowWindow(false);
					pUpdate->SetPos(0);
					SetDlgItemText(IDC_UPDATE, "Firmware Update");
					SetDlgItemText(IDC_GPMTIME, "Firmware update complete.");
					Disconnect(im); // Disconnect, because system reboots
				} else {					
					memcpy(fwdata+0x10, &fwfile[iFW], BLK_FWSIZE);
					fwdata[0] = 0x55;
					fwdata[1] = 0xAA;
					fwdata[2] = (iFW / BLK_FWSIZE) >> 8;
					fwdata[3] = (iFW / BLK_FWSIZE) & 0xFF;
					Send(cim, (char*)fwdata, BLK_FWSIZE+0x10);
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
				Send(cim, (char*)fwdata, BLK_FWSIZE+0x10);
				iFW += BLK_FWSIZE;
			}; 
			if (rtn[0] == 'Q') { // exit
				KillTimer(4);
				SetTimer(1, 1000, NULL);
				SetTimer(2, 1000, NULL);
				vm[im].upgrading = false;
				SetDlgItemText(IDC_UPDATE, "Firmware Update");
				pUpdate->ShowWindow(false);
				//OnBnClickedConnect(); // Disconnect, because system reboots
			};
		}
		else {
			wFW++;
			if (wFW > 5)
			{
				Send(cim, (char*)fwdata, BLK_FWSIZE+0x10); // timeout; send again
				wFW = 0;
			}
		}
	}

	

	CDialog::OnTimer(nIDEvent);
}


void CGPM_VibrationDlg::OnBnClickedReset()
{
	if (vm[cim].Connected)
	{
		Send(cim, "rs0\r", 4);
		Sleep(100);
		Disconnect(cim);
	}
}

void CGPM_VibrationDlg::OnDestroy()
{
	int im = 0;
	if (logFile)
	{
		fclose(logFile);
		logFile = NULL;
	}
	for (im = 0; im < 10; im++) {
		if ((vm[im].Connected == 2) && (vm[im].gpmSock))
		{
			shutdown(vm[im].gpmSock, SD_SEND);
			Sleep(100);
			closesocket(vm[im].gpmSock);
		}
	}

	CString p;
	GetDlgItemText(IDC_PROJECT, p);
	AfxGetApp()->WriteProfileString("", "Project", p);

	CString d;
	GetDlgItemText(IDC_DEVICE, d);
	AfxGetApp()->WriteProfileString("", "Device", d);

	CString t;
	GetDlgItemText(IDC_TOPIC, t);
	AfxGetApp()->WriteProfileString("", "Topic", t);

	TerminateThread(theApp.m_pDiscoverThread, 0);

	CDialog::OnDestroy();
}

void CGPM_VibrationDlg::OnBnClickedUpdate()
{
	char key = 27;
	char szFile[500];
	char msg[100];
	int nc;

	if (!vm[cim].Connected)
		return;

	if (vm[cim].upgrading == false)
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
		memset(fwfile, 255, sizeof(fwfile));
		if (fopen_s(&fw, ofn.lpstrFile, "rb")) 
			return;

		nFW = fread(fwfile, 1, sizeof(fwfile), fw);
		nFW = (nFW / ERASE_BLOCK_SIZE + 1) * ERASE_BLOCK_SIZE;
		fclose(fw);
		crc = 0;
		for (int i = 0; i < nFW; i++) crc += fwfile[i];
		fw = NULL;
		iFW = 0;
		wFW = 0;
		KillTimer(1);
		vm[cim].upgrading = true;
		if (!vm[cim].ambootloader)  // make sure we're not connected to the bootloader
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
					Send(cim, msg, (int)strlen(msg));
					Sleep(100);
					nc = 100;
					while (nc == 100)
						Recv(cim, msg, &nc);
					if (nc > 0) {
						if (msg[nc - 1] == 'K') {
							SetTimer(4, 100, NULL);
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
				if (n == 3) {
					vm[cim].upgrading = false;
					SetTimer(1, 1000 / pRate->GetPos(), NULL);
				}
			}
		}
		
	}
	else {
		KillTimer(4);
		SetTimer(1, 1000 / pRate->GetPos(), NULL);
		vm[cim].upgrading = false;
		Sleep(100);
		SetDlgItemText(IDC_UPDATE, "Firmware Update");
		pUpdate->ShowWindow(false);
	}
}







void CGPM_VibrationDlg::OnBnClickedHelp()
{
	::HtmlHelp(m_hWnd, "GPM_Vibration.chm", HH_DISPLAY_TOC, 0);
}


void CGPM_VibrationDlg::OnBnClickedSetutctime()
{
	// Get Time and Date and set UBLOX receiver UTC time
}


void CGPM_VibrationDlg::OnEnChangeFreq()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (myupdate) return;
	CString msg;
	GetDlgItemText(IDC_FREQ,msg);
	msg = "SP81," + msg + "\r";
	Send(cim, msg.GetBuffer(), msg.GetLength());
}


void CGPM_VibrationDlg::OnEnChangeGain()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	if (myupdate) return;

	// TODO:  Add your control notification handler code here
	CString msg;
	GetDlgItemText(IDC_FREQ, msg);
	msg = "SP80," + msg + "\r";
	char* cmsg = msg.GetBuffer();
	Send(cim, cmsg, msg.GetLength());
	TRACE1("Scale = %s\r\n", msg);
}


void CGPM_VibrationDlg::OnCbnSelchangeAxis()
{
	// TODO: Add your control notification handler code here
	if (myupdate) return;
	CComboBox* p = (CComboBox*)GetDlgItem(IDC_AXIS);
	iAxis = p->GetCurSel();
	if (iAxis < 3) {
		char msg[] = "LD6,6\r";
		msg[4] += iAxis;
		Send(cim, msg, 6);
	}  
}


void CGPM_VibrationDlg::OnCbnSelchangeScale()
{
	// TODO: Add your control notification handler code here
	if (myupdate) return;
	CComboBox* p = (CComboBox*)GetDlgItem(IDC_SCALE);
	int i = p->GetCurSel();
	char msg[] = "SP82,0\r";
	msg[5] += i;
	Send(cim, msg, 7);
}


void CGPM_VibrationDlg::OnCbnSelchangeFftwndfcn()
{
	// TODO: Add your control notification handler code here
	if (myupdate) return;
	CComboBox* p = (CComboBox*)GetDlgItem(IDC_FFTWNDFCN);
	int i = p->GetCurSel()+1;
	char msg[10];
	sprintf_s(msg, 10, "SP83,%i\r", i);
	Send(cim, msg, strlen(msg));
}


void CGPM_VibrationDlg::OnCbnSelchangeDwnsamples()
{
	// TODO: Add your control notification handler code here
	if (myupdate) return;
	CComboBox* p = (CComboBox*)GetDlgItem(IDC_SCALE);
	int i = p->GetCurSel();
	if (i == 0) Send(cim, "SP85,0\r", 7);
	else Send(cim, "SP85,1\r", 7);
}


void CGPM_VibrationDlg::OnCbnSelchangeSamplesize()
{
	// TODO: Add your control notification handler code here
	if (myupdate) return;
	CComboBox* p = (CComboBox*)GetDlgItem(IDC_SCALE);
	int i = p->GetCurSel();
	switch (i) {
	case 0: Send(cim, "SP84,2048\r", 10); break;
	case 1: Send(cim, "SP84,1024\r", 10); break;
	case 2: Send(cim, "SP84,512\r", 9); break;
	}
}


void CGPM_VibrationDlg::OnBnClickedKey()
{
	CString project;
	GetDlgItemText(IDC_PROJECT, project);
	
	char ccmd[1000];
	sprintf_s(ccmd, 1000, "getjwt.bat %s %s", 
		project.GetBuffer(),
		"c:/users/edzko/rsa_private.pem");
	WinExec(ccmd,SW_HIDE);
	
	FILE* fid;
	fopen_s(&fid, "JWT.tok", "r");
	if (fid) {
		int n = fread(jwt, 1, 1000, fid);
		fclose(fid);
		if (n<1000)
			jwt[n] = 0;
		SetDlgItemText(IDC_KEY, "JWT.TOK");
	}
	else strcpy_s(jwt, 1000, "");
}


void CGPM_VibrationDlg::OnBnClickedPostnow()
{
	// curl -X POST -H 'authorization: Bearer JWT' -H 'content-type: application/json' --data '{"binary_data": "DATA"}' -H 'cache-control: no-cache' 'https://cloudiotdevice.googleapis.com/v1/projects/{project-id}/locations/{cloud-region}/registries/{registry-id}/devices/{device-id}:publishEvent'

	CString project;
	GetDlgItemText(IDC_PROJECT, project);
	CString device;
	GetDlgItemText(IDC_DEVICE, device);
	CString topic;
	GetDlgItemText(IDC_TOPIC, topic);

	char ccmd[2000];
	sprintf_s(ccmd, 2000, "curl -X POST -H 'authorization: Bearer %s' -H 'content-type: application/json' --data '{\"ascii_data\": \"my first data\", \"%s\": \"VIB_DATA\"}' -H 'cache-control: no-cache' 'https://cloudiotdevice.googleapis.com/v1/projects/%s/locations/%s/registries/%s/devices/%s:publishEvent'",
		jwt, topic.GetBuffer(), project.GetBuffer(), "global", "MAGNA_GPM_DEV_EDZKO", device.GetBuffer());
	//WinExec(ccmd, SW_HIDE);
	TRACE0(ccmd);
	// use --libcurl <code.c> to generate code from curl command


}


void CGPM_VibrationDlg::OnCbnSelchangeComlist()
{
	int im;
	int i = pCOMList->GetCurSel();
	if (i >= 0) {
		char vmname[100];
		pCOMList->GetLBText(i, vmname);
		for (im = 0; im < 10; im++)
			if (strcmp(vm[cim].ip,vmname)==0)
				UpdateGUI(im);
	}
}
