#include "CMatToMFCCImage.h"

void CMatToMFCCImage::Mat2CImage(void)
{
	if (m_Frame->empty())
	{
		MessageBoxW(nullptr, TEXT("Frame does not exist."), TEXT("cv::Mat to image"), MB_OK);
	}
	int nBPP = m_Frame->channels() * 8;
	m_ImageToShow.Destroy();
	m_ImageToShow.Create(m_Frame->cols, m_Frame->rows, nBPP);
	if (nBPP == 8)
	{
		static RGBQUAD pRGB[256];
		for (int i = 0; i < 256; i++)
			pRGB[i].rgbBlue = pRGB[i].rgbGreen = pRGB[i].rgbRed = i;
		m_ImageToShow.SetColorTable(0, 256, pRGB);
	}
	uchar* psrc = m_Frame->data;
	uchar* pdst = (uchar*)m_ImageToShow.GetBits();
	int imgPitch = m_ImageToShow.GetPitch();
	for (int y = 0; y < m_Frame->rows; y++)
	{
		memcpy(pdst, psrc, m_Frame->cols*m_Frame->channels());//cv::Mat.step is incorrect for those images created by roi (sub-images!)
		psrc += m_Frame->step;
		pdst += imgPitch;
	}
}

void CMatToMFCCImage::ShowImage(cv::Mat* pMat2Show)
{
	m_Frame = pMat2Show;
	Mat2CImage();
	m_ImageToShow.StretchBlt(m_HDCToDraw, *m_RectToDraw);
}