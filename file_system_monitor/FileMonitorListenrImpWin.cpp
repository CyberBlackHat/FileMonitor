#include "FileMonitorListenerImpWin.h"
#include "LogMsg.h"
#include "MonitorStruct.h"
#include <sstream>
#include <Shlwapi.h>

std::wstring getLastErrorFormattedString() {
	wchar_t buf[256];
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buf, (sizeof(buf) / sizeof(wchar_t)), NULL);
	return std::wstring(buf); 
}


void CALLBACK FileCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);

bool RefreshMonitor(MonitorStruct* pMonitor, bool clear = false) {
	return ReadDirectoryChangesW(
		pMonitor->m_dirHandle, pMonitor->m_buffer, sizeof(pMonitor->m_buffer), TRUE,
		pMonitor->m_notifyFilter, NULL, &pMonitor->m_overlapped, clear ? nullptr : FileCompletionRoutine) != 0;
}

/// Starts monitoring a directory
MonitorStruct* CreateMonitor(LPCTSTR szDirectory, DWORD dwNotifyFilter)
{
	MonitorStruct* pMonitor;
	size_t ptrsize = sizeof(*pMonitor);
	pMonitor = static_cast<MonitorStruct*>(::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ptrsize));
	if (!pMonitor) {
		return pMonitor; 
	}
	pMonitor->m_dirHandle = CreateFile(szDirectory, FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
		OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED |
		FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME /*| FILE_NOTIFY_CHANGE_SIZE*/, NULL);

	if (INVALID_HANDLE_VALUE != pMonitor->m_dirHandle)
	{
		pMonitor->m_overlapped.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		pMonitor->m_notifyFilter = dwNotifyFilter;
		if (RefreshMonitor(pMonitor))
		{
			return pMonitor;
		}
		else
		{
			if(0 != pMonitor->m_overlapped.hEvent) 
				::CloseHandle(pMonitor->m_overlapped.hEvent);
			if(0 != pMonitor->m_dirHandle)
				::CloseHandle(pMonitor->m_dirHandle);
		}
	}

	::HeapFree(GetProcessHeap(), 0, pMonitor);
	return NULL;
}


void DestroyMonitor(MonitorStruct* pMonitor) {
	if (pMonitor)
	{
		pMonitor->m_FileMonitorListener->notifyStop();

		CancelIo(pMonitor->m_dirHandle);

		RefreshMonitor(pMonitor, true);

		if (!HasOverlappedIoCompleted(&pMonitor->m_overlapped))
		{
			::SleepEx(5, TRUE);
		}

		::CloseHandle(pMonitor->m_overlapped.hEvent);
		::CloseHandle(pMonitor->m_dirHandle);
		delete[] pMonitor->m_dirName;
		::HeapFree(::GetProcessHeap(), 0, pMonitor);
	}
}



void CALLBACK FileCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
	TCHAR szFile[MAX_PATH];
	PFILE_NOTIFY_INFORMATION pNotify;
	MonitorStruct* pMonitor = (MonitorStruct*)lpOverlapped;
	size_t offset = 0;

	if (dwNumberOfBytesTransfered == 0)
		return;

	if (dwErrorCode == ERROR_SUCCESS)
	{
		do
		{
			pNotify = (PFILE_NOTIFY_INFORMATION)&pMonitor->m_buffer[offset];
			offset += pNotify->NextEntryOffset;

#			if defined(UNICODE)
			{
				lstrcpynW(szFile, pNotify->FileName,
					min(MAX_PATH, pNotify->FileNameLength / sizeof(WCHAR) + 1));
			}
#			else
			{
				int count = WideCharToMultiByte(CP_ACP, 0, pNotify->FileName,
					pNotify->FileNameLength / sizeof(WCHAR),
					szFile, MAX_PATH - 1, NULL, NULL);
				szFile[count] = TEXT('\0');
			}
#			endif

			pMonitor->m_FileMonitorListener->handleAction(pMonitor, szFile, pNotify->Action);

		} while (pNotify->NextEntryOffset != 0);
	}

	if (!pMonitor->m_FileMonitorListener->getStopState())
	{
		RefreshMonitor(pMonitor);
	}
}


FileMonitorListenerImpWin::FileMonitorListenerImpWin()
	:m_stopEvent(INVALID_HANDLE_VALUE) {}

FileMonitorListenerImpWin::~FileMonitorListenerImpWin() {
	if (m_pMonitoringThread->joinable()) { 
		m_pMonitoringThread->join();
	}
	delete m_pMonitoringThread; 
}

void FileMonitorListenerImpWin::handleAction(MonitorStruct* pMonitor, const String& filename, unsigned long action) {

	FILE_EVENTS fileEvent;

	switch (action)
	{
	case FILE_ACTION_RENAMED_NEW_NAME:
		fileEvent = FILE_EVENTS::RENAMED_NEW_NAME;
		break;
	case FILE_ACTION_ADDED:
		fileEvent = FILE_EVENTS::CREATED;
		break;
	case FILE_ACTION_RENAMED_OLD_NAME:
		fileEvent = FILE_EVENTS::RENAMED_OLD_NAME;
		break;
	case FILE_ACTION_REMOVED:
		fileEvent = FILE_EVENTS::DELETED;
		break;
	case FILE_ACTION_MODIFIED:
		fileEvent = FILE_EVENTS::CHANGED;
		break;
	default:
		fileEvent = FILE_EVENTS::UNKNOWN;

	};
	handleFileAction(pMonitor->m_dirName, filename, fileEvent);

}

void FileMonitorListenerImpWin::handleFileAction(const String& dir, const String& filename, FILE_EVENTS action) {
	static TCHAR szFile[MAX_PATH] = { 0 };
	::ZeroMemory(szFile, sizeof(szFile));
	::PathCombine(szFile, dir.c_str(), filename.c_str());
	if (FILE_EVENTS::RENAMED_OLD_NAME == action || FILE_EVENTS::DELETED == action) {
		handleDeletedOrRenamed(String(szFile), action); 
	} 
	else {
		DWORD dwAttrs = ::GetFileAttributes(szFile);
		if (INVALID_FILE_ATTRIBUTES == dwAttrs) {
			//TODO - log error message
			return;
		}
		if (dwAttrs & FILE_ATTRIBUTE_DIRECTORY) {
			handleDirectoryLogging(szFile, action);
		}
		else {
			handleFileLogging(szFile, action);
		}
	}
	
	
}



std::thread& FileMonitorListenerImpWin::startMonitoring( const String& dir) {
	m_pMonitoringThread = new std::thread(&FileMonitorListenerImpWin::MonitoringThread, this, dir);
	return *m_pMonitoringThread;
}

void FileMonitorListenerImpWin::notifyStop() {
	FileMonitorListener::notifyStop();
	if (INVALID_HANDLE_VALUE != m_stopEvent &&
		NULL != m_stopEvent) {
		::SetEvent(m_stopEvent);
		cleanUpStopEvent();
	}

}

void FileMonitorListenerImpWin::cleanUpStopEvent() {
	::CloseHandle(m_stopEvent);
	m_stopEvent = INVALID_HANDLE_VALUE;
}
void FileMonitorListenerImpWin::MonitoringThread(const String& directory) {
	MonitorStruct*  pMonitorStruct = CreateMonitor(directory.c_str(), FILE_NOTIFY_CHANGE_FILE_NAME| FILE_NOTIFY_CHANGE_DIR_NAME| FILE_NOTIFY_CHANGE_ATTRIBUTES| FILE_NOTIFY_CHANGE_SIZE| FILE_NOTIFY_CHANGE_LAST_WRITE| FILE_NOTIFY_CHANGE_CREATION| FILE_NOTIFY_CHANGE_FILE_NAME );
	if (!pMonitorStruct)
		throw FileNotFoundException(directory);
	pMonitorStruct->m_FileMonitorListener = this;
	pMonitorStruct->m_dirName = new TCHAR[directory.length() + 1];
	std::memcpy(pMonitorStruct->m_dirName, directory.c_str(), (directory.length() + 1) * sizeof(TCHAR));
	do {
		m_stopEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		if (NULL == m_stopEvent) {
			std::cout << "Error at creation of stop event for directory " << std::string(directory.begin(), directory.end()) << " not monitoring this directory";
			break;
		}
		HANDLE handles[] = { m_stopEvent };
		while (false == getStopState()) {
			DWORD dwWaitResult = ::MsgWaitForMultipleObjectsEx(1, handles, INFINITE, QS_ALLINPUT, MWMO_ALERTABLE);
			switch (dwWaitResult) {
				case WAIT_OBJECT_0: //Stop Event 
					break;
				case WAIT_IO_COMPLETION:
					continue;
				case WAIT_TIMEOUT:
					std::cout << "Fatal error: wait timeout for MsgWaitForMultipleObjectsEx" << std::endl;
				case WAIT_ABANDONED_0: 
					std::cout << "Fatal error: wait abandoneted for MsgWaitForMultipleObjectsEx" << std::endl;
				case WAIT_FAILED:
					std::cout << "Fatal error: wait failed for MsgWaitForMultipleObjectsEx for reason:";
					std::cout << getLastErrorFormattedString().c_str() << std::endl;
					break;
					


			}
		}
	} while (false); 
	//Cleanup code
	if (INVALID_HANDLE_VALUE != m_stopEvent &&
		NULL != m_stopEvent) {
		cleanUpStopEvent(); 
		DestroyMonitor(pMonitorStruct);
		
	}
}



 