#pragma once

#include <Format.h>


#define LOG_NONE  0
#define LOG_INFO  1
#define LOG_WARN  2
#define LOG_ERROR 3
#define LOG_TRACE 4

#define Info(...) Print(LOG_INFO, __VA_ARGS__)
#define Warn(...) Print(LOG_WARN, __VA_ARGS__)
#define Error(...) Print(LOG_ERROR, __VA_ARGS__)
#define Trace(...) Print(LOG_TRACE, __VA_ARGS__)


void Print(int level, const char *fmt, ...);

void Put(const char *fmt, ...);

void Panic(const char *fmt, ...);
