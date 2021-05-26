#ifndef __EVENTSENUM_H_
#define __EVENTSENUM_H_
#pragma once
#include <string>
enum FILE_EVENTS {
	RENAMED_NEW_NAME, 
	RENAMED_OLD_NAME,
	CREATED,
	DELETED,
	CHANGED,
	UNKNOWN
};

static const char* enum_str[] = { "RENAMED_NEW_NAME", "RENAMED_OLD_NAME", "CREATED", "DELETED", "CHANGED", "UNKNOWN" };
std::string fileEventsToString(FILE_EVENTS fileEvent);
#endif //__EVENTSENUM_H_
