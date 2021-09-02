
// Load_East_ModelDlg.h: 头文件
//
#pragma once

#include "East_Dll_Export_H.h"

#include "HalconCpp.h"
using namespace HalconCpp;

// CLoadEastModelDlg 对话框
class CLoadEastModelDlg : public CDialogEx
{
// 构造
public:
	CLoadEastModelDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOAD_EAST_MODEL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoadModel();

	HObject m_hImage;
	HTuple  m_hWidth;
	HTuple  m_hHeight;
	HTuple	hImgWnd;
	double  m_dDispImagePartRow0;
	double  m_dDispImagePartCol0;
	double  m_dDispImagePartRow1;
	double  m_dDispImagePartCol1;
	HTuple  m_hRowB, m_hColB, m_hRowE, m_hColE;
	int m_iCursor;//按下左键，滚轮滚动
	double m_dXO, m_dYO;

	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void ShowImage();
	BOOL PreTranslateMessage(MSG * pMsg);
	void CLoadEastModelDlg::MoveWnd(CPoint point, HObject srcImg, HTuple hWindow);
	void CLoadEastModelDlg::OnMouseMove(UINT nFlags, CPoint point);

	HObject hoRectDisp;

	East_DLL_Export_H east_model;
	afx_msg void OnBnClickedTest();
};