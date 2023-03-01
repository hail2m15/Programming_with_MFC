
// SieveDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Sieve.h"
#include "SieveDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSieveDlg dialog



CSieveDlg::CSieveDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIEVE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSieveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSieveDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED( IDC_START, OnStart)
	ON_MESSAGE(WM_USER_THREAD_FINISHED, OnThreadFinished)
END_MESSAGE_MAP()


// CSieveDlg message handlers

BOOL CSieveDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	// Set initial value of prime field
	SetDlgItemInt(IDC_PRIME, 2);
	return TRUE;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSieveDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSieveDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//#define NOTHREAD
void CSieveDlg::OnStart()
{	
	int nMax = GetDlgItemInt(IDC_PRIME);
	if (nMax < 2)
	{
		MessageBox(_T("Enter a number greater than 2"));
		GetDlgItem(IDC_PRIME)->SetFocus();
		return;
	}
	SetDlgItemText(IDC_EDIT2, _T(""));
	GetDlgItem(IDC_START)->EnableWindow(FALSE);

#ifdef NOTHREAD
	int count = Sieve(nMax);
	::PostMessage(m_hWnd, WM_USER_THREAD_FINISHED, (WPARAM)count, 0);
	return;
#endif // NOTHREAD


	THREADPARAMS* ptp = new THREADPARAMS;
	ptp->nMax = nMax;
	ptp->hWnd = m_hWnd;
	AfxBeginThread(ThreadFunc, ptp);
	return;
}

LRESULT CSieveDlg::OnThreadFinished(WPARAM wParam, LPARAM lParam)
{
	SetDlgItemInt(IDC_EDIT2, static_cast<int>(wParam));
	GetDlgItem(IDC_START)->EnableWindow(TRUE);
	return 0;
}
//Global function for the thread

UINT ThreadFunc(LPVOID pParam)
{
	THREADPARAMS* ptp = static_cast<THREADPARAMS*>(pParam);
	int nMax = ptp->nMax;
	HWND hWnd = ptp->hWnd;
	delete ptp;

	int count = Sieve(nMax);
	::PostMessage(hWnd, WM_USER_THREAD_FINISHED, (WPARAM)count, 0);
	return 0;
}

// Global function to calculate Number of Prime Numbers.

int Sieve(int nMax)
{
	PBYTE pBuffer = new BYTE[nMax + 1];
	::FillMemory(pBuffer, nMax + 1, 1);
	int nLimit = 2;
	while (nLimit * nLimit < nMax)
		nLimit++;
	for (int i = 2; i <= nLimit; i++) {
		// To slow down calculation to show the working
		::Sleep(100);

		if (pBuffer[i]) {
			for (int k = i + i; k <= nMax; k += i)
				pBuffer[k] = 0;
		}
	}
	int nCount = 0;
	for (int i = 2; i <= nMax; i++)
		if (pBuffer[i])
			nCount++;
	delete[] pBuffer;
	return nCount;
}
