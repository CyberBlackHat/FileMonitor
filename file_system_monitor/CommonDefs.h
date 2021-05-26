#ifndef __COMMON_DEFS_H_
#define __COMMON_DEFS_H_ 
#pragma once
#include <string>
#include <stdexcept>

#if UNICODE
/// Type for a string
typedef std::wstring String;
#define SNPRINTF _snwprintf

#else
/// Type for a string
typedef std::string String;
#define SNPRINTF snprintf
#endif
typedef unsigned long ListenerID;
std::string createReadableTimeStamp(); 
std::string createFileTimeStamp();
class Exception : public std::runtime_error
{
public:
	Exception(const std::string& message)
		: std::runtime_error(message.c_str())
	{}
};

/// Exception thrown when a file is not found.
	/// @class FileNotFoundException
class FileNotFoundException : public Exception
{
public:
	FileNotFoundException()
		: Exception("File not found")
	{}

	FileNotFoundException(const String& filename)
		: Exception("File not found (" + std::string(filename.begin(), filename.end()) + ")")
	{}
};
std::string calculate_md5_hash_of_file(const String& fname, unsigned long& status); 
int64_t fileSize(const std::string& file_path); 
#endif //__COMMON_DEFS_H_
