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
	}
	//CComboBox* pVeh = (CComboBox*)GetDlgItem(IDC_VEHSEL);
	//int selVeh = AfxGetApp()->GetProfileInt("", "vehSelect", 0);
	//pVeh->SetCurSel(selVeh);

	pUpdate = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	pUpdate->ShowWindow(false);
	pUpdate->SetRange(0, 100);

	strcpy_s(jwt, 1000, "");

	pRate = (CSliderCtrl*)GetDlgItem(IDC_RATE);
	pRate->SetRange(1, 10);
	pRate->SetPos(1);

	
	fw = NULL;
	CButton *pFW = (CButton*)GetDlgItem(IDC_UPDATE);
	pFW->EnableWindow(false);

	theApp.m_pDiscoverThread = AfxBeginThread(&ProcDiscoverThreadFunction, this);
	
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
		if (vm[im].Connected>0)
		{
		//CPaintDC dc(this); // device context for painting

		//drawChart(&dc);
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

	for (int i = 0; i < 128; i++) {
		p[i].x = 20 + i;
		p[i].y = rc.bottom - 20 - vm[im].data[i];
	}
	dc->Polyline(p, 128);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGPM_VibrationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGPM_VibrationDlg::Send(char *msg, int len)
{
	int im = 0;
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

void CGPM_VibrationDlg::Recv(char *msg, int *len)
{
	int im = 0;
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

void CGPM_VibrationDlg::parseSP(char* msg, int len)
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
}



/// <summary>
/// Connect to Cloud
/// </summary>
/// https://www.tutorialspoint.com/python/python_command_line_arguments.htm
/// https://stackoverflow.com/questions/9776857/curl-simple-https-request-returns-nothing-c

void CGPM_VibrationDlg::OnBnClickedConnect()
{
	char CommPort[100], msg[100];
	int nResult;
	CComboBox* pCom = (CComboBox*)GetDlgItem(IDC_COMLIST);
	int com = (int)pCom->GetItemData(pCom->GetCurSel());
	sockaddr_in sa;
	CButton *pFW = (CButton*)GetDlgItem(IDC_UPDATE);
	int im = 0;
	KillTimer(4);

	
	vm[im].gpmSock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	USHORT port = 2000;
	int iCom = pCom->GetCurSel();
	if (iCom == -1)
		GetDlgItemText(IDC_COMLIST, msg, 100);
	else {
		pCom->GetLBText(iCom, CommPort);
		//pCom->GetDlgItemText(CommPort);
		strcpy_s(msg, 100, &CommPort[6]);
		for (int i = 0; i < 100; i++) if (msg[i] == ')') msg[i] = 0;
	}
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

	Send("?v\r",3);
	Sleep(500);
	int nc = 500;
	Recv(msg,&nc);
	TRACE1("Error: %s\r\n",msg);
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
		vm[im].ambootloader = false;
	}
	else {
		shutdown(vm[im].gpmSock, SD_SEND);
		Sleep(100);
		closesocket(vm[im].gpmSock);
		vm[im].Connected = 0;
	}
		

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

void CGPM_VibrationDlg::OnTimer(UINT_PTR nIDEvent)
{
	int nResult = 0;
	char txt[500];
	int nc = 1000;
	int im = 0;
	if (nIDEvent==1)
	{
		if (vm[im].Connected == 0)
			KillTimer(1);
		else {
			pRate = (CSliderCtrl*)GetDlgItem(IDC_RATE);
			int rate = pRate->GetPos();
			if (rate>0)
				SetTimer(1, 1000 / rate, NULL);
		}

		sprintf_s(txt, 500, "VI7\r");
		Send(txt, strlen(txt)); 
		
		Recv((char*)vm[im].data[0], &nc);
		if (nc > 0)
		{			
			if (nc >= 256) {
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
					vm[im].upgrading = false;
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
				Send((char*)fwdata, BLK_FWSIZE+0x10); // timeout; send again
				wFW = 0;
			}
		}
	}

	

	CDialog::OnTimer(nIDEvent);
}


void CGPM_VibrationDlg::OnBnClickedReset()
{
	int im = 0;
	if (vm[im].Connected)
	{
		Send("rs0\r", 4);
		Sleep(100);
		OnBnClickedConnect();
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
	if ((vm[im].Connected==2) && (vm[im].gpmSock))
	{
		shutdown(vm[im].gpmSock,SD_SEND);
		Sleep(100);
		closesocket(vm[im].gpmSock);
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

	
	CDialog::OnDestroy();
}

void CGPM_VibrationDlg::OnBnClickedUpdate()
{
	char key = 27;
	char szFile[500];
	char msg[100];
	int nc;
	int im = 0;

	if (!vm[im].Connected)
		return;

	if (vm[im].upgrading == false)
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
		errno_t nErr = fopen_s(&fw, ofn.lpstrFile, "rb");
		nFW = fread(fwfile, 1, sizeof(fwfile), fw);
		nFW = (nFW / ERASE_BLOCK_SIZE + 1) * ERASE_BLOCK_SIZE;
		fclose(fw);
		crc = 0;
		for (int i = 0; i < nFW; i++) crc += fwfile[i];
		fw = NULL;
		iFW = 0;
		wFW = 0;
		KillTimer(1);
		vm[im].upgrading = true;
		if (!vm[im].ambootloader)  // make sure we're not connected to the bootloader
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
					Sleep(100);
					nc = 100;
					while (nc == 100)
						Recv(msg, &nc);
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
					vm[im].upgrading = false;
					SetTimer(1, 1000 / pRate->GetPos(), NULL);
				}
			}
		}
		
	}
	else {
		KillTimer(4);
		SetTimer(1, 1000 / pRate->GetPos(), NULL);
		vm[im].upgrading = false;
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

	CString msg;
	GetDlgItemText(IDC_FREQ,msg);
	msg = "SP81," + msg + "\r";
	Send(msg.GetBuffer(), msg.GetLength());
}


void CGPM_VibrationDlg::OnEnChangeGain()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString msg;
	GetDlgItemText(IDC_FREQ, msg);
	msg = "SP80," + msg + "\r";
	Send(msg.GetBuffer(), msg.GetLength());
}


void CGPM_VibrationDlg::OnCbnSelchangeAxis()
{
	// TODO: Add your control notification handler code here
	CComboBox* p = (CComboBox*)GetDlgItem(IDC_AXIS);
	int i = p->GetCurSel();
	char msg[] = "LD6,6\r";
	msg[4] += i;
	Send(msg, 6);
}


void CGPM_VibrationDlg::OnCbnSelchangeScale()
{
	// TODO: Add your control notification handler code here
	CComboBox* p = (CComboBox*)GetDlgItem(IDC_SCALE);
	int i = p->GetCurSel();
	char msg[] = "SP82,6\r";
	msg[5] += i;
	Send(msg, 7);
}


void CGPM_VibrationDlg::OnCbnSelchangeFftwndfcn()
{
	// TODO: Add your control notification handler code here
	CComboBox* p = (CComboBox*)GetDlgItem(IDC_FFTWNDFCN);
	int i = p->GetCurSel()+1;
	char msg[10];
	sprintf_s(msg, 10, "SP83,%i\r", i);
	Send(msg, strlen(msg));
}


void CGPM_VibrationDlg::OnCbnSelchangeDwnsamples()
{
	// TODO: Add your control notification handler code here
	CComboBox* p = (CComboBox*)GetDlgItem(IDC_SCALE);
	int i = p->GetCurSel();
	if (i == 0) Send("SP85,0\r", 7);
	else Send("SP85,1\r", 7);
}


void CGPM_VibrationDlg::OnCbnSelchangeSamplesize()
{
	// TODO: Add your control notification handler code here
	CComboBox* p = (CComboBox*)GetDlgItem(IDC_SCALE);
	int i = p->GetCurSel();
	switch (i) {
	case 0: Send("SP84,2024\r", 10); break;
	case 1: Send("SP84,1024\r", 10); break;
	case 2: Send("SP84,512\r", 9); break;
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
		fread(jwt, 1000, 1, fid);
		fclose(fid);
	}
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
	sprintf_s(ccmd, 2000, "curl -X POST -H 'authorization: Bearer %s' -H 'content-type: application/json' --data '{\"ascii_data\": \"my first data\"}' -H 'cache-control: no-cache' 'https://cloudiotdevice.googleapis.com/v1/projects/%s/locations/%s/registries/%s/devices/%s:publishEvent'",
		jwt, project.GetBuffer(), "global", "MAGNA_GPM_DEV_EDZKO", device.GetBuffer());
	WinExec(ccmd, SW_HIDE);
}
