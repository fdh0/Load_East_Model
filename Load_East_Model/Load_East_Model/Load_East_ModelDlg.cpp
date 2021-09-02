
// Load_East_ModelDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Load_East_Model.h"
#include "Load_East_ModelDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


char* img_path= "1.bmp";

CString strImgPath;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CLoadEastModelDlg 对话框



CLoadEastModelDlg::CLoadEastModelDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOAD_EAST_MODEL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLoadEastModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLoadEastModelDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(BTN_LOAD_MODEL, &CLoadEastModelDlg::OnBnClickedLoadModel)

	ON_WM_HSCROLL()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()

	ON_BN_CLICKED(BTN_TEST, &CLoadEastModelDlg::OnBnClickedTest)
END_MESSAGE_MAP()


// CLoadEastModelDlg 消息处理程序

BOOL CLoadEastModelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	// TODO:  在此添加额外的初始化
	CRect m_rtImage;
	GetDlgItem(IDC_STATIC_HALCON_IMG)->GetClientRect(&m_rtImage);//IDC_DISPIMAGE为第一个图像控件的ID号，第二个窗口类似处理。
	HWND hWnd;
	SetCheck("~father");
	hWnd = GetDlgItem(IDC_STATIC_HALCON_IMG)->m_hWnd;
	SetWindowAttr("background_color", "black");
	OpenWindow(m_rtImage.left, m_rtImage.top, m_rtImage.Width(), m_rtImage.Height(), (Hlong)hWnd, "visible", "", &hImgWnd);

	SetDraw(hImgWnd, "margin");
	SetColor(hImgWnd,"red");

	try
	{
		HTuple htFileExists;
		FileExists(img_path, &htFileExists);
		if (!htFileExists[0].I())
		{
			return TRUE;
		}

		ReadImage(&m_hImage, img_path);

		GetImageSize(m_hImage, &m_hWidth, &m_hHeight);
		//设置窗口
		float fImage = m_hWidth.D() / m_hHeight.D();
		float fWindow = (float)m_rtImage.Width() / m_rtImage.Height();
		float Row0 = 0, Col0 = 0, Row1 = m_hHeight - 1, Col1 = m_hWidth - 1;
		if (fWindow > fImage)
		{
			float w = fWindow * m_hHeight;
			Row0 = 0;
			Col0 = -(w - m_hWidth) / 2;
			Row1 = m_hHeight - 1;
			Col1 = m_hWidth + (w - m_hWidth) / 2;
		}
		else
		{
			float h = m_hWidth / fWindow;
			Row0 = -(h - m_hHeight) / 2;
			Col0 = 0;
			Row1 = m_hHeight + (h - m_hHeight) / 2;
			Col1 = m_hWidth - 1;
		}

		m_dDispImagePartRow0 = Row0;
		m_dDispImagePartCol0 = Col0;
		m_dDispImagePartRow1 = Row1;
		m_dDispImagePartCol1 = Col1;

		ShowImage();
	}
	catch(...)
	{
		AfxMessageBox(_T("初始化失败"));
	}
	try
	{
		const char* model_path = "frozen_east_text_detection.pb"; //"..\\x64\\Debug\\frozen_east_text_detection.pb";
		east_model.LoadModel(model_path);
	}
	catch (const std::exception&)
	{
		AfxMessageBox(_T("模型初始化失败"));
	}


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
void CLoadEastModelDlg::ShowImage()
{
	if (hImgWnd != 0)
	{
		SetSystem("flush_graphic", "false");
		ClearWindow(hImgWnd);
		//显示

		try 
		{

			if (m_hImage.IsInitialized())
			{
				SetPart(hImgWnd, m_dDispImagePartRow0, m_dDispImagePartCol0, m_dDispImagePartRow1 - 1, m_dDispImagePartCol1 - 1);
				DispObj(m_hImage, hImgWnd);
			}

			if (hoRectDisp.IsInitialized())
			{
				SetPart(hImgWnd, m_dDispImagePartRow0, m_dDispImagePartCol0, m_dDispImagePartRow1 - 1, m_dDispImagePartCol1 - 1);
				DispObj(hoRectDisp, hImgWnd);
			}
		}
		catch (...)
		{

		}

		SetSystem("flush_graphic", "true");
		HObject emptyObject;
		emptyObject.GenEmptyObj();
		DispObj(emptyObject, hImgWnd);
	}
}

BOOL CLoadEastModelDlg::PreTranslateMessage(MSG* pMsg)
{
	BOOL m_bImgMove;
	CRect m_rPic;
	CPoint point;
	GetCursorPos(&point);
	CWnd * phWnd = GetDlgItem(IDC_STATIC_HALCON_IMG);
	phWnd->GetClientRect(m_rPic);
	phWnd->ClientToScreen(m_rPic);
	point.x = point.x - m_rPic.left;
	point.y = point.y - m_rPic.top;
	phWnd->GetClientRect(m_rPic);
	if (m_rPic.PtInRect(point))    //鼠标移动在窗口内
	{
		CPoint point2;
		point2 = point;
		if (pMsg->message == WM_LBUTTONDOWN)    //鼠标左键单击
		{
			if (5 == m_iCursor)    //图像移动
			{
				m_dXO = point2.x;
				m_dYO = point2.y;
				m_bImgMove = true;
			}
		}
		else if (pMsg->message == WM_LBUTTONUP)
		{
			if (5 == m_iCursor)    //图像移动
			{
				m_bImgMove = false;
				MoveWnd(point2, m_hImage, hImgWnd);
				m_dXO = 0;
				m_dYO = 0;
			}
		}
	}

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return __super::PreTranslateMessage(pMsg);
}

void CLoadEastModelDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	m_iCursor = 5;
	CDialogEx::OnMouseMove(nFlags, point);
}
void CLoadEastModelDlg::MoveWnd(CPoint point, HObject srcImg, HTuple hWindow)
{
	double xOffset = point.x - m_dXO;
	double yOffset = point.y - m_dYO;
	m_dDispImagePartRow0 = m_dDispImagePartRow0 - yOffset;
	m_dDispImagePartCol0 = m_dDispImagePartCol0 - xOffset;
	m_dDispImagePartRow1 = m_dDispImagePartRow1 - yOffset;
	m_dDispImagePartCol1 = m_dDispImagePartCol1 - xOffset;
	ClearWindow(hWindow);

	SetPart(hWindow, m_dDispImagePartRow0, m_dDispImagePartCol0, m_dDispImagePartRow1, m_dDispImagePartCol1);
	//DispObj(srcImg, hWindow);
	ShowImage();
}

BOOL CLoadEastModelDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CRect rtImage;
	GetDlgItem(IDC_STATIC_HALCON_IMG)->GetWindowRect(&rtImage);
	if (rtImage.PtInRect(pt) && m_hImage.IsInitialized())
	{
		Hlong  ImagePtX, ImagePtY;
		Hlong  Row0, Col0, Row1, Col1;
		double Scale = 0.1;
		if (zDelta < 0)
		{
			ImagePtX = m_dDispImagePartCol0 + (pt.x - rtImage.left) / (rtImage.Width() - 1.0)*(m_dDispImagePartCol1 - m_dDispImagePartCol0);
			ImagePtY = m_dDispImagePartRow0 + (pt.y - rtImage.top) / (rtImage.Height() - 1.0)*(m_dDispImagePartRow1 - m_dDispImagePartRow0);
			Row0 = ImagePtY - 1 / (1 - Scale)*(ImagePtY - m_dDispImagePartRow0);
			Row1 = ImagePtY - 1 / (1 - Scale)*(ImagePtY - m_dDispImagePartRow1);
			Col0 = ImagePtX - 1 / (1 - Scale)*(ImagePtX - m_dDispImagePartCol0);
			Col1 = ImagePtX - 1 / (1 - Scale)*(ImagePtX - m_dDispImagePartCol1);
			m_dDispImagePartRow0 = Row0;
			m_dDispImagePartCol0 = Col0;
			m_dDispImagePartRow1 = Row1;
			m_dDispImagePartCol1 = Col1;
		}
		else
		{
			ImagePtX = m_dDispImagePartCol0 + (pt.x - rtImage.left) / (rtImage.Width() - 1.0)*(m_dDispImagePartCol1 - m_dDispImagePartCol0);
			ImagePtY = m_dDispImagePartRow0 + (pt.y - rtImage.top) / (rtImage.Height() - 1.0)*(m_dDispImagePartRow1 - m_dDispImagePartRow0);
			Row0 = ImagePtY - 1 / (1 + Scale)*(ImagePtY - m_dDispImagePartRow0);
			Row1 = ImagePtY - 1 / (1 + Scale)*(ImagePtY - m_dDispImagePartRow1);
			Col0 = ImagePtX - 1 / (1 + Scale)*(ImagePtX - m_dDispImagePartCol0);
			Col1 = ImagePtX - 1 / (1 + Scale)*(ImagePtX - m_dDispImagePartCol1);
			m_dDispImagePartRow0 = Row0;
			m_dDispImagePartCol0 = Col0;
			m_dDispImagePartRow1 = Row1;
			m_dDispImagePartCol1 = Col1;
		}
		ShowImage();
	}
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}
void CLoadEastModelDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLoadEastModelDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLoadEastModelDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLoadEastModelDlg::OnBnClickedLoadModel()
{
	// TODO: 在此添加控件通知处理程序代码

	// Load Model
	//const char* img_path = "..\\x64\\Debug\\4.bmp";
	
	float Threshold = 20;
	vector<vector<double>> TextPos;

	char* imgpath = img_path;

	if (strImgPath != "")
	{
		USES_CONVERSION;
		imgpath = T2A(strImgPath);
	}

	auto detect_image_path = imgpath;
	cv::Mat srcImg = cv::imread(detect_image_path);
	
	int or_h = srcImg.rows;
	int or_w = srcImg.cols;

	cv::resize(srcImg, srcImg,cv::Size(640,640));

	double ratio_h = or_h* 1.0 / 640;
	double ratio_w = or_w* 1.0 / 640;

	auto bLoadFlag = east_model.TextDetect(srcImg,Threshold, TextPos);
	if (bLoadFlag)
	{
		AfxMessageBox(_T("文本检测异常"));
		return;
	}

	HObject hoRect;
	HTuple maxT1, maxT2, minT2, minT1;

	int m, n;
	GenEmptyObj(&hoRectDisp);

	for (m = 0; m < TextPos.size(); m++)
	{
		TupleMax2(TextPos[m].at(3), TextPos[m].at(1),&maxT1);
		TupleMin2(TextPos[m].at(3), TextPos[m].at(1), &minT1);
		
		TupleMax2(TextPos[m].at(2), TextPos[m].at(0), &maxT2);
		TupleMin2(TextPos[m].at(2), TextPos[m].at(0), &minT2);

		GenRectangle1(&hoRect, minT1, minT2, maxT1, maxT2);

		ZoomRegion(hoRect, &hoRect, ratio_w, ratio_h);

		ConcatObj(hoRectDisp,hoRect,&hoRectDisp);
		DispObj(hoRect, hImgWnd);
	}
	ShowImage();
	AfxMessageBox(_T("Finished"));
}


void CLoadEastModelDlg::OnBnClickedTest()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFilter[] = _T("图像(*.jpg)|*.jpg;*.jpeg;*.bmp;*.png|所有文件(*.*)|*.*||");
	// 构造打开文件对话框
	CFileDialog fileDlg(TRUE, _T("Image"), NULL, 0, szFilter, this);
	CString strFilePath;
	// 显示打开文件对话框   
	if (IDOK == fileDlg.DoModal())
	{
		// 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里   
		strFilePath = fileDlg.GetPathName();
		strImgPath = (strFilePath);

		USES_CONVERSION;
		char* imgname = T2A(strImgPath);

		CRect m_rtImage;
		GetDlgItem(IDC_STATIC_HALCON_IMG)->GetClientRect(&m_rtImage);//IDC_DISPIMAGE为第一个图像控件的ID号，第二个窗口类似处理。


		ReadImage(&m_hImage, imgname);

		GetImageSize(m_hImage, &m_hWidth, &m_hHeight);
		//设置窗口
		float fImage = m_hWidth.D() / m_hHeight.D();
		float fWindow = (float)m_rtImage.Width() / m_rtImage.Height();
		float Row0 = 0, Col0 = 0, Row1 = m_hHeight - 1, Col1 = m_hWidth - 1;
		if (fWindow > fImage)
		{
			float w = fWindow * m_hHeight;
			Row0 = 0;
			Col0 = -(w - m_hWidth) / 2;
			Row1 = m_hHeight - 1;
			Col1 = m_hWidth + (w - m_hWidth) / 2;
		}
		else
		{
			float h = m_hWidth / fWindow;
			Row0 = -(h - m_hHeight) / 2;
			Col0 = 0;
			Row1 = m_hHeight + (h - m_hHeight) / 2;
			Col1 = m_hWidth - 1;
		}

		m_dDispImagePartRow0 = Row0;
		m_dDispImagePartCol0 = Col0;
		m_dDispImagePartRow1 = Row1;
		m_dDispImagePartCol1 = Col1;

		ClearWindow(hImgWnd);
		m_iCursor=0;
		ShowImage();
	}


}
