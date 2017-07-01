// VideoLocalizationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VideoLocalization.h"
#include "VideoLocalizationDlg.h"
#include "afxdialogex.h"
#include "..\Camera\CameraManaging.h"

//User global functions defination

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


// CVideoLocalizationDlg dialog



CVideoLocalizationDlg::CVideoLocalizationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VIDEOLOCALIZATION_DIALOG, pParent), m_FileReader{L"Config.txt"}, m_pSPoint {static_cast<SPoint*>(calloc(1, sizeof SPoint))},
	m_LocationAnalyser{ m_FileReader }, m_Receiver{ m_LocationAnalyser }, m_FigureInput{ [&](CLocAna* pPar) {while (true) { m_Receiver.Receive(*pPar); } }, &m_LocationAnalyser },
	m_ProcessData{ [&](void) {while (true) { m_Receiver.Process(); } } }, m_MainViewImage{}, m_Figure{ m_FileReader }, m_CameraManager{},m_InfoFusion{}
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//User peripheral device initialization
	m_CameraManager.InitialResources();

	//User peripheral device initialization

}

void CVideoLocalizationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOCATION_POINTS, m_location_axis);
	DDX_Control(pDX, IDC_View1, m_MainViewControl);
	DDX_Control(pDX, IDC_View3, m_SideViewControl);
}

BEGIN_MESSAGE_MAP(CVideoLocalizationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_STN_CLICKED(IDC_LOCATION_POINTS, &CVideoLocalizationDlg::OnStnClickedLocationPoints)
//	ON_BN_CLICKED(IDC_DRAW, &CVideoLocalizationDlg::OnBnClickedDraw)
//	ON_ACN_START(IDC_ANIMATE1, &CVideoLocalizationDlg::OnAcnStartAnimate1)
	ON_WM_TIMER()
//	ON_STN_CLICKED(IDC_View1, &CVideoLocalizationDlg::OnStnClickedView1)
END_MESSAGE_MAP()


// CVideoLocalizationDlg message handlers

BOOL CVideoLocalizationDlg::OnInitDialog()
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

	// TODO: Add extra initialization here
	SetTimer(1, 59, NULL);

	m_CameraManager.StartCapture();

	/*Test code*/
	static auto DC{ m_location_axis.GetDC() };
	static auto Rect{ &(m_Figure.GetPlotRect()) };
	m_location_axis.GetClientRect(Rect);//Firstly, use the picture control to get the client rect. 
	m_Figure.SelectObject(DC);//Secondly, get the picture control's dc, then give it to the figure class to draw.

	static auto SomeHdc{ m_MainViewControl.GetDC()->GetSafeHdc() };
	m_MainViewImage.SetHDC(SomeHdc);
	static CRect SomeRect{};
	m_MainViewControl.GetClientRect(&SomeRect);
	m_MainViewImage.SetRect(&SomeRect);

	static auto SomeHdc1{ m_SideViewControl.GetDC()->GetSafeHdc() };
	m_SideViewImage.SetHDC(SomeHdc1);
	static CRect SomeRect1{};
	m_SideViewControl.GetClientRect(&SomeRect1);
	m_SideViewImage.SetRect(&SomeRect1);
	/*Test code*/

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVideoLocalizationDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CVideoLocalizationDlg::OnPaint()
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
HCURSOR CVideoLocalizationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVideoLocalizationDlg::OnStnClickedLocationPoints()
{
	// TODO: Add your control notification handler code here
}

void CVideoLocalizationDlg::OnAcnStartAnimate1()
{
	// TODO: Add your control notification handler code here
}

size_t AnalyseCount{};
void CVideoLocalizationDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	m_InfoFusion.SetJudgeArgs(m_Figure.GetPlotRect(), m_FileReader);
	m_InfoFusion.SetMapPointer(m_CameraManager.GetImages());

	auto CurrentPoint{ m_Receiver.Dequeue() };

	/*Test codes*/
	m_Figure.ClearPoints();
	m_Figure.Plot(CurrentPoint);

	m_CameraManager.RetrieveImage();

	auto MainImage{ m_InfoFusion.GetMainViewMat(CurrentPoint) };
	m_MainViewImage.ShowImage(MainImage);

	auto SideImage{ m_InfoFusion.GetSideViewMats(FIRST_QUADRANT_CAM_ID) };
	m_SideViewImage.ShowImage(SideImage);
	/*Test Codes*/
	CDialogEx::OnTimer(nIDEvent);
}
