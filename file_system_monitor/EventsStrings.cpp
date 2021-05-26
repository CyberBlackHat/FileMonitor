#include "EventsEnum.h"
std::string fileEventsToString(FILE_EVENTS fileEvent) {
	return std::string(enum_str[fileEvent]);
}