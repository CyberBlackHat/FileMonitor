#ifndef __FILE_MONITOR_LISTENR_IMP_WIN_H_
#define __FILE_MONITOR_LISTENR_IMP_WINH_ 
#pragma once
#include <memory>
#include "FileMonitorListener.h"
#include "ConcurrentQueue.h"
#include <Windows.h>

class FileMonitorListenerImpWin:public FileMonitorListener {
	struct LogMessage; //Forward declaration 
	//typedef std::shared_ptr<LogMessage> LogMessagePtr; 
public:
	FileMonitorListenerImpWin();
	virtual ~FileMonitorListenerImpWin();
	virtual void handleAction(MonitorStruct* pMonitor, const String& filename, unsigned long action);
	virtual std::thread& startMonitoring(const String& dir);
	virtual void notifyStop(); 
protected:
	virtual void handleFileAction(const String& dir, const String& filename, FILE_EVENTS action);
	void MonitoringThread(const String& dir);
	void cleanUpStopEvent(); 
	HANDLE m_stopEvent; 
	//Queue<LogMessagePtr> loggingQueue; 





};
#endif //__FILE_MONITOR_LISTENR_IMP_H_
