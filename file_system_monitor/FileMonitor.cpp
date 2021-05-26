#include "FileMonitor.h"
#include "FileMonitorImpl.h"
#if FILE_MONITOR_PLATFORM == WIN_PLATFORM
#include "FileMonitorImpWin.h"
#define MONITORIMP FileMonitorImpWin
#elif FILE_MONITOR_PLATFORM == LINUX_PLATFORM
#include "FileMonitorImpLinux.h"
#define MONITORIMP FileMonitorImpLinux
#elif  FILE_MONITOR_PLATFORM == KQUEUE_PLATFORM
#include "FileMonitorImpKqueue.h"
#define MONITORIMP FileMonitorImpKqueue
#endif

FileMonitor::FileMonitor()
{
	mImpl = new MONITORIMP(); 
}

FileMonitor::~FileMonitor()
{
	delete mImpl;
	mImpl = nullptr; 
}


ListenerID FileMonitor::addFileMonitorListener(String& directory)
{
	return mImpl->addFileMonitorListener(directory); 
}

void FileMonitor::removeListener(ListenerID listenerId)
{
	mImpl->removeListener(listenerId);
}

