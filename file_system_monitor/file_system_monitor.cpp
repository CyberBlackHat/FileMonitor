// file_system_monitor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <vector>
#include <Tchar.h>
#include "CommonDefs.h"
#include <windows.h>
#include "FileMonitorImpWin.h"
#include <tchar.h>
#include <string>
#include <codecvt>
#include <winnt.h>
#include <locale>
#include <algorithm>

std::vector<ListenerID> listenerIdVector;
HANDLE notifyHandle = 0; 
BOOL WINAPI consoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        printf("Ctrl-C handled\n"); // do cleanup
        ::SetEvent(notifyHandle); //Assuming the handle is v alid - Check already at main 

    }
    

    return TRUE;
} 

int main(int argc, char** argv)
{
    notifyHandle = ::CreateEvent(NULL, TRUE, FALSE, NULL); 
    
        if (INVALID_HANDLE_VALUE == notifyHandle) {
            std::cout << "\nError creating stop event, aborting ... " << std::endl;
            return 1;
        }
        if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
            std::cout << "\nERROR: Could not set control handler" << std::endl;
            return 2;
        }
        std::unique_ptr<FileMonitorImpWin> pFileMonitor(new FileMonitorImpWin());
        //Assuming input is valid directory names - otherwise there is need to add validation code 
        for (int idx = 1; idx < argc; ++idx) {
            String DirectoryName(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(argv[idx]));
            listenerIdVector.push_back(pFileMonitor->addFileMonitorListener(DirectoryName));

        }
        std::cout << "\n Press Ctrl + C to stop monitoring" << std::endl;
        DWORD dwWaitResult = ::WaitForSingleObject(notifyHandle, INFINITE);
        switch (dwWaitResult) {
        case WAIT_OBJECT_0:
            std::cout << "Stop monitoring cleanup ..." << std::endl;
            std::for_each(listenerIdVector.begin(), listenerIdVector.end(),
                [&](ListenerID listenerid) {
                    pFileMonitor->removeListener(listenerid);
                });
            ::CloseHandle(notifyHandle);
            break;
        default:
            std::cout << " Error waiting for stop notify " << ::GetLastError() << std::endl;
            std::cout << "Aborting ... " << std::endl;
            ::CloseHandle(notifyHandle);
            abort();

        }

   ::CloseHandle(notifyHandle);
   
    
   
    



}
