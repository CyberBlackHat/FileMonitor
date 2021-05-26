#ifndef __LOG_MSG_H_
#define __LOG_MSG_H_
#include "CommonDefs.h"
#include "EventsEnum.h"

struct LogMessage {
	bool isDirectory;
	String directoryName;
	String fileName;
	FILE_EVENTS action;
	String hashValue;
	bool isEncrypted;
};
#pragma once
#endif //__LOG_MSG_H_
