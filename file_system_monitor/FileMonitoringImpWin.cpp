#include <windows.h>
#include "FileMonitorImpWin.h"
#include "FileMonitorListener.h"
#include "FileMonitorListenerImpWin.h"





FileMonitorImpWin::FileMonitorImpWin() {
}

FileMonitorImpWin::~FileMonitorImpWin() {
	for (const auto kv : m_monitors) {
		kv.second->notifyStop();
	}
	for (const auto lt : m_ThreadMonitors) {
		lt.second->join();
	}
	m_monitors.clear();
	m_ThreadMonitors.clear();
}

ListenerID FileMonitorImpWin::addFileMonitorListener(String& directory) {
	ListenerID listenerId = ++lastListenerId;
	std::shared_ptr<FileMonitorListener> fileMonitorListener(new FileMonitorListenerImpWin());
	std::thread& monitoringThread = fileMonitorListener->startMonitoring(directory);
	m_monitors[listenerId] = fileMonitorListener;
	m_ThreadMonitors[listenerId] = &monitoringThread; 
	return listenerId;
}

void FileMonitorImpWin::removeListener(ListenerID listenerId) {
	auto listenerItem = m_monitors.find(listenerId); 
	if (m_monitors.end() == listenerItem) {
		throw new Exception("listenerId not exists"); //TODO fix this message
	}
	listenerItem->second->notifyStop();
	auto listenerThreadItem = m_ThreadMonitors.find(listenerId);
	if (m_ThreadMonitors.end() == listenerThreadItem) {
		throw new Exception("thread record does not exists for listenerId");
	}
	if (listenerThreadItem->second->joinable()) {
		listenerThreadItem->second->join();
	}
	m_monitors.erase(listenerItem);
	m_ThreadMonitors.erase(listenerThreadItem);
}

