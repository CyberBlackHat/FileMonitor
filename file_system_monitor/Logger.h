#ifndef __LOGGER_H_
#define __LOGGER_H_
#pragma once
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>


class FileLogger {
private:
	static const char*  file_prefix; 
	std::ofstream loggerFile; 
	std::mutex loggerFileMutext; 
	FileLogger();
public:
	static FileLogger& getInstance();
	~FileLogger();
	void Log(const char* logMessage);
	
};

#endif //__LOGGER_H_

