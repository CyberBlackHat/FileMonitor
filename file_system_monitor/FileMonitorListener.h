#ifndef __FILE__MONITOR__LISTENR_H_
#define __FILE__MONITOR__LISTENR_H_
#pragma once
#include <thread>
#include "CommonDefs.h"
#include "EventsEnum.h"
#include "Logger.h"
#include "ShannonEntropy.h"
struct MonitorStruct; 
class FileMonitorListener {
public:
	FileMonitorListener();
	virtual ~FileMonitorListener();
	virtual void handleAction(MonitorStruct* pMonitor, const String& filename, unsigned long action) = 0;
	virtual std::thread& startMonitoring(const String& dir) = 0;
	virtual void handleFileLogging(const String& filename, FILE_EVENTS fileEvent);
	virtual void handleDirectoryLogging(const String& dirname, FILE_EVENTS fileEvent);
	virtual void handleDeletedOrRenamed(const String& fileName, FILE_EVENTS fileEvent);
	virtual void notifyStop() {
		m_stopNow = true;
	}
	bool getStopState() {
		return m_stopNow;
	}

protected:
	virtual void handleFileAction(const String& dir, const String& filename, FILE_EVENTS action) = 0;
	virtual void preparePreleminaryLogMessage(std::stringstream& strstream);
	std::thread* m_pMonitoringThread; 
	FileLogger& m_fileLogger;
	bool m_stopNow;
	entropy::ShannonEncryptionChecker m_shannonEncryptChecker; 
};

#endif //__FILE__MONITOR__LISTENR_H_