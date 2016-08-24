
// MagicMirrorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MagicMirror.h"
#include "MagicMirrorDlg.h"
#include "afxdialogex.h"

#include <vector>
#include "rgbquad_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMagicMirrorDlg dialog



CMagicMirrorDlg::CMagicMirrorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MAGICMIRROR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMagicMirrorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMagicMirrorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMagicMirrorDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMagicMirrorDlg::OnBnClickedButton2)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CMagicMirrorDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON3, &CMagicMirrorDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON5, &CMagicMirrorDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CMagicMirrorDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CMagicMirrorDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CMagicMirrorDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON4, &CMagicMirrorDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON9, &CMagicMirrorDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON12, &CMagicMirrorDlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CMagicMirrorDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON14, &CMagicMirrorDlg::OnBnClickedButton14)
END_MESSAGE_MAP()


// CMagicMirrorDlg message handlers

BOOL CMagicMirrorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	CRect rect;
	GetDlgItem(IDC_MAINDISPLAY)->GetWindowRect(rect);
	ScreenToClient(rect);
	m_oglWindow.oglCreate(rect, this);
	m_oglWindow.m_unpTimer = m_oglWindow.SetTimer(1, 33, 0);

	int nWidth = 2048;
	int nHeight = 1024;

	std::vector<RGBQUAD> img(nWidth * nHeight);
	for (int i = 0; i < nWidth * nHeight; ++i) {
		img[i].rgbBlue = 0xff;
		img[i].rgbGreen = 0;
		img[i].rgbRed = 0;
		img[i].rgbReserved = 0xff;
	}

	m_oglWindow.oglSetTexture(img.data(), nWidth, nHeight);
	m_oglWindow.oglSetKinect(&m_kinectManager);

	// sequence choosing combo box
	m_pSequenceComboBox = (CComboBox*)GetDlgItem(IDC_COMBO1);
	m_pSequenceComboBox->ResetContent();
	/*m_pSequenceComboBox->AddString(L"Test 1");
	m_pSequenceComboBox->AddString(L"Test 2");
	m_pSequenceComboBox->AddString(L"Test 3");*/
	m_pSequenceComboBox->AddString(L"Mirror 1");
	m_pSequenceComboBox->AddString(L"Mirror 2");
	m_pSequenceComboBox->AddString(L"Mirror 3");
	m_pSequenceComboBox->AddString(L"Mirror 4");
	m_pSequenceComboBox->AddString(L"Video 1");
	m_pSequenceComboBox->AddString(L"Video 2");
	m_pSequenceComboBox->AddString(L"Video 3");
	m_pSequenceComboBox->AddString(L"Video 4");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMagicMirrorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMagicMirrorDlg::OnPaint()
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
HCURSOR CMagicMirrorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMagicMirrorDlg::OnBnClickedButton1()
{
	m_oglWindow.Reset();
}


void CMagicMirrorDlg::OnBnClickedButton2()
{
	m_oglWindow.ToggleVP();
}


void CMagicMirrorDlg::OnCbnSelchangeCombo1()
{
	int cur_sel = m_pSequenceComboBox->GetCurSel();
	CString sel_name;
	m_pSequenceComboBox->GetLBText(cur_sel, sel_name);
	CT2CA pszConvertedAnsiString(sel_name);
	std::string strStd(pszConvertedAnsiString);
	m_oglWindow.LoadSequence(strStd);
}


void CMagicMirrorDlg::OnBnClickedButton3()
{
	m_oglWindow.Pause();
}


void CMagicMirrorDlg::OnBnClickedButton5()
{
	m_oglWindow.Record("");
}


void CMagicMirrorDlg::OnBnClickedButton6()
{
	m_oglWindow.Record("pre");
}


void CMagicMirrorDlg::OnBnClickedButton7()
{
	m_oglWindow.Record("v");
}


void CMagicMirrorDlg::OnBnClickedButton8()
{
	m_oglWindow.Record("m");
}


void CMagicMirrorDlg::OnBnClickedButton4()
{
	m_oglWindow.Record("", false);
}


void CMagicMirrorDlg::OnBnClickedButton9()
{
	m_oglWindow.ToggleMirror();
}


void CMagicMirrorDlg::OnBnClickedButton12()
{
	m_oglWindow.ToggleMesh();
}


void CMagicMirrorDlg::OnBnClickedButton13()
{
	m_oglWindow.ToggleTexture();
}


void CMagicMirrorDlg::OnBnClickedButton14()
{
	m_oglWindow.ToggleSkeleton();
}
