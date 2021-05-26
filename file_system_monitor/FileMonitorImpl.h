#ifndef __FILE_MONITOR_IMPL_H_
#define __FILE_MONITOR_IMPL_H_
#include "FileMonitor.h"
#include "EventsEnum.h"

#define WIN_PLATFORM 1
#define LINUX_PLATFORM 2
#define KQUEUE_PLATFORM 2
#if defined(_WIN32) 
#	define FILE_MONITOR_PLATFORM WIN_PLATFORM
#elif defined(__APPLE_CC__) || defined(BSD)
#	define FILE_MONITOR_PLATFORM KQUEUE_PLATFORM
#elif defined(__linux__) 
#	define FILE_MONITOR_PLATFORM LINUX_PLATFORM
#endif 
#include <map>
#include <thread>

struct MonitorStruct; //Forward declaration - each implementation will define it separetly 
class FileMonitorImpl {
public:
	typedef std::map<ListenerID, std::thread*> monitorThreadMap;
	typedef std::map<ListenerID, std::shared_ptr<FileMonitorListener>> monitorListenersMap;
	FileMonitorImpl();
	virtual ~FileMonitorImpl();
	//Add Monitor Listener 
	//throws FileNotFound Exception is the file doesn't exists 
	virtual ListenerID addFileMonitorListener(String& directory) = 0;
	//Remove Listner according to ListenerID
	virtual void removeListener(ListenerID listenerId) = 0;
protected:
	monitorThreadMap m_ThreadMonitors;
	monitorListenersMap m_monitors; 
	ListenerID lastListenerId; 
};


/*struct 
class */

#pragma once
#endif //__FILE_MONITOR_IMPL_H_