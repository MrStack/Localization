#pragma once
#include<Windows.h>
#include<cmath>
#include"..\Communication\serial_com.h"
#include<thread>
#include<iostream>
#include<iomanip>
#include"..\DataStructure\thread_safe_queue.h"
#include<fstream>
#include"VideoLocalizationDlg.h"

//Define a rectangular
struct SRectangular
{
	long Width;
	long Height;
};

//Receive data structure definations
struct SPoint
{
	unsigned int Num;
	double x;
	double y;

	//The tolerance means that the point's fastest speed can not be more than 200cm per move
	//We use the tolerance to compare if the point's axis is right. If it's not, filt it
#define TOLERANCE_CM_INIT 200
	const float m_CompareTolerance;

	SPoint(void) :Num{}, x{}, y{}, m_CompareTolerance{ TOLERANCE_CM_INIT } {}
	SPoint(unsigned int unNum, double fx, double fy) :Num{ unNum }, x{ fx }, y{ fy }, m_CompareTolerance{ TOLERANCE_CM_INIT } {}
	//SPoint(SPoint& Point) :Num{ Point.Num }, x{ Point.x }, y{ Point.y }, m_CompareTolerance{ TOLERANCE_CM_INIT } {}
	~SPoint() = default;

	void Insert(unsigned int unNum, double fx, double fy)
	{
		Num = unNum;
		x = fx;
		y = fy;
	}

	virtual SPoint& operator=(SPoint& Point)
	{
		this->Num = Point.Num;
		this->x = Point.x;
		this->y = Point.y;
		return *this;
	}
	virtual bool operator==(SPoint& Point)
	{
		return Point.x - m_CompareTolerance < this->x && this->x < Point.x + m_CompareTolerance
			&& Point.y - m_CompareTolerance < this->y && this->y < Point.y + m_CompareTolerance;
	}
	virtual bool operator!=(SPoint& Point)
	{
		return !operator==(Point);
	}
	virtual bool operator>(SPoint& Point)
	{
		return abs(this->x - Point.x) > m_CompareTolerance || abs(this->y - Point.y) > m_CompareTolerance;
	}
	virtual bool operator<(SPoint& Point)
	{
		return !operator>(Point);
	}
	virtual bool operator>=(SPoint& Point)
	{
		//return abs(this->x - Point.x) >= m_CompareTolerance || abs(this->y - Point.y) >= m_CompareTolerance;
		return operator>(Point) && operator==(Point);
	}
	virtual bool operator<=(SPoint& Point)
	{
		return operator<(Point) && operator==(Point);
	}
};

#include<tuple>
class CReadFile
{
#include<fstream>
private:
	fstream m_File;
public:
	double X0, Y0;
	double X1, Y1;
	double X2, Y2;
	double Height;
public:
	CReadFile(const wchar_t* pLocation) :m_File{ pLocation,ios::in }
	{
		if (!m_File.is_open())
		{
			MessageBox(nullptr, TEXT("Can't open config file."), TEXT("Error"), MB_OK);
			exit(1);
		}
		m_File >> X0;
		m_File >> Y0;
		m_File >> X1;
		m_File >> Y1;
		m_File >> X2;
		m_File >> Y2;
		m_File >> Height;
	}

	//Get the real world rectangular width and height.
	std::tuple<long,long> ReadRect(void)
	{
		return std::tuple<long,long>{ static_cast<long>(X1),static_cast<long>(Y2) };
	}
};

extern size_t AnalyseCount;
extern size_t PrintCount;

class CLocAna
{
private:
	CCom m_Com;

	const double m_A0x;
	const double m_A0y;
	const double m_A1x;
	const double m_A1y;
	const double m_A2x;
	const double m_A2y;
	const double m_Height;

	double m_Reality_x;
	double m_Reality_y;
	double m_Reality_A0;
	double m_Reality_A1;
	double m_Reality_A2;

	double m_Distance[3][4];
	double m_FilterBuf[10][2]; 
	threadsafe_queue<SPoint> m_Queue;

	unsigned char m_MovingPoint;

	std::thread m_ComReceive;
	std::thread m_Analyzer;

	/*Test code*/
	std::fstream m_FileStream;
	std::fstream m_Axis;

	void Compute(unsigned char MovingPoint)
	{
		m_Reality_A0 = sqrt(pow(m_Distance[MovingPoint][0], 2.0) - pow(m_Height, 2.0));
		m_Reality_A1 = sqrt(pow(m_Distance[MovingPoint][1], 2.0) - pow(m_Height, 2.0));
		m_Reality_A2 = sqrt(pow(m_Distance[MovingPoint][2], 2.0) - pow(m_Height, 2.0));
		m_FileStream << m_Distance[MovingPoint][0] << " " << m_Distance[MovingPoint][1] << " " << m_Distance[MovingPoint][2] << std::endl;
	}

public:
	CLocAna(CReadFile& ConfigFile) :m_Com{}, m_A0x{ ConfigFile.X0 }, m_A0y{ ConfigFile.Y0 }, m_A1x{ ConfigFile.X1 }, m_A1y{ ConfigFile.Y1 }, m_A2x{ ConfigFile.X2 }, m_A2y{ ConfigFile.Y2 }, m_Height{ ConfigFile.Height },
		m_Reality_x{}, m_Reality_y{}, m_Reality_A0{}, m_Reality_A1{}, m_Reality_A2{}, m_Distance{}, m_FilterBuf{}, m_MovingPoint {},
		m_ComReceive{ [&](void) {m_Com.ReceiveData(); } }, m_Analyzer{ std::bind(&CLocAna::Analyze_data,this) },
		m_FileStream{ "Record.txt",std::ios::app | std::ios::in }, m_Axis{ "Axis.txt",std::ios::app | std::ios::in }
	{

	}
	~CLocAna()
	{
		m_ComReceive.detach();
		m_Analyzer.detach();
	}

	void Unpack(unsigned char MovingPoint)
	{
		Compute(MovingPoint);
		m_Reality_x = (pow(m_Reality_A0, 2) - pow(m_Reality_A1, 2) + pow(m_A1x, 2)) / (2 * m_A1x);
		m_Reality_y = (pow(m_Reality_A0, 2) - pow(m_Reality_A2, 2) - pow(m_Reality_x, 2) + pow(m_Reality_x - m_A2x, 2) + pow(m_A2y, 2)) / (2 * m_A2y);
		m_MovingPoint = MovingPoint;
		m_Axis << m_MovingPoint << " " << m_Reality_x << " " << m_Reality_y << endl;
		if (m_Reality_x > 10000 || m_Reality_x < -10000 || m_Reality_y>10000 || m_Reality_y < -10000)
		{
			return;
		}
		SPoint Point{ MovingPoint,m_Reality_x,m_Reality_y };
		m_Queue.push(Point);
	}
	void Analyze_data(void)
	{
		unsigned char PackBuffer[8]{};
		unsigned char CheckParity{};
#define MOVING_POINTS 3
		unsigned int AnchorMoPoint[MOVING_POINTS][3]{};

		SMessage* pMessageReceived;
		while (true)
		{
			pMessageReceived = m_Com.GetOneMessage();

			unsigned char* pToProcess{ pMessageReceived->pBuffer };
			unsigned long Counter{ pMessageReceived->ReceivedCount };

Pass:		while (*pToProcess != 0xE0 && Counter > 0)
			{
				pToProcess++;
				Counter--;
			}
			if (Counter <= 8)//If the frame is not complete, delete it
			{
				free(pMessageReceived);
				continue;
			}
			pToProcess++;//Pass the E0
			for (size_t i{}; i < 8; i++)
			{
				PackBuffer[i] = *pToProcess++;
				if (i != 7)
				{
					CheckParity ^= PackBuffer[i];
					//m_FileStream << std::hex << std::setw(4) << static_cast<int>(PackBuffer[i]) << ":" << static_cast<int>(CheckParity);
				}
				else
				{
					if (CheckParity != PackBuffer[7])
					{
						CheckParity = 0;
						goto Pass;
					}
					CheckParity = 0;
					AnchorMoPoint[PackBuffer[1]][PackBuffer[0]] = PackBuffer[0] + 1;
					//m_FileStream << std::hex << std::setw(4) << static_cast<int>(PackBuffer[i]) << ":" << static_cast<int>(CheckParity) << "  " << std::endl;
					m_Distance[PackBuffer[1]][PackBuffer[0]] = ((int)PackBuffer[2] << 16 | (int)PackBuffer[3] << 8 | (int)PackBuffer[4]) * 0.117375;
					m_Distance[PackBuffer[1]][3] += 1;
					if (m_Distance[PackBuffer[1]][3] != 3)
					{
						goto Pass;
					}
					if (AnchorMoPoint[PackBuffer[1]][0] == 1 && AnchorMoPoint[PackBuffer[1]][1] == 2 && AnchorMoPoint[PackBuffer[1]][2] == 3)
					{
						Unpack(PackBuffer[1]);
					}
					else
					{
						m_Distance[PackBuffer[1]][3] = 0;
						memset(AnchorMoPoint[PackBuffer[1]], 0, sizeof(unsigned int) * 3);
						goto Pass;
					}

					m_Distance[PackBuffer[1]][3] = 0;
					memset(AnchorMoPoint[PackBuffer[1]], 0, sizeof(unsigned int) * 3);
					goto Pass;
				}
			}
		}
	}

	SPoint GetPoint(void)
	{
		while (true)
		{
			if (m_Queue.size() > 0)
			{
				SPoint Point{ m_Queue.front() };
				m_Queue.pop();
				return Point;
			}
		}
	}
};