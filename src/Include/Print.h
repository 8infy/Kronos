#pragma once

#include <Format.h>


#define LOG_MSG_SIZE 80

#define LOG_NONE  0
#define LOG_INFO  1
#define LOG_WARN  2
#define LOG_ERROR 3

struct Log
{
	char    msg[LOG_MSG_SIZE];

	size_t  len : 62;
	size_t type :  2;

	size_t time;
};


void Log(struct Log *log);

void KPrint(const char *fmt, ...);

void KPut(const char *fmt, ...);

void Info(const char *fmt, ...);

void Warn(const char *fmt, ...);

void Error(const char *fmt, ...);
