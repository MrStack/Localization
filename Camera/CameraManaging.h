#pragma once
#include"FlyCapture2.h"
#include"..\AxisAnalyse\Location_Analyse.h"
#include<opencv2/opencv.hpp>
#include<cstdlib>
#include<Windows.h>
#include<map>

class CCameraManager
{
#define CAMERA_ID_NUMBER //This is an identifier to know that this is a camera id number.
private:
	FlyCapture2::Camera* m_pCams;
	unsigned int m_numCameras;
	FlyCapture2::PGRGuid* m_pGuids;
	FlyCapture2::CameraInfo* m_pCamInfo;
	std::map<unsigned int CAMERA_ID_NUMBER, FlyCapture2::Camera*> m_IdsMapCameras;
	FlyCapture2::Error m_Error;
	FlyCapture2::Image* m_pRawImage;
	FlyCapture2::Image* m_pRgbImage;
	cv::Mat* m_pMatImage;
	std::map<unsigned int CAMERA_ID_NUMBER, cv::Mat*> m_IdsMapMat;
	FlyCapture2::BusManager m_busMgr;

	void PrintError(FlyCapture2::Error& error) { error.PrintErrorTrace(); }
public:
	CCameraManager(void) :m_pCams{}, m_numCameras{}, m_pGuids{}, m_pCamInfo{}, m_IdsMapCameras{},
		m_IdsMapMat{},m_Error{}, m_pRawImage{}, m_pRgbImage{}, m_pMatImage{}, m_busMgr{} {}
	~CCameraManager()
	{
		//Stop and disconnect the cameras.
		for (unsigned int i{}; i < m_numCameras; i++)
		{
			// Stop capturing images
			m_Error = m_pCams[i].StopCapture();
			if (m_Error != FlyCapture2::PGRERROR_OK)
			{
				PrintError(m_Error);
			}

			// Disconnect the camera
			m_Error = m_pCams[i].Disconnect();
			if (m_Error != FlyCapture2::PGRERROR_OK)
			{
				PrintError(m_Error);
			}
		}

		delete[] m_pCams;
		delete[] m_pGuids;
		delete[] m_pCamInfo;
		delete[] m_pRawImage;
		delete[] m_pRgbImage;
		delete[] m_pMatImage;
	}

	//To start capture, firstly use this function. Ans this funciton will connect the cameras.
	void InitialResources(void)
	{
		m_Error = m_busMgr.GetNumOfCameras(&m_numCameras);
		if (m_Error != FlyCapture2::PGRERROR_OK)
		{
			PrintError(m_Error);
			MessageBox(nullptr, TEXT("Camera bus failed."), TEXT("Error"), MB_OK);
			exit(1);
		}

		//Initialize the resources.
		m_pCams = new FlyCapture2::Camera[m_numCameras]{};
		m_pGuids = new FlyCapture2::PGRGuid[m_numCameras]{};
		m_pCamInfo = new FlyCapture2::CameraInfo[m_numCameras]{};
		m_pRawImage = new FlyCapture2::Image[m_numCameras]{};
		m_pRgbImage = new FlyCapture2::Image[m_numCameras]{};
		m_pMatImage = new cv::Mat[m_numCameras]{};

		//Get cameras' guid.
		for (unsigned int i{}; i < m_numCameras; i++)
		{
			m_Error = m_busMgr.GetCameraFromIndex(i, &m_pGuids[i]);  //索引相机并将地址存放在guid里面
			if (m_Error != FlyCapture2::PGRERROR_OK)
			{
				PrintError(m_Error);
				MessageBox(nullptr, TEXT("Camera finding failed."), TEXT("Error"), MB_OK);
				exit(1);
			}
		}



		//Connect all the cameras.
		for (unsigned int i{}; i < m_numCameras; i++)
		{
			m_pCams[i].Connect(&m_pGuids[i]);

			if (m_Error != FlyCapture2::PGRERROR_OK)
			{
				PrintError(m_Error);
				MessageBox(nullptr, TEXT("Some camera connection failed."), TEXT("Error"), MB_OK);
				exit(1);
			}
		}

		//Get cameras' info and map cameras' serial number to the cameras.
		for (unsigned int i{}; i < m_numCameras; i++)
		{
			m_Error = m_pCams[i].GetCameraInfo(&m_pCamInfo[i]);
			if (m_Error != FlyCapture2::PGRERROR_OK)
			{
				PrintError(m_Error);
				MessageBox(nullptr, TEXT("Failed to get some of the camera's info."), TEXT("Error"), MB_OK);
				exit(1);
			}
			m_IdsMapCameras.insert(std::make_pair(m_pCamInfo[i].serialNumber, &m_pCams[i]));//Map the serial number to the camera pointer.
			m_IdsMapMat.insert(std::make_pair(m_pCamInfo[i].serialNumber, &m_pMatImage[i]));//Maping the serial number to the cv::Mat in order to identify the image from which camera.
		}

	}

	//When cameras are initialized, use this function to start capture.
	void StartCapture(void)
	{
		for (unsigned int i{}; i < m_numCameras; i++)
		{
			m_pCams[i].StartCapture();

			if (m_Error != FlyCapture2::PGRERROR_OK)
			{
				PrintError(m_Error);
				MessageBox(nullptr, TEXT("Some camera can't run."), TEXT("Error"), MB_OK);
				exit(1);
			}
		}
	}

	//In capturing mode, use this function to retrieve images for one frame.
	void RetrieveImage(void)
	{
		for (unsigned int i{}; i < m_numCameras; i++)
		{
			m_pCams[i].RetrieveBuffer(&m_pRawImage[i]);
			if (m_Error != FlyCapture2::PGRERROR_OK)
			{
				PrintError(m_Error);
				continue;
			}

			m_Error = m_pRawImage[i].Convert(FlyCapture2::PIXEL_FORMAT_BGR, &m_pRgbImage[i]); //将原始图像转换成三通道RGB
			if (m_Error != FlyCapture2::PGRERROR_OK)
			{
				PrintError(m_Error);
				continue;
			}

			// convert to OpenCV Mat  
			unsigned int rowBytes{ static_cast<unsigned int>(static_cast<double>(m_pRgbImage[i].GetReceivedDataSize()) / static_cast<double>(m_pRgbImage[i].GetRows())) };

			m_pMatImage[i] = cv::Mat(m_pRgbImage[i].GetRows(), m_pRgbImage[i].GetCols(), CV_8UC3, m_pRgbImage[i].GetData(), rowBytes);
		}
	}

	//Get the map's iterator pointer of serial number and cv::Mat pointer.
	std::map<unsigned int CAMERA_ID_NUMBER, cv::Mat*>* GetImages(void)
	{
		return &m_IdsMapMat;
	}
};


//TODO:Consider the show on the window, the camera on the specific position in the monitering situation whose serial number should be known to
//print the image on the specific window using the map<unsigned int CAMERA_ID_NUMBER,cv::Mat*>::iterator*.

enum EQuadrant
{
	ERROR_QUADRANT = 0, FIRST_QUADRANT, SECOND_QUADRANT, THIRD_QUADRANT, FOURTH_QUADRANT
};

//To judge if the point is in the specific position
class CSpotJudge
{
protected:
	CRect* m_pRect;

	//Real world rect.
	struct SRectangular m_RealWorldRectan;
	//Rect and real world rect ratio.
	//This should be known so the showing of points in the axis could zoom automatically.
	struct SRectangular m_RWR_R_Ratio;

	//The comparision point
	POINT m_CentralPoint;
public:
	CSpotJudge(void) {}
	~CSpotJudge() {}

	//Compute needed parameters
	void ComputeParams(CRect& lrRect, CReadFile& lrConfigFile)
	{
		m_pRect = &lrRect;

		//Get the real world rectangular.
		m_RealWorldRectan.Width = std::get<0>(lrConfigFile.ReadRect());
		m_RealWorldRectan.Height = std::get<1>(lrConfigFile.ReadRect());

		//Rect and real world rect ratio.
		m_RWR_R_Ratio.Width = m_pRect->Width() / m_RealWorldRectan.Width;
		m_RWR_R_Ratio.Height = m_pRect->Height() / m_RealWorldRectan.Height;

		//Central point the comparision element.
		m_CentralPoint.x = m_pRect->Width() / 2;
		m_CentralPoint.y = m_pRect->Height() / 2;
	}

	//Determine the location of the current real world point.
	EQuadrant MesurePosition(SPoint& lrPoint) 
	{
		//Convert the real world point to the point on the window of positioning, then compare it.
		if ((lrPoint.x*m_RWR_R_Ratio.Width > m_CentralPoint.x) && (lrPoint.y*m_RWR_R_Ratio.Height < m_CentralPoint.y))
		{
			//First quadrant detection
			return FIRST_QUADRANT;
		}
		else if ((lrPoint.x*m_RWR_R_Ratio.Width <= m_CentralPoint.x) && (lrPoint.y*m_RWR_R_Ratio.Height <= m_CentralPoint.y))
		{
			//Second quadrant detection
			return SECOND_QUADRANT;
		}
		//else if ((lrPoint.x*m_RWR_R_Ratio.Width <= m_CentralPoint.x) && (lrPoint.y*m_RWR_R_Ratio.Height >= m_CentralPoint.y))
		//{
		//	//Third quadrant detection
		//	return THIRD_QUADRANT;
		//}
		//else if ((lrPoint.x*m_RWR_R_Ratio.Width > m_CentralPoint.x) && (lrPoint.y*m_RWR_R_Ratio.Height > m_CentralPoint.y))
		//{
		//	//Fourth quadrant detection
		//	return FOURTH_QUADRANT;
		//}
		else
		{
			////Error
			//return ERROR_QUADRANT;
			//Test
			return FIRST_QUADRANT;
		}
	}
};

#include"VideoLocalizationDlg.h"
//Out put the main view and side view cv::Mat
class CInfoFusion
{
private:
	unsigned int m_SpotCamSerialNum;
	std::map<unsigned int CAMERA_ID_NUMBER, cv::Mat*>* m_pSerialMapMat;
	std::map<EQuadrant, unsigned int> m_QuadrantMapSerial;
#define FIRST_QUADRANT_CAM_ID  14468908
#define SECOND_QUADRANT_CAM_ID 15297065
#define THIRD_QUADRANT_CAM_ID  123456
#define FOURTH_QUADRANT_CAM_ID 123456

	CSpotJudge m_SpotJudge;

	//Using fusion methods to find the spot camera.
	unsigned int CAMERA_ID_NUMBER FindSpotCam(SPoint& lrPoint)
	{
		auto iter{ m_QuadrantMapSerial.find(m_SpotJudge.MesurePosition(lrPoint)) };
		
		return iter->second;
	}
public:
	CInfoFusion(void) :m_SpotJudge{},
		m_SpotCamSerialNum{}, m_pSerialMapMat{}, m_QuadrantMapSerial{}
	{
		//Initialize the map of quadrant and serial number of camera.
		m_QuadrantMapSerial.insert(std::make_pair(FIRST_QUADRANT, FIRST_QUADRANT_CAM_ID));
		m_QuadrantMapSerial.insert(std::make_pair(SECOND_QUADRANT, SECOND_QUADRANT_CAM_ID));
	}
	~CInfoFusion() {}

	//Set pointer of std::map<unsigned int CAMERA_ID_NUMBER, cv::Mat*>*
	void SetMapPointer(std::map<unsigned int CAMERA_ID_NUMBER, cv::Mat*>* pMap)
	{
		m_pSerialMapMat = pMap;
	}

	//Set axis figure rect
	void SetJudgeArgs(CRect& lrRect, CReadFile& lrConfigFile)
	{
		m_SpotJudge.ComputeParams(lrRect, lrConfigFile);
	}

	//Get the Mat* of main view, using the point.
	cv::Mat* GetMainViewMat(SPoint& lrPoint)
	{
		m_SpotCamSerialNum = FindSpotCam(lrPoint);
		auto iter{ m_pSerialMapMat->find(m_SpotCamSerialNum) };

		return iter->second;
	}

	//Using serial number of camera to find the Mat*.
	cv::Mat* GetSideViewMats(unsigned int unSerialNumber) const
	{
		auto iter{ m_pSerialMapMat->find(unSerialNumber) };

		return iter->second;
	}
};