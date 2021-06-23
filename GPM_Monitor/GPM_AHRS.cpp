// GPM_AHRS.cpp : implementation file
//

#include "stdafx.h"
#include "GPM_Monitor.h"
#include "GPM_AHRS.h"

// CGPM_AHRS dialog

IMPLEMENT_DYNAMIC(CGPM_AHRS, CDialog)

CGPM_AHRS::CGPM_AHRS(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_AHRS, pParent)
{
	myUpdate = false;
	firstUpdate = false;
	roll = 0.0; pitch = 0.0;	yaw = 0.0;
	roll1 = 0.0; pitch1 = 0.0; yaw1 = 0.0;
	roll2 = 0.0; pitch2 = 0.0; yaw2 = 0.0;
	_time = 0.0;
	_vpp = 0.0;
	Kf = 1.0;

	tdFont.CreateFont(24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, "Cambria");
	bg.CreateSolidBrush(RGB(190, 190, 190));
	pDb = pParent;
	dc = new CDC();
}

CGPM_AHRS::~CGPM_AHRS()
{
	delete dc;
}

void CGPM_AHRS::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGPM_AHRS, CDialog)
	ON_WM_SIZING()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CGPM_AHRS message handlers


void CGPM_AHRS::OnSizing(UINT fwSide, LPRECT pRect)
{
	ResizeAhrs(pRect);

	CDialog::OnSizing(fwSide, pRect);
}
void CGPM_AHRS::ResizeAhrs(RECT* rc)
{
	RECT rcc;

	MoveWindow(rc);
	rc->bottom -= 20;

	CWnd* pCtrl = GetDlgItem(IDC_COMPASS_FRAME);
	if (pCtrl == NULL) return;

	rcc.right = rc->right / 2 - 5;
	rcc.left = 10;
	rcc.top = 10;
	rcc.bottom = rc->bottom - 80;
	pCtrl->MoveWindow(&rcc);

	pCtrl = GetDlgItem(IDC_INS_FRAME);
	if (pCtrl == NULL) return;
	rcc.left = rc->right / 2 + 5;
	rcc.right = rc->right - 20;
	rcc.bottom = rc->bottom - 80;
	rcc.top = 10;
	pCtrl->MoveWindow(&rcc);

	pCtrl = GetDlgItem(IDC_POSITION_FRAME);
	if (pCtrl == NULL) return;
	rcc.left = 10;
	rcc.right = rc->right - 20;
	rcc.bottom = rc->bottom - 30;
	rcc.top = rc->bottom - 80;
	pCtrl->MoveWindow(&rcc);

	pCtrl = GetDlgItem(IDC_POSITION);
	if (pCtrl == NULL) return;
	rcc.left = 15;
	rcc.right = rc->right - 25;
	rcc.bottom = rc->bottom - 40;
	rcc.top = rc->bottom - 65;
	pCtrl->MoveWindow(&rcc);
}

void CGPM_AHRS::DrawCompass(CDC* dc1, int CID)
{
	CBitmap bmp;
	CPoint pnt[5];
	CWnd* pCID = GetDlgItem(CID);
	RECT cwnd, dwnd;
	pCID->GetWindowRect(&cwnd);// control window
	GetWindowRect(&dwnd);// dialog box window
	cwnd.top -= dwnd.top;
	cwnd.bottom -= dwnd.top;
	cwnd.left -= dwnd.left;
	cwnd.right -= dwnd.left;

	//dc->SelectClipRgn(NULL);
	RECT tb; // instrument drawing area
	tb.left = cwnd.left + 10;
	tb.right = cwnd.right - 15;
	tb.top = cwnd.top + 20;
	tb.bottom = cwnd.bottom - 10;

	CRgn crgn; // setup clipping region to avoid drawing outside instrument
	crgn.CreateRectRgnIndirect(&tb);
	dc1->SelectClipRgn(&crgn);

	// Create buffered drawing plane
	dc->CreateCompatibleDC(dc1);
	bmp.CreateBitmap(tb.right, tb.bottom, 1, 32, NULL);
	dc->SelectObject(&bmp);

	dc->FillSolidRect(&tb, RGB(0, 0, 0));
	CPen art; // circular axis on compass
	art.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	dc->SelectObject(&art);
	double r1, r = min((tb.right - tb.left) / 2.3, (tb.bottom - tb.top) / 2.3);
	int gndx = (tb.right + tb.left) / 2;
	int gndy = (tb.bottom + tb.top) / 2;
	for (int i = 0; i < 360; i += 5)
	{
		r1 = r - 5;
		pnt[0].x = gndx + (long)(r * sin(3.1415 / 180 * i));
		pnt[0].y = gndy + (long)(r * cos(3.1415 / 180 * i));
		pnt[1].x = gndx + (long)(r1 * sin(3.1415 / 180 * i));
		pnt[1].y = gndy + (long)(r1 * cos(3.1415 / 180 * i));
		dc->Polyline(pnt, 2);
	}
	dc->SetTextColor(RGB(200, 200, 200));
	dc->SetBkColor(RGB(0, 0, 0));
	RECT txtbox;
	char num[50];
	for (int i = 0; i < 360; i += 30)
	{
		r1 = r - 10;
		pnt[0].x = gndx + (long)(r * sin(3.1415 / 180 * i));
		pnt[0].y = gndy + (long)(r * cos(3.1415 / 180 * i));
		pnt[1].x = gndx + (long)(r1 * sin(3.1415 / 180 * i));
		pnt[1].y = gndy + (long)(r1 * cos(3.1415 / 180 * i));
		dc->Polyline(pnt, 2);

		CFont fnt;
		int j = i + 180;
		fnt.CreateFont(10, 0, j * 10, j * 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
		dc->SelectObject(&fnt);
		txtbox.left = gndx + (long)((r - 20) * sin(3.1415 / 180 * (j - 2)));
		txtbox.right = txtbox.left;
		txtbox.top = gndy + (long)((r - 20) * cos(3.1415 / 180 * (j - 2)));
		txtbox.bottom = txtbox.top;
		sprintf_s(num, 50, "%i", i / 10);
		dc->DrawText(num, (int)strlen(num), &txtbox, DT_NOCLIP);
	}
	for (int i = 0; i < 360; i += 90)
	{
		r += 3;
		r1 = r - 30;
		pnt[0].x = gndx + (long)(r * sin(3.1415 / 180 * i));
		pnt[0].y = gndy + (long)(r * cos(3.1415 / 180 * i));
		pnt[1].x = gndx + (long)(r1 * sin(3.1415 / 180 * i));
		pnt[1].y = gndy + (long)(r1 * cos(3.1415 / 180 * i));
		dc->Polyline(pnt, 2);
	}

	// draw arrow
	r -= 25;
	CPen arw;
	arw.CreatePen(PS_SOLID, 3, RGB(0, 200, 0));
	dc->SelectObject(&arw);
	pnt[0].x = gndx + (long)(r * sin(yaw));
	pnt[0].y = gndy + (long)(r * cos(yaw));
	pnt[1].x = gndx - (long)(r * sin(yaw));
	pnt[1].y = gndy - (long)(r * cos(yaw));
	pnt[2].x = gndx - (long)((r - 10) * sin(yaw - 0.1));
	pnt[2].y = gndy - (long)((r - 10) * cos(yaw - 0.1));
	pnt[3].x = gndx - (long)((r - 10) * sin(yaw + 0.1));
	pnt[3].y = gndy - (long)((r - 10) * cos(yaw + 0.1));
	pnt[4].x = gndx - (long)(r * sin(yaw));
	pnt[4].y = gndy - (long)(r * cos(yaw));
	dc->Polygon(pnt, 5);
	CPen cir;
	cir.CreatePen(PS_SOLID, 1, RGB(0, 150, 0));
	dc->SelectObject(&cir);
	CBrush cirb;
	cirb.CreateSolidBrush(RGB(0, 150, 0));
	dc->SelectObject(&cirb);
	RECT cdot;
	for (int i = -6; i <= 6; i += 3)
	{
		cdot.left = gndx - 2; cdot.right = gndx + 2;
		cdot.top = gndy - (long)(0.1 * i * r) - 2; cdot.bottom = cdot.top + 4;
		dc->Ellipse(&cdot);
	}
	for (int i = -6; i <= 6; i += 3)
	{
		cdot.left = gndx - (long)(0.1 * i * r) - 2; cdot.right = cdot.left + 4;
		cdot.top = gndy - 2; cdot.bottom = cdot.top + 4;
		dc->Ellipse(&cdot);
	}
	CFont dcal;
	dcal.CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	dc->SelectObject(&dcal);
	dc->SetTextColor(RGB(180, 0, 0));
	txtbox.left = tb.left + 10; txtbox.right = txtbox.left;
	txtbox.top = tb.top + 3; txtbox.bottom = txtbox.top;
	dc->DrawText("GMT", 3, &txtbox, DT_NOCLIP);

	txtbox.left = tb.right - 37; txtbox.right = txtbox.left;
	txtbox.top = tb.top + 3; txtbox.bottom = txtbox.top;
	dc->DrawText("YAW", 3, &txtbox, DT_NOCLIP);

	dc->SetTextColor(RGB(0, 100, 0));
	txtbox.left = tb.left + 10; txtbox.right = txtbox.left;
	txtbox.top = tb.top + 15; txtbox.bottom = txtbox.top;
	sprintf_s(num, 50, "%03i", (int)(_time * 10));
	dc->DrawText(num, (int)strlen(num), &txtbox, DT_NOCLIP);

	dc->SetTextColor(RGB(200, 200, 200));
	txtbox.left = tb.right - 37; txtbox.right = txtbox.left;
	txtbox.top = tb.top + 15; txtbox.bottom = txtbox.top;
	sprintf_s(num, 50, "%03i", (int)(yaw * 180 / 3.1415));
	dc->DrawText(num, (int)strlen(num), &txtbox, DT_NOCLIP);

	dc->SetTextColor(RGB(200, 200, 200));
	txtbox.left = tb.right - 37; txtbox.right = txtbox.left;
	txtbox.top = tb.bottom - 28; txtbox.bottom = txtbox.top;
	dc->DrawText("BAT", 4, &txtbox, DT_NOCLIP);
	txtbox.left = tb.right - 37; txtbox.right = txtbox.left;
	txtbox.top = tb.bottom - 15; txtbox.bottom = txtbox.top;
	if (_vpp >= 10.0) sprintf_s(num, 50, "%4.1f", _vpp); else sprintf_s(num, 50, "%4.2f", _vpp);
	dc->DrawText(num, (int)strlen(num), &txtbox, DT_NOCLIP);

	txtbox.left = tb.left + 10; txtbox.right = txtbox.left;
	txtbox.top = tb.bottom - 28; txtbox.bottom = txtbox.top;
	dc->DrawText("TIG", 3, &txtbox, DT_NOCLIP);
	txtbox.left = tb.left + 10; txtbox.right = txtbox.left;
	txtbox.top = tb.bottom - 15; txtbox.bottom = txtbox.top;
	dc->DrawText("0:00", 4, &txtbox, DT_NOCLIP);

	dc1->BitBlt(0, 0, tb.right, tb.bottom, dc, 0, 0, SRCCOPY);
	bmp.DeleteObject();
	dc->DeleteDC();

};

void CGPM_AHRS::DrawOrientation(CDC* dc1, int CID)
{
	CBitmap bmp;
	CPoint pnt[4], gndline[2];
	CWnd* pCID = GetDlgItem(CID);
	RECT cwnd, dwnd;
	pCID->GetWindowRect(&cwnd); // control window
	GetWindowRect(&dwnd); // dialog box window
	cwnd.top -= dwnd.top;
	cwnd.bottom -= dwnd.top;
	cwnd.left -= dwnd.left;
	cwnd.right -= dwnd.left;

	RECT tb; // instrument drawing area
	tb.left = cwnd.left + 10;
	tb.right = cwnd.right - 15;
	tb.top = cwnd.top + 20;
	tb.bottom = cwnd.bottom - 10;

	CRgn crgn; // setup clipping region to avoid drawing outside instrument
	crgn.CreateRectRgn(tb.left, tb.top, tb.right, tb.bottom);
	dc1->SelectClipRgn(&crgn, RGN_COPY);

	// Create buffered drawing plane
	dc->CreateCompatibleDC(dc1);
	bmp.CreateBitmap(tb.right, tb.bottom, 1, 32, NULL);
	dc->SelectObject(&bmp);

	dc->FillSolidRect(&tb, RGB(0, 0, 150));
	// ground plane
	int gndx0 = (tb.right + tb.left) / 2;
	int gndy0 = (tb.bottom + tb.top) / 2;
	int gndx = gndx0 - (int)(gndy0 * (sin(pitch)) * sin(roll));
	int gndy = (int)(gndy0 * (1.0 + sin(pitch)));
	int gndw = tb.right - tb.left;
	gndline[0].x = gndx - (long)(gndw * cos(roll));
	gndline[0].y = gndy - (long)(gndw * sin(roll));
	gndline[1].x = gndx + (long)(gndw * cos(roll));
	gndline[1].y = gndy + (long)(gndw * sin(roll));
	pnt[0] = gndline[0];
	pnt[1] = gndline[1];
	pnt[2].x = pnt[1].x;
	pnt[2].y = pnt[1].y + 2 * (tb.bottom - tb.top);
	pnt[3].x = pnt[0].x;
	pnt[3].y = pnt[0].y + 2 * (tb.bottom - tb.top);

	CBrush gndb;
	gndb.CreateSolidBrush(RGB(0x95, 0x4A, 0x4A));
	dc->SelectObject(gndb);
	CPen gndp;
	gndp.CreatePen(PS_NULL, 0, RGB(0x95, 0x4A, 0x4A));
	dc->SelectObject(&gndp);
	dc->Polygon(pnt, 4);

	CPen art, vzr; // axis on ground plane
	art.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	dc->SelectObject(&art);
	pnt[0].x = gndline[0].x; pnt[0].y = gndline[0].y;
	pnt[1].x = gndline[1].x; pnt[1].y = gndline[1].y;
	dc->Polyline(pnt, 2);

	for (int i = 10; i < gndw; i += 20)
	{
		pnt[0].x = gndx + (long)(i * cos(roll));
		pnt[0].y = gndy + (long)(i * sin(roll));
		pnt[1].x = gndx + (long)(i * cos(roll) + 2.0 * sin(roll));
		pnt[1].y = gndy + (long)(i * sin(roll) - 2.0 * cos(roll));
		dc->Polyline(pnt, 2);
		pnt[0].x = gndx - (long)(i * cos(roll));
		pnt[0].y = gndy - (long)(i * sin(roll));
		pnt[1].x = gndx - (long)(i * cos(roll) - 2.0 * sin(roll));
		pnt[1].y = gndy - (long)(i * sin(roll) + 2.0 * cos(roll));
		dc->Polyline(pnt, 2);
	}
	for (int i = 0; i < gndw; i += 20)
	{
		pnt[0].x = gndx + (long)((double)i * cos(roll));
		pnt[0].y = gndy + (long)((double)i * sin(roll));
		pnt[1].x = gndx + (long)((double)i * cos(roll) + 5.0 * sin(roll));
		pnt[1].y = gndy + (long)((double)i * sin(roll) - 5.0 * cos(roll));
		dc->Polyline(pnt, 2);
		pnt[0].x = gndx - (long)((double)i * cos(roll));
		pnt[0].y = gndy - (long)((double)i * sin(roll));
		pnt[1].x = gndx - (long)((double)i * cos(roll) - 5.0 * sin(roll));
		pnt[1].y = gndy - (long)((double)i * sin(roll) + 5.0 * cos(roll));
		dc->Polyline(pnt, 2);
	}
	// print units on ground plane axis
	CFont fnt;
	fnt.CreateFont(10, 0, (int)(roll * 1800.0 / -3.14), (int)(roll * 1800.0 / -3.14), 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	//fnt.CreateFont(12,0,0,0,0,0,0,0,0,0,0,0,0,"Arial");
	dc->SetTextColor(RGB(200, 200, 200));
	dc->SetBkColor(RGB(00, 0, 150));
	dc->SelectObject(fnt);

	RECT txtbox;
	txtbox.left = gndx + (long)(-3.0 * cos(roll) + 18.0 * sin(roll));
	txtbox.right = txtbox.left;
	txtbox.top = gndy + (long)(-3.0 * sin(roll) - 18.0 * cos(roll));
	txtbox.bottom = txtbox.top;
	dc->DrawText("0", 1, &txtbox, DT_NOCLIP);
	txtbox.left = gndx + (long)(50.0 * cos(roll) + 18.0 * sin(roll));
	txtbox.right = txtbox.left;
	txtbox.top = gndy + (long)(50.0 * sin(roll) - 18.0 * cos(roll));
	txtbox.bottom = txtbox.top;
	dc->DrawText("-30", 3, &txtbox, DT_NOCLIP);
	txtbox.left = gndx + (long)(-65.0 * cos(roll) + 18.0 * sin(roll));
	txtbox.right = txtbox.left;
	txtbox.top = gndy + (long)(-65.0 * sin(roll) - 18.0 * cos(roll));
	txtbox.bottom = txtbox.top;
	dc->DrawText("30", 2, &txtbox, DT_NOCLIP);

	// roll axis on sky
	for (int i = -90; i <= 90; i += 15)
	{
		pnt[0].x = gndx0 - (long)(80 * sin(3.1415 / 180 * i));
		pnt[0].y = gndy0 - (long)(80 * cos(3.1415 / 180 * i));
		if ((i == -45) || (i == 45))
		{
			pnt[1].x = gndx0 - (long)(70 * sin(3.1415 / 180 * i));
			pnt[1].y = gndy0 - (long)(70 * cos(3.1415 / 180 * i));
		}
		else
		{
			pnt[1].x = gndx0 - (long)(75 * sin(3.1415 / 180 * i));
			pnt[1].y = gndy0 - (long)(75 * cos(3.1415 / 180 * i));
		}
		dc->Polyline(pnt, 2);
	}
	// roll angle indicator
	pnt[0].x = gndx0; pnt[0].y = gndy0 - 75;
	pnt[1].x = gndx0 - 5; pnt[1].y = gndy0 - 80;
	pnt[2].x = gndx0 + 5; pnt[2].y = gndy0 - 80;
	dc->Polygon(pnt, 3);
	pnt[0].x = gndx0 + (long)(70.0 * sin(roll));
	pnt[0].y = gndy0 - (long)(70.0 * cos(roll));
	pnt[1].x = gndx0 + (long)(65.0 * sin(roll + 0.1));
	pnt[1].y = gndy0 - (long)(65.0 * cos(roll + 0.1));
	pnt[2].x = gndx0 + (long)(65.0 * sin(roll - 0.1));
	pnt[2].y = gndy0 - (long)(65.0 * cos(roll - 0.1));
	dc->Polygon(pnt, 3);

	// visor pitch scale
	CPen pax;
	pax.CreatePen(PS_SOLID, 1, RGB(180, 180, 180));
	dc->SelectObject(&pax);
	for (int i = -60; i <= 60; i += 20)
	{
		if (i != 0) /*&&((i+180.0/3.1415*pitch)<=60)&&((i+180.0/3.1415*pitch)>=-60))*/
		{
			if ((i == -40) || (i == 40)) {
				pnt[0].x = gndx - (long)(30.0 * cos(roll) + (double)i * sin(roll));
				pnt[0].y = gndy + (long)((double)i * cos(roll) - 30.0 * sin(roll));
				pnt[1].x = gndx + (long)(30.0 * cos(roll) - (double)i * sin(roll));
				pnt[1].y = gndy + (long)((double)i * cos(roll) + 30.0 * sin(roll));

				if (i < 0) dc->SetBkColor(RGB(00, 0, 150));
				else dc->SetBkColor(RGB(0x95, 0x4A, 0x4A));
				txtbox.left = gndx - (long)(45.0 * cos(roll) + (double)(i - 7) * sin(roll));
				txtbox.right = txtbox.left;
				txtbox.top = gndy + (long)((double)(i - 7) * cos(roll) - 45.0 * sin(roll));
				txtbox.bottom = txtbox.top;
				dc->DrawText("18", 2, &txtbox, DT_NOCLIP);
				txtbox.left = gndx + (long)(32.0 * cos(roll) - (double)(i - 7) * sin(roll));
				txtbox.right = txtbox.left;
				txtbox.top = gndy + (long)((double)(i - 7) * cos(roll) + 32.0 * sin(roll));
				txtbox.bottom = txtbox.top;
				dc->DrawText("18", 2, &txtbox, DT_NOCLIP);

			}
			else {
				pnt[0].x = gndx - (long)(20.0 * cos(roll) + (double)i * sin(roll));
				pnt[0].y = gndy + (long)((double)i * cos(roll) - 20.0 * sin(roll));
				pnt[1].x = gndx + (long)(20.0 * cos(roll) - (double)i * sin(roll));
				pnt[1].y = gndy + (long)((double)i * cos(roll) + 20.0 * sin(roll));
			}
			dc->Polyline(pnt, 2);
		}
	}
	// green vizor handles
	vzr.CreatePen(PS_SOLID, 3, RGB(0, 200, 0));
	dc->SelectObject(&vzr);
	pnt[0].x = gndx0 - 50; pnt[0].y = gndy0 + 15;
	pnt[1].x = gndx0 - 50; pnt[1].y = gndy0;
	pnt[2].x = gndx0 - 100; pnt[2].y = gndy0;
	dc->Polyline(pnt, 3);
	pnt[0].x = gndx0 + 50; pnt[0].y = gndy0 + 15;
	pnt[1].x = gndx0 + 50; pnt[1].y = gndy0;
	pnt[2].x = gndx0 + 100; pnt[2].y = gndy0;
	dc->Polyline(pnt, 3);

	// red vizor focal point indicator
	CPen fcp;
	fcp.CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	dc->SelectObject(&fcp);
	pnt[0].x = gndx0 - 12; pnt[0].y = gndy0 - 5;
	pnt[1].x = gndx0 - 12; pnt[1].y = gndy0 + 5;
	pnt[2].x = gndx0 - 5; pnt[2].y = gndy0;
	dc->Polygon(pnt, 3);
	pnt[0].x = gndx0 + 12; pnt[0].y = gndy0 - 5;
	pnt[1].x = gndx0 + 12; pnt[1].y = gndy0 + 5;
	pnt[2].x = gndx0 + 5; pnt[2].y = gndy0;
	dc->Polygon(pnt, 3);

	dc1->BitBlt(0, 0, tb.right, tb.bottom, dc, 0, 0, SRCCOPY);
	bmp.DeleteObject();
	dc->DeleteDC();
}

void CGPM_AHRS::Update(void) {

}

void CGPM_AHRS::UpdateFilter(void)
{
	if (!(::IsWindow(m_hWnd))) return;

	if (roll1 - roll2 > 3.14) roll2 += 6.283; else if (roll2 - roll1 > 3.14) roll2 -= 6.283;
	if (pitch1 - pitch2 > 3.14) pitch2 += 6.283; else if (pitch2 - pitch1 > 3.14) pitch2 -= 6.283;
	if (yaw1 - yaw2 > 3.14) yaw2 += 6.283; else if (yaw2 - yaw1 > 3.14) yaw2 -= 6.283;

	roll2 = (1.0 - Kf) * roll2 + Kf * roll1;
	pitch2 = (1.0 - Kf) * pitch2 + Kf * pitch1;
	yaw2 = (1.0 - Kf) * yaw2 + Kf * yaw1;

	if (roll - roll2 > 3.14) roll -= 6.283; else if (roll2 - roll > 3.14) roll += 6.283;
	if (pitch - pitch2 > 3.14) pitch -= 6.283; else if (pitch2 - pitch > 3.14) pitch += 6.283;
	if (yaw - yaw2 > 3.14) yaw -= 6.283; else if (yaw2 - yaw > 3.14) yaw += 6.283;

	roll = (1.0 - Kf) * roll + Kf * roll2;
	pitch = (1.0 - Kf) * pitch + Kf * pitch2;
	yaw = (1.0 - Kf) * yaw + Kf * yaw2;

	RECT crc, wrc;
	CWnd* pCID = GetDlgItem(IDC_COMPASS_FRAME);
	pCID->GetWindowRect(&crc);
	GetWindowRect(&wrc);
	crc.left -= wrc.left; crc.right -= wrc.left;
	crc.top -= wrc.top; crc.bottom -= wrc.top;
	crc.left += 10; crc.right -= 15; crc.bottom -= 10;
	InvalidateRect(&crc);

	pCID = GetDlgItem(IDC_INS_FRAME);
	pCID->GetWindowRect(&crc);
	GetWindowRect(&wrc);
	crc.left -= wrc.left; crc.right -= wrc.left;
	crc.top -= wrc.top; crc.bottom -= wrc.top;
	crc.left += 10; crc.right -= 15; crc.bottom -= 10;
	InvalidateRect(&crc);

	myUpdate = true;
	Kf = 0.8;

	UpdateWindow();
}



void CGPM_AHRS::OnPaint()
{
	CPaintDC pdc(this); 

	if (firstUpdate)
	{
		firstUpdate = false;
	}
	DrawCompass(&pdc, IDC_COMPASS_FRAME);
	DrawOrientation(&pdc, IDC_INS_FRAME);

	CDialog::OnPaint();

	myUpdate = false;
}


BOOL CGPM_AHRS::OnEraseBkgnd(CDC* pDC)
{
	if (myUpdate)
	{
		//TRACE0("No Erase Backgnd.\r\n");
		return FALSE;
	}
	else {
		//TRACE0("Yes Erase Backgnd.\r\n");
		return CDialog::OnEraseBkgnd(pDC);
	}

}
