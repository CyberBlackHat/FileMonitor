#include "Logger.h"
#include "CommonDefs.h"
const char* FileLogger::file_prefix = "DirecoryMonitoring_"; 

FileLogger& FileLogger::getInstance() {
	
	/*
	* No need to worry about Double lock checking at all 
	* The C++11 standard’s got our back in §6.7.4:
	* If control enters the declaration concurrently while the variable is being initialized, the concurrent execution shall wait for completion of the initialization.
	*/
	static FileLogger gInstance; 
	return gInstance; 

}
FileLogger::FileLogger() {
	
	const std::string timestamp = createFileTimeStamp();
	const std::string fileName = std::string(file_prefix) + timestamp + std::string(".log"); 
	loggerFile.open(fileName.c_str(), std::ofstream::out | std::ofstream::app); 
	
}
FileLogger::~FileLogger() {
	loggerFile.close(); 

}
void FileLogger::Log(const char* logMessage) {
	const std::lock_guard<std::mutex> lock(loggerFileMutext);
	loggerFile << logMessage << std::endl;
}

