#include <time.h>
#include <fstream>
#include "CommonDefs.h"

static std::string createTimeStamp(const char* frmt) {
	char s[1000];

	time_t t = time(NULL);
	struct tm* p = localtime(&t);

	strftime(s, 1000, frmt, p);
	return std::string(s);
}
std::string createReadableTimeStamp() {
	return createTimeStamp("%F:%T"); 
}

std::string createFileTimeStamp() {
	return createTimeStamp("%F_%H%M%S");
}

int64_t fileSize(const std::string& file_path) {
	std::ifstream in(file_path, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}
