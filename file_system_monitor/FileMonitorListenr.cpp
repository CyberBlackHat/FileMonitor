#include <stdio.h>
#include <time.h>
#include <sstream>
#include "FileMonitorListener.h"




FileMonitorListener::FileMonitorListener()
	:m_fileLogger(FileLogger::getInstance()), m_stopNow(false), m_pMonitoringThread(nullptr) {}
FileMonitorListener::~FileMonitorListener() { 
	if (nullptr == m_pMonitoringThread) {
		m_pMonitoringThread->join();
	}
}
void FileMonitorListener::handleFileLogging(const String& filename, FILE_EVENTS fileEvent) {
	std::stringstream strstream;
	preparePreleminaryLogMessage(strstream);
	std::string dirStdstringRep(filename.begin(), filename.end());
	strstream << "File Operation: file " << dirStdstringRep << " has been " << fileEventsToString(fileEvent);
	if (FILE_EVENTS::CHANGED == fileEvent) {
		//Calculate md5 hash of the file
		unsigned long status = 0; 
		std::string md5String =  calculate_md5_hash_of_file(filename, status);
		if (0 == status) {
			strstream << " new file MD5 hash " << md5String.c_str(); 
		}
		else {
			strstream << "Could't calculate md5 hash: error " << status; 
		}
		//Checking if file is encrypted:  //Consider doing it async 
		std::cout << "Please patience, entropy calculation on big files takes a while...\n";
		double entropy_measure = m_shannonEncryptChecker.get_file_entropy(dirStdstringRep);
		int64_t file_size = fileSize(dirStdstringRep);
		entropy::ShannonEncryptionChecker::InformationEntropyEstimation entropy_estimation =
			m_shannonEncryptChecker.information_entropy_estimation(entropy_measure, file_size);
		std::string description = m_shannonEncryptChecker.get_information_description(entropy_estimation); ;
		strstream << " , the file seems to be " << description << " file"; 

	}
	m_fileLogger.Log(strstream.str().c_str());

}

void FileMonitorListener::handleDeletedOrRenamed(const String& fileName, FILE_EVENTS fileEvent) {
	std::stringstream strstream;
	preparePreleminaryLogMessage(strstream); 
	strstream << std::string(fileName.begin(), fileName.end()) << " has been " << fileEventsToString(fileEvent);
	m_fileLogger.Log(strstream.str().c_str());
}
void FileMonitorListener::handleDirectoryLogging(const String& dirname, FILE_EVENTS fileEvent) {
	std::stringstream strstream;
	preparePreleminaryLogMessage(strstream);
	strstream << "Directory Operation: directory " << std::string(dirname.begin(), dirname.end()) << " has been " << fileEventsToString(fileEvent); 
	m_fileLogger.Log(strstream.str().c_str());
}

void FileMonitorListener::preparePreleminaryLogMessage(std::stringstream& strstream) {
	strstream << createReadableTimeStamp().c_str() << " " << ::_getpid() << ":" << std::this_thread::get_id() << " ";
}