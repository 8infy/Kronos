#include <Format.h>
#include <Macros.h>
#include <String.h>
#include <Types.h>
#include <Print.h>
#include <Ports.h>


enum LogCaps
{
	LOGCAP_NONE     = 0,
	LOGCAP_OUT_E9   = 1,
	LOGCAP_SERIAL   = 2,
	LOGCAP_GFX_TERM = 4,
};

static enum LogCaps log_caps = LOGCAP_NONE;

static const char *log_messages[] = {
	"        ",
	"\x1B[32;1m[INFO]\x1B[0m  ",
	"\x1B[33;1m[WARN]\x1B[0m  ",
	"\x1B[31;1m[ERROR]\x1B[0m ",
	"\x1B[34;1m[TRACE]\x1B[0m ",
};


static void LogOut(const char *buf, size_t len)
{
	if(log_caps & LOGCAP_OUT_E9) {
		for(size_t i = 0; i < len; i++)
			Out8(0xE9, buf[i]);
	}
}


void LogInit()
{
	log_caps |= In8(0xE9) == 0xE9 ? LOGCAP_OUT_E9 : 0;
}

void Print(int level, const char *fmt, ...)
{
	char buf[128] = { 0 };

	va_list ap;
	va_start(ap, fmt);

	size_t len = Format(buf, 124, fmt, ap);

	va_end(ap);

	if(len >= 124) {
		memcpy(&buf[124], "...", 4);
		len = 128;
	}

	LogOut(log_messages[level], level == 0 ? 8 : 20);
	LogOut(buf, len);
}

void Put(const char *fmt, ...)
{
	char buf[128] = { 0 };

	va_list ap;
	va_start(ap, fmt);

	size_t len = min(Format(buf, 127, fmt, ap), 128);

	va_end(ap);

	LogOut(buf, len);
}

void Panic(const char *fmt, ...)
{
	Put("\x1B[34;1m#!# \x1B[31;1mKernel Panic: \x1B[34m");

	{
		char buf[128] = { 0 };

		va_list ap;
		va_start(ap, fmt);

		size_t len = min(Format(buf, 127, fmt, ap), 128);

		va_end(ap);

		LogOut(buf, len);
	}

	Put("\x1B[34;1m#!#\n");
}
