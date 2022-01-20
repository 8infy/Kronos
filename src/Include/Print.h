#pragma once

#include <Format.h>


#define LOG_MSG_SIZE 80

#define LOG_INFO  0
#define LOG_WARN  1
#define LOG_ERROR 2
#define LOG_TRACE 3

#define Info(fmt, ...) KPrint(LOG_INFO, fmt, __VA_ARGS__)
#define Warn(fmt, ...) KPrint(LOG_WARN, fmt, __VA_ARGS__)
#define Error(fmt, ...) KPrint(LOG_ERROR, fmt, __VA_ARGS__)
#define Trace(fmt, ...) KPrint(LOG_TRACE, fmt, __VA_ARGS__)


struct Log
{
	char    msg[LOG_MSG_SIZE];

	size_t  len : 62;
	size_t type :  2;

	size_t time;
};


void Log(struct Log *log);

void KPrint(int level, const char *fmt, ...);

void KPut(const char *fmt, ...);
