
// VideoLocalizationDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include<vector>

using namespace std;

// Macro definations

#define IS_NULLPTR(p) (p)==nullptr
#define ISNOT_NULLPTR(p) (p)==nullptr

// Struct definations


// Class definations

class CDrawing
{
protected:
	CPen m_DrawPen;
	CBrush m_DrawBrush;
	CDC* m_pCDC;
	CRect* m_pRect;

	POINT m_CurrentPoint;
	int m_CurrentWidth;
	COLORREF m_CurrentColor;

	int PointCheck(POINT Point)
	{
		return Point.x + (int)ceil(m_CurrentWidth/2)+1 >= m_pRect->Width() || Point.y + (int)ceil(m_CurrentWidth/2)+1 >= m_pRect->Height();
	}
public:
	CDrawing(CDC* pCDC) :m_DrawPen{}, m_DrawBrush{}, m_pCDC{ pCDC }, m_CurrentPoint{}, m_CurrentColor{}, m_CurrentWidth{}, m_pRect{ nullptr }
	{
		if (IS_NULLPTR(pCDC))
		{
			MessageBox(nullptr, TEXT("Initialization of drawing object error. Nullptr of CDC."), TEXT("Error"), MB_OK);
			exit(1);
		}
	}
	CDrawing(void) :m_DrawPen{}, m_DrawBrush{}, m_pCDC{}, m_CurrentPoint{}, m_CurrentColor{}, m_CurrentWidth{}, m_pRect{ nullptr } {}
	~CDrawing()
	{
		m_DrawPen.~CPen();
		m_DrawBrush.~CBrush();
	}

	void SetDC(CDC* pCDC)
	{
		if (IS_NULLPTR(pCDC))
		{
			MessageBox(nullptr, TEXT("Failed to set DC in CDrawer. Nullptr"), TEXT("Error"), MB_OK);
			exit(1);
		}
		m_pCDC = pCDC;
	}
	void SetClientRect(CRect* pRect)
	{
		if (IS_NULLPTR(pRect))
		{
			MessageBox(nullptr, TEXT("Failed to set Client Rect in CDrawer. Nullptr"), TEXT("Error"), MB_OK);
			exit(1);
		}
		m_pRect = pRect;
	}
	virtual void DrawBackground(CRect& lrRect, COLORREF clrColor)
	{
		m_CurrentColor = clrColor;
		m_pRect = &lrRect;
		m_DrawBrush.CreateSolidBrush(m_CurrentColor);
		m_pCDC->SelectObject(&m_DrawBrush);
		m_pCDC->Rectangle(lrRect);
		m_DrawBrush.DeleteObject();
	}
	virtual void DrawBackground(COLORREF clrColor)
	{
		m_CurrentColor = clrColor;
		m_DrawBrush.CreateSolidBrush(m_CurrentColor);
		m_pCDC->SelectObject(&m_DrawBrush);
		m_pCDC->Rectangle(m_pRect);
		m_DrawBrush.DeleteObject();
	}
	void SetCurrentPoint(POINT Point)
	{
		m_CurrentPoint = Point;
		m_pCDC->MoveTo(m_CurrentPoint);
	}
	void SetCurrentPoint(int x, int y)
	{
		m_CurrentPoint.x = x;
		m_CurrentPoint.y = y;
		m_pCDC->MoveTo(m_CurrentPoint);
	}
	void SetCurrentColor(COLORREF clrColor)
	{
		m_CurrentColor = clrColor;
	}
	void SetCurrentWidth(int nWidth)
	{
		m_CurrentWidth = nWidth;
	}
	void DrawPoint(POINT Point, int nWidth, COLORREF clrColor)
	{
		if (PointCheck(Point))
		{
			//When the point goes out of the rect, do not draw.
			return;
		}
		SetCurrentColor(clrColor);
		SetCurrentWidth(nWidth);
		m_DrawPen.CreatePen(PS_SOLID, nWidth, m_CurrentColor);
		m_pCDC->SelectObject(&m_DrawPen);
		SetCurrentPoint(Point);
		m_pCDC->Pie(m_CurrentPoint.x - m_CurrentWidth / 2, m_CurrentPoint.y - m_CurrentWidth / 2, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y + m_CurrentWidth / 2, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y);
		//m_pCDC->Ellipse(m_CurrentPoint.x - m_CurrentWidth / 2, m_CurrentPoint.y - m_CurrentWidth / 2, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y + m_CurrentWidth / 2);
		m_DrawPen.DeleteObject();
	}
	void DrawPoint(int nWidth, COLORREF clrColor)
	{
		if (PointCheck(m_CurrentPoint))
		{
			//When the point goes out of the rect, do not draw.
			return;
		}
		SetCurrentColor(clrColor);
		SetCurrentWidth(nWidth);
		m_DrawPen.CreatePen(PS_SOLID, nWidth, m_CurrentColor);
		m_pCDC->SelectObject(&m_DrawPen);
		m_pCDC->Pie(m_CurrentPoint.x - m_CurrentWidth / 2, m_CurrentPoint.y - m_CurrentWidth / 2, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y + m_CurrentWidth / 2, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y);
		//m_pCDC->Ellipse(m_CurrentPoint.x - m_CurrentWidth / 2, m_CurrentPoint.y - m_CurrentWidth / 2, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y + m_CurrentWidth / 2);
		m_DrawPen.DeleteObject();
	}
	void MovePointTo(POINT Point)
	{
		if (PointCheck(Point))
		{
			//When the point goes out of the rect, do not draw, but refresh the background.
			COLORREF BkColor{ m_pCDC->GetBkColor() };

			m_DrawPen.CreatePen(PS_SOLID, m_CurrentWidth, BkColor);
			m_pCDC->SelectObject(&m_DrawPen);
			m_pCDC->Pie(m_CurrentPoint.x - m_CurrentWidth / 2, m_CurrentPoint.y - m_CurrentWidth / 2, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y + m_CurrentWidth / 2, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y);
			//m_pCDC->Ellipse(m_CurrentPoint.x - m_CurrentWidth / 2, m_CurrentPoint.y - m_CurrentWidth / 2, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y + m_CurrentWidth / 2);
			m_DrawPen.DeleteObject();
			return;
		}

		COLORREF BkColor{ m_pCDC->GetBkColor()};
		
		m_DrawPen.CreatePen(PS_SOLID, m_CurrentWidth, BkColor);
		m_pCDC->SelectObject(&m_DrawPen);
		m_pCDC->Pie(m_CurrentPoint.x - m_CurrentWidth / 2, m_CurrentPoint.y - m_CurrentWidth / 2, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y + m_CurrentWidth / 2, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y);
		//m_pCDC->Ellipse(m_CurrentPoint.x - m_CurrentWidth / 2, m_CurrentPoint.y - m_CurrentWidth / 2, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y + m_CurrentWidth / 2);
		m_DrawPen.DeleteObject();

		SetCurrentPoint(Point);
		m_DrawPen.CreatePen(PS_SOLID, m_CurrentWidth, m_CurrentColor);
		m_pCDC->SelectObject(&m_DrawPen);
		m_pCDC->Pie(m_CurrentPoint.x - m_CurrentWidth / 2, m_CurrentPoint.y - m_CurrentWidth / 2, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y + m_CurrentWidth / 2, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y);
		//m_pCDC->Ellipse(m_CurrentPoint.x - m_CurrentWidth / 2, m_CurrentPoint.y - m_CurrentWidth / 2, m_CurrentPoint.x + m_CurrentWidth / 2, m_CurrentPoint.y + m_CurrentWidth / 2);
		m_DrawPen.DeleteObject();
	}
	void MovePointTo(int x, int y)
	{
		POINT Point{ x,y };
		MovePointTo(Point);
	}
	/*void DrawLineTo(POINT EndPoint)
	{
		COLORREF CurrentPointColor{ m_pCDC->GetDCPenColor() };
	}
	void DrawLintTo(int EndX, int EndY)
	{

	}*/
	void Ellipse(POINT TopLeft, POINT BottomRight)
	{
		m_DrawPen.CreatePen(PS_SOLID, 1, m_CurrentColor);
		m_pCDC->SelectObject(&m_DrawPen);
		m_pCDC->Ellipse(TopLeft.x, TopLeft.y, BottomRight.x, BottomRight.y);
		m_DrawPen.DeleteObject();
	}

	void DrawTexts(POINT TopLeft,POINT BottomRight,LPCWSTR pString,int nStrLen)
	{
		CRect Rect{ TopLeft,BottomRight };
		DrawText(*m_pCDC, pString, nStrLen, &Rect, DT_CENTER&DT_VCENTER);
	}
};

class CFigure
{
protected:
	struct SAxis
	{
		bool X_Axis;
		bool Y_Axis;
		wchar_t* X_Label;
		wchar_t* Y_Label;
		double X_Range;
		double Y_Range;
	};
	enum EPointStyle
	{
		STAR, DOT, PLUS, CIRCLE
	};
	struct SFigureStyle
	{
		struct SPoint
		{
			int nPointWidth;
			COLORREF clrPointColor;
			enum EPointStyle ePointStyle;
		}Point;
		COLORREF clrAxisBkColor;
	};
	struct SAxis m_Axis;
	struct SFigureStyle m_FigureStyle;

	//Control variables
	CRect m_Rect;
	CDrawing m_Drawer;
	bool m_Drew;//Check if the background color is drew

	//Protected functions
	void DrawBkColor(COLORREF clrColor)
	{
		if (m_Drew == true)
		{
			return;
		}
		else
		{
			m_Drawer.DrawBackground(m_Rect, clrColor);
			m_Drew = true;
		}
	}
public:
	CFigure(CDC* pCDC)
		:m_Drawer{ pCDC }, m_Axis{ true,true,nullptr,nullptr,0,0 },
		m_FigureStyle{ {3,RGB(0,122,204),DOT},RGB(255,255,255) },
		m_Rect{}, m_Drew{ false } {}
	CFigure(void)
		:m_Drawer{}, m_Axis{ true,true,nullptr,nullptr,0,0 },
		m_FigureStyle{ { 3,RGB(0,122,204),DOT },RGB(255,255,255) },
		m_Rect{}, m_Drew{ false } {}
	~CFigure()
	{
		delete m_Axis.X_Label;
		delete m_Axis.Y_Label;
	}

	void SelectObject(CDC* pCDC)
	{
		if (IS_NULLPTR(pCDC))
		{
			MessageBox(nullptr, TEXT("Nullptr in CFigure's memeber function SelectObject."), TEXT("Error"), MB_OK);
			exit(1);
		}
		m_Drawer.SetDC(pCDC);
		m_Drawer.SetClientRect(&m_Rect);
	}
	CRect& GetPlotRect(void)
	{
		return m_Rect;
	}
	void SetXLabel(const wchar_t* pXlabel)
	{
		m_Axis.X_Label = new wchar_t[strlen((char*)pXlabel)];
		memcpy(m_Axis.X_Label, pXlabel, sizeof(wchar_t) * strlen((char*)pXlabel));
	}
	void SetYLabel(const wchar_t* pYlabel)
	{
		m_Axis.Y_Label = new wchar_t[strlen((char*)pYlabel)];
		memcpy(m_Axis.Y_Label, pYlabel, sizeof(wchar_t) * strlen((char*)pYlabel));
	}
	void Plot(POINT Point)
	{
		DrawBkColor(m_FigureStyle.clrAxisBkColor);
		m_Drawer.SetCurrentPoint(Point);
		m_Drawer.DrawPoint(m_FigureStyle.Point.nPointWidth, m_FigureStyle.Point.clrPointColor);
	}
	void Plot(vector<POINT>& itrPoints)
	{
		DrawBkColor(m_FigureStyle.clrAxisBkColor);
		for (vector<POINT>::iterator iter{}; iter != end(itrPoints); iter++)
		{
			m_Drawer.SetCurrentPoint(*iter);
			m_Drawer.DrawPoint(m_FigureStyle.Point.nPointWidth, m_FigureStyle.Point.clrPointColor);
		}
	}
	void MovePointTo(POINT Point)
	{
		m_Drawer.MovePointTo(Point);
	}

	void NumToStr(long lNum,wchar_t* pString,int nStrLen)
	{
		_ltow_s(lNum, pString, nStrLen, 10);
	}
	void DrawTexts(POINT TopLeft, POINT BottomRight, LPCWSTR pString, int nStrLen)
	{
		m_Drawer.DrawTexts(TopLeft, BottomRight, pString, nStrLen);
	}

};

template<typename T>
class CFilter
{
	using std::vector;
protected:
	vector<T> m_Buffer;
public:
	CFilter(void) :m_Buffer{} {}
	~CFilter() = default;
	
	void Insert(T InsertData)
	{
		m_Buffer.push_back(InsertData);
	}
	void Delete(T InsertData)
	{
		for (vector<T>::iterator& iter{}, int index{}; iter != end(m_Buffer); iter++, index++)
		{
			if (iter == InsertData)
			{
				m_Buffer.erase(index);
			}
		}
	}
	void Delete(int nIndex)
	{
		m_Buffer.erase(nIndex);
	}
	//This function takes a function object, which processes the m_Buffer
	template<class Fun>
	virtual void Filt(Fun& fFunObj)
	{
		fFunObj(&m_Buffer);
	}
};

//This is a function object class, which sifts the unusual elements
template<typename T>
class CSiftFilter
{
protected:
public:
	int operator()(CFilter& Filter)
	{

	}
};

//Receive data structure definations
struct SPoint
{
	unsigned int Num;
	float x;
	float y;
};

template<typename DataType>
class CReceiveData
{
protected:
	vector<DataType> m_Buffer;
public:
	CReceiveData(void) :m_Buffer{} {}
	~CReceiveData() = default;
};

// CVideoLocalizationDlg dialog
class CVideoLocalizationDlg : public CDialogEx
{
// Construction
public:
	CVideoLocalizationDlg(CWnd* pParent = NULL);	// standard constructor

// User functions
#define PIXEL_PER_CM 1500 //Unit:CM. This value should be manipulated by the real axis.TODO:Use a variable when the stick point changed.


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIDEOLOCALIZATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	// User variables

	struct SPoint* m_pSPoint;
	class CFigure m_Figure{};

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	// User variables
	CStatic m_location_axis;
	afx_msg void OnStnClickedLocationPoints();
	afx_msg void OnBnClickedDraw();
	afx_msg void OnAcnStartAnimate1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
