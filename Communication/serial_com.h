#pragma once
#include<Windows.h>
#include<stdio.h>
#include<queue>
#include<iomanip>
#include<iostream>
#include"..\DataStructure\thread_safe_queue.h"

using namespace std;

struct SMessage
{
	unsigned char* pBuffer;//data buffer
	unsigned long ReceivedCount;//data receive count
};

class CCom
{
private:
	HANDLE m_hcom;
	DCB m_dcb;
	HKEY m_hkey;
	LPWSTR m_MainSubKey;
	int m_RET;
	LPWSTR m_pKeyValueName;
	unsigned long m_KeyValueType;
	unsigned long m_KeyValueDataSize;
	LPWSTR m_pKeyValueDataSize;

	//We use a data structure to pass the data received.
	threadsafe_queue<SMessage*> m_MessagesPointers;

public:
	CCom(void) :m_hcom{}, m_dcb{}, m_hkey{ HKEY_LOCAL_MACHINE }, m_MainSubKey{ TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM") }, m_RET{}, m_pKeyValueName{ TEXT("\\Device\\VCP0") }, m_KeyValueDataSize{}, m_KeyValueType{ KEY_READ }, m_pKeyValueDataSize{}
	{
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, m_MainSubKey, NULL, m_KeyValueType, &m_hkey);
		RegQueryValueEx(m_hkey, m_pKeyValueName, NULL, &m_KeyValueType, NULL, &m_KeyValueDataSize);
		m_pKeyValueDataSize = new TCHAR[m_KeyValueDataSize];

		RegQueryValueEx(m_hkey, m_pKeyValueName, NULL, &m_KeyValueType, (LPBYTE)m_pKeyValueDataSize, &m_KeyValueDataSize);
		RegCloseKey(m_hkey);
		m_hcom = CreateFile(m_pKeyValueDataSize, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (m_hcom == INVALID_HANDLE_VALUE)
		{
			MessageBox(nullptr, TEXT("Serial com error."), TEXT("Error"), MB_OK);
			exit(1);
		}
		SetupComm(m_hcom, 8192, 8192);         //输入缓冲区和输出缓冲区都是8192
		GetCommState(m_hcom, &m_dcb);            //读取串口配置信息
		m_dcb.BaudRate = 115200;
		m_dcb.ByteSize = 8;
		m_dcb.Parity = NOPARITY;
		m_dcb.StopBits = ONESTOPBIT;
		SetCommState(m_hcom, &m_dcb);            //修改串口配置信息
		PurgeComm(m_hcom, PURGE_TXCLEAR | PURGE_RXCLEAR);      //将接受缓冲区清空
	}
	~CCom()
	{
		CloseHandle(m_hcom);
		delete[] m_pKeyValueDataSize;
	}

	HANDLE ComHandle(void)
	{
		return m_hcom;
	}
	void __cdecl ReceiveData(void)
	{
		BOOL ReadSta = TRUE;

		while (true)
		{
#define SERIALCOM_BUFFER_SIZE 256
			SMessage* pMessage{ new SMessage{} };
			pMessage->pBuffer = (unsigned char*)calloc(SERIALCOM_BUFFER_SIZE + 1, sizeof(unsigned char));
			if (pMessage->pBuffer == nullptr)
			{
				MessageBoxW(nullptr, TEXT("Can not allocate memery"), TEXT("Serial com"), MB_OK);
				exit(1);
			}
			ReadSta = ReadFile(m_hcom, pMessage->pBuffer, SERIALCOM_BUFFER_SIZE, &(pMessage->ReceivedCount), NULL);
			m_MessagesPointers.push(pMessage);
			if (!ReadSta)
			{
				MessageBox(nullptr, TEXT("Can't read com."), TEXT("Serial com"), MB_OK);
				exit(1);
			}
		}
	}
	SMessage* GetOneMessage(void)
	{
		return m_MessagesPointers.wait_and_pop();
	}
};