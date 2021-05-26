#ifndef __MONITOR_STRUCT_H_
#define __MONITOR_STRUCT_H_
#pragma once
#include <Windows.h> 
#include <tchar.h>
#include "CommonDefs.h"
#include "FileMonitorListener.h"

struct MonitorStruct {
	OVERLAPPED m_overlapped;
	HANDLE m_dirHandle;
	BYTE m_buffer[32 * 1024];
	LPARAM lParam;
	DWORD m_notifyFilter;
	TCHAR* m_dirName;
	FileMonitorListener* m_FileMonitorListener;
};
#endif //__MONITOR_STRUCT_H_
