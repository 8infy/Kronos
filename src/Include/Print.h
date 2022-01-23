#pragma once

#include <Format.h>


#define LOG_MSG_SIZE 80

#define LOG_INFO  0
#define LOG_WARN  1
#define LOG_ERROR 2
#define LOG_TRACE 3

#define Info(...) KPrint(LOG_INFO, __VA_ARGS__)
#define Warn(...) KPrint(LOG_WARN, __VA_ARGS__)
#define Error(...) KPrint(LOG_ERROR, __VA_ARGS__)
#define Trace(...) KPrint(LOG_TRACE, __VA_ARGS__)


struct Log
{
	char    msg[LOG_MSG_SIZE];

	size_t  len : 62;
	size_t type :  2;
};


void Log(struct Log *log);

void KPrint(int level, const char *fmt, ...);

void KPut(const char *fmt, ...);
