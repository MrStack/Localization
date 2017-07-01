
// VideoLocalizationDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include<thread>
#include<vector>
#include"..\AxisAnalyse\Location_Analyse.h"
#include"..\Camera\CMatToMFCCImage.h"
#include"..\Camera\CameraManaging.h"

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
	void SetCurrentPoint(POINT Point)//With point manipulation
	{
		m_CurrentPoint = { Point.x,m_pRect->Height() - Point.y };
		m_pCDC->MoveTo(m_CurrentPoint);
	}
	void SetCurrentPoint(int x, int y)//With point manipulation
	{
		m_CurrentPoint.x = x;
		m_CurrentPoint.y = m_pRect->Height() - y;
		
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

using std::vector;

template<typename SPoint>
class CFilter
{
protected:
	vector<SPoint> m_Buffer;
	vector<SPoint> m_Filterd;
public:
	CFilter(void) :m_Buffer{} {}
	~CFilter() = default;

	void Insert(SPoint& InsertData)
	{
		m_Buffer.push_back(InsertData);
	}
	void InsertFilterd(SPoint& InsertPoint)
	{
		m_Filterd.push_back(InsertPoint);
	}
	void Delete(SPoint& InsertData)
	{
		for (vector<SPoint>::iterator& iter{ m_Buffer.begin() }; iter != m_Buffer.end(); iter++)
		{
			if (*iter == InsertData)
			{
				iter = m_Buffer.erase(iter);
			}
		}
	}
	void Delete(int nIndex)
	{
		m_Buffer.erase(nIndex);
	}
	size_t Size(void)
	{
		return m_Buffer.size();
	}
	//This function takes a function object, which processes the m_Buffer
	template<class Fun>
	void Filt(Fun& fFunObj)
	{
		fFunObj(m_Buffer,m_Filterd);
	}
	void ClearBuff(void)
	{
		m_Buffer.clear();
		m_Filterd.clear();
	}
	vector<SPoint>& GetBuffer(void)
	{
		return m_Filterd;
	}

	SPoint& operator[](int Index)
	{
		return m_Filterd.at(Index);
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
		struct SPointAttr
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

	//Real world rectangular.
	const struct SRectangular m_RealWorldRectan;
	//Rect and real world rect ratio.
	//This should be known so the showing of points in the axis could zoom automatically.
	struct SRectangular m_RWR_R_Ratio;

	//Converte the real world point to the showing rect of point inside the program window.
	POINT RealWorldPointToPoint(SPoint& lrPoint)
	{
		return POINT{ static_cast<long>(lrPoint.x*m_RWR_R_Ratio.Width),static_cast<long>(lrPoint.y*m_RWR_R_Ratio.Height) };
	}
	
public:
	CFigure(CDC* pCDC, CReadFile& lrConfigFile)
		:m_Drawer{ pCDC }, m_Axis{ true,true,nullptr,nullptr,0,0 },
		m_FigureStyle{ {3,RGB(0,122,204),DOT},RGB(255,255,255) },
		m_Rect{}, m_Drew{ false }, m_RealWorldRectan{ std::get<0>(lrConfigFile.ReadRect()),std::get<1>(lrConfigFile.ReadRect()) }
	{
		//DrawBkColor(m_FigureStyle.clrAxisBkColor);
	}
	CFigure(CReadFile& lrConfigFile)
		:m_Drawer{}, m_Axis{ true,true,nullptr,nullptr,0,0 },
		m_FigureStyle{ { 3,RGB(0,122,204),DOT },RGB(255,255,255) },
		m_Rect{}, m_Drew{ false }, m_RealWorldRectan{ std::get<0>(lrConfigFile.ReadRect()),std::get<1>(lrConfigFile.ReadRect()) }
	{
		//DrawBkColor(m_FigureStyle.clrAxisBkColor);
	}
	~CFigure()
	{
		delete m_Axis.X_Label;
		delete m_Axis.Y_Label;
	}
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
	//Select object to draw
	//Warning:When using this function, make sure that the m_Rect has been already modified by the client funcion of MFC library.
	//For instance:CStatic AxisArea;AxisArea.GetClientRect(&(m_Figure.GetPlotRect()));//These procedure should already been executed.
	//Then:m_Figure.SelectObject(pCDC);
	//The showing rect should be known first, then the ratio of showing rect to real world rect can be set.
	void SelectObject(CDC* pCDC)
	{
		if (IS_NULLPTR(pCDC))
		{
			MessageBox(nullptr, TEXT("Nullptr in CFigure's memeber function SelectObject."), TEXT("Error"), MB_OK);
			exit(1);
		}
		m_Drawer.SetDC(pCDC);
		m_Drawer.SetClientRect(&m_Rect);

		//Setting the ratio of showing rect to real world rect.
		m_RWR_R_Ratio.Width = m_Rect.Width() / m_RealWorldRectan.Width;
		m_RWR_R_Ratio.Height = m_Rect.Height() / m_RealWorldRectan.Height;
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
	void ClearPoints(void)
	{
		m_Drawer.DrawBackground(m_Rect, m_FigureStyle.clrAxisBkColor);
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
		for (vector<POINT>::iterator iter{ begin(itrPoints) }; iter != end(itrPoints); iter++)
		{
			m_Drawer.SetCurrentPoint(*iter);
			m_Drawer.DrawPoint(m_FigureStyle.Point.nPointWidth, m_FigureStyle.Point.clrPointColor);
		}
	}
	void Plot(vector<SPoint>& lrPoints)
	{
		DrawBkColor(m_FigureStyle.clrAxisBkColor);
		for (vector<SPoint>::iterator iter{ begin(lrPoints) }; iter != end(lrPoints); iter++)
		{
			POINT Point{ static_cast<long>(iter->x),static_cast<long>(iter->y) };
			m_Drawer.SetCurrentPoint(Point);
			POINT TextPointTopLeft{ Point.x + 1,Point.y + 1 };
			POINT TextPointBottomRight{ Point.x + 8,Point.y + 14 };
			m_Drawer.DrawPoint(m_FigureStyle.Point.nPointWidth, m_FigureStyle.Point.clrPointColor);
			wchar_t* pStr{ new wchar_t[2] };//Attention:this buffer count may case display imperfect
			DrawTexts(TextPointTopLeft, TextPointBottomRight, NumToStr(iter->Num, pStr, 2), 1);
		}
	}
	//In use
	void Plot(struct SPoint& sPoint)
	{
		DrawBkColor(m_FigureStyle.clrAxisBkColor);
		POINT Point{};
		Point = RealWorldPointToPoint(sPoint);
		m_Drawer.SetCurrentPoint(Point);		
		POINT TextPointTopLeft{ Point.x + 1,m_Rect.Height()-Point.y + 1 };//With axis manipulation
		POINT TextPointBottomRight{ Point.x + 8,m_Rect.Height()-Point.y + 14 };//With axis manipulation
		m_Drawer.DrawPoint(m_FigureStyle.Point.nPointWidth, m_FigureStyle.Point.clrPointColor);
		wchar_t* pStr{ new wchar_t[2] };//Attention:this buffer count may case display imperfect
		DrawTexts(TextPointTopLeft, TextPointBottomRight, NumToStr(sPoint.Num, pStr, 2), 1);
	}
	//Special Plot. Using CFilter<SPoint>* as a input.
	template<typename SPoint>
	void Plot(CFilter<SPoint>* pPoints,int Count)
	{
		//DrawBkColor(m_FigureStyle.clrAxisBkColor);
		for (int i{}; i < Count; i++)
		{
			for (vector<SPoint>::iterator iter{ begin(pPoints[i].GetBuffer()) }; iter != end(pPoints[i].GetBuffer()); iter++)
			{
				POINT Point{ static_cast<long>(iter->x),static_cast<long>(iter->y) };
				m_Drawer.SetCurrentPoint(Point);
				POINT TextPointTopLeft{ Point.x + 1,Point.y + 1 };
				POINT TextPointBottomRight{ Point.x + 8,Point.y + 14 };
				m_Drawer.DrawPoint(m_FigureStyle.Point.nPointWidth, m_FigureStyle.Point.clrPointColor);
				wchar_t* pStr{ new wchar_t[2] };//Attention:this buffer count may case display imperfect
				DrawTexts(TextPointTopLeft, TextPointBottomRight, NumToStr(iter->Num, pStr, 2), 1);
			}
		}
	}
	void MovePointTo(POINT Point)
	{
		m_Drawer.MovePointTo(Point);
	}

	wchar_t* NumToStr(long lNum,wchar_t* pString,int nStrLen)
	{
		_ltow_s(lNum, pString, nStrLen, 10);
		
		return pString;
	}
	void DrawTexts(POINT TopLeft, POINT BottomRight, LPCWSTR pString, int nStrLen)
	{
		m_Drawer.DrawTexts(TopLeft, BottomRight, pString, nStrLen);
	}
};



//This is a function object class, which sifts the unusual elements
template<typename SPoint>
class CSiftFilter
{
protected:
	SPoint m_Average;

	void Average(vector<SPoint>& Filter)
	{
		int xAvr{};
		int yAvr{};
		
		vector<int> Int{};

		for (vector<SPoint>::iterator iter{ begin(Filter) }; iter != end(Filter); iter++)
		{
			xAvr += static_cast<int>((*iter).x);
			yAvr += static_cast<int>((*iter).y);;
		}
		xAvr /= static_cast<int>(Filter.size());
		yAvr /= static_cast<int>(Filter.size());

		m_Average.x = xAvr;
		m_Average.y = yAvr;
	}
	void SiftUnusual(vector<SPoint>& Filter,vector<SPoint>& Filtered)
	{
		Average(Filter);
		for (vector<SPoint>::iterator iter{ begin(Filter) }; iter != end(Filter); iter++)
		{
			if ((*iter) > m_Average)
			{
				continue;
			}
			else
			{
				Filtered.push_back(*iter);
			}
		}
	}
public:
	CSiftFilter(void) :m_Average{} {}
	~CSiftFilter() = default;

	void operator()(vector<SPoint>& Filter,vector<SPoint>& Filtered)
	{
		SiftUnusual(Filter,Filtered);
	}
};
#include<algorithm>
template<typename SPoint>
class CReceiveData
{
protected:
	threadsafe_queue<SPoint> m_Buffer;

	CFilter<SPoint>* m_Filter;
	int m_FilterCount;

	CSiftFilter<SPoint> m_SiftFilterFunObj;

#define MAX_POINTS_TO_FIND 15
	const int m_cPointsToFind;

	void Copy(SPoint& pPoint)
	{		
		(m_Filter + pPoint.Num)->Insert(pPoint);
	}
	//This function finds the number of the moving points, which used to determin the number of buffer.
	void FindMaxIndex(CLocAna& lrLocation)
	{
		vector<unsigned int> PointsToFindMax;
		for (int i{}; i < m_cPointsToFind; i++)
		{
			PointsToFindMax.push_back(lrLocation.GetPoint().Num);
		}
		vector<unsigned int>::iterator fiter{ begin(PointsToFindMax) };
		vector<unsigned int>::iterator liter{ end(PointsToFindMax) };
		m_FilterCount = *(max_element(fiter, liter)) + 1;//The 1 is added because of the index starts with 0
		//Construct the Buffers
		m_Filter = { new CFilter<SPoint>[m_FilterCount] };
	}

public:
	CReceiveData(CLocAna& lrLocationAna) :m_Buffer{}, m_Filter{ nullptr }, m_SiftFilterFunObj{}, m_cPointsToFind{ MAX_POINTS_TO_FIND }, m_FilterCount{}
	{
		FindMaxIndex(lrLocationAna);
	}
	~CReceiveData()
	{
		delete[] m_Filter;
	}

	void Receive(CLocAna& rlLocation)
	{
		Copy(rlLocation.GetPoint());
	}
	void Process(void)
	{
		for (int i{}; i < m_FilterCount; i++)
		{
			if (m_Filter[i].Size() == 20)
			{
				m_Filter[i].Filt(m_SiftFilterFunObj);
				for (vector<SPoint>::iterator iter{ begin(m_Filter[i].GetBuffer()) }; iter != end(m_Filter[i].GetBuffer()); iter++)
				{
					m_Buffer.push(*iter);
				}
				m_Filter[i].ClearBuff();
			}
		}
	}
	int GetFilterCount(void)
	{
		return m_FilterCount;
	}
	CFilter<SPoint>* GetFilterPionter(void)
	{
		return m_Filter;
	}
	SPoint Dequeue(void)
	{
		auto Ret{ m_Buffer.wait_and_pop() };
		//auto Ret{ m_Buffer.front() };
		//m_Buffer.try_pop();

		return Ret;
	}
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

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


public:
	// User variables
	CReadFile m_FileReader;
	CStatic m_location_axis;//Location axis control
	CLocAna m_LocationAnalyser;
	CReceiveData<SPoint> m_Receiver;
	thread m_FigureInput;
	thread m_ProcessData;

	CStatic m_MainViewControl;//Main view control
	CMatToMFCCImage m_MainViewImage;

	CStatic m_SideViewControl;//A side view control 1
	CMatToMFCCImage m_SideViewImage;

	CCameraManager m_CameraManager;
	CInfoFusion m_InfoFusion;

	struct SPoint* m_pSPoint;
	class CFigure m_Figure;

	afx_msg void OnStnClickedLocationPoints();
	afx_msg void OnAcnStartAnimate1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);	
};
