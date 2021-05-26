#ifndef __FILE_MONITOR_IMP_WIN_H_
#define __FILE_MONITOR_IMP_WIN_H_
#pragma once
#include <windows.h>
#include "FileMonitorImpl.h"
#if FILE_MONITOR_PLATFORM == WIN_PLATFORM
class FileMonitorImpWin: public FileMonitorImpl {
public:
	FileMonitorImpWin();
	virtual ~FileMonitorImpWin();
	//Add Monitor Listener 
	//throws FileNotFound Exception is the file doesn't exists 
	virtual ListenerID addFileMonitorListener(String& directory);
	//Remove Listner according to ListenerID
	virtual void removeListener(ListenerID listenerId);
	
};
#endif //FILE_MONITOR_PLATFORM == WIN_PLATFORM
#endif //__FILE_MONITOR_IMP_WIN_H_