#ifndef __FILEMONITOR_H_
#define __FILEMONITOR_H_
#pragma once
#include "CommonDefs.h"

class FileMonitorImpl;
class FileMonitorListener;
class FileMonitor {
public:
	FileMonitor(); 
	virtual ~FileMonitor();
	virtual ListenerID addFileMonitorListener(String& directory);
	virtual void removeListener(ListenerID listenerId); 
	
protected:
	/// The implementation
	FileMonitorImpl* mImpl;
	virtual void startMonitoring() = 0;
};

#endif
