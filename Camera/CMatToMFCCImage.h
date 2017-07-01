#pragma once

#include "..\VideoLocalization\stdafx.h"

#define CVAPI_EXPORTS
#pragma warning(disable:4251 4819 4275)

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define CAMERA_CONTROL __declspec(dllexport)

//Containing showing
class CMatToMFCCImage
{
protected:
	CRect* m_RectToDraw;
	HDC m_HDCToDraw;
	cv::Mat* m_Frame;
	CImage m_ImageToShow;

	void Mat2CImage(void);
public:
	CMatToMFCCImage(void)
		:m_RectToDraw{ nullptr }, m_HDCToDraw{}, m_Frame{ nullptr }, m_ImageToShow{} {}

	virtual ~CMatToMFCCImage() {}

	void SetHDC(HDC HDCToDraw)
	{
		m_HDCToDraw = HDCToDraw;
	}
	void SetRect(CRect* RectToDraw)
	{
		if (RectToDraw == nullptr)
		{
			MessageBoxW(nullptr, TEXT("Set Rect fault."), TEXT("Nullptr"), MB_OK);
			exit(0);
		}
		m_RectToDraw = RectToDraw;
	}
	void CAMERA_CONTROL ShowImage(cv::Mat* pMat2Show);
};