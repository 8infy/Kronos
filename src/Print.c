#include <Format.h>
#include <Macros.h>
#include <Types.h>
#include <Queue.h>
#include <Print.h>
#include <Ports.h>


static struct Log logs[80] = { 0 };

static struct Queue log_queue = (struct Queue) { logs, sizeof(struct Log), 80, 0, 0 };

static const char *log_messages[4] =
{
	"\x1B[32;1m[INFO]\x1B[0m  ",
	"\x1B[33;1m[WARN]\x1B[0m  ",
	"\x1B[31;1m[ERROR]\x1B[0m ",
	"\x1B[34;1m[TRACE]\x1B[0m "
};


static void PrintBuffer(const char *buf, size_t len)
{
	if(In8(0xE9) == 0xE9) {
		for(size_t i = 0; i < len; i++)
			Out8(0xE9, buf[i]);
	}
}

static void PrintLog(struct Log *log)
{
	PrintBuffer(log_messages[log->type], 20);
	PrintBuffer(log->msg, log->len);
}

void Log(struct Log *log)
{
	if(QueueFull(&log_queue))
		QueueConsume(&log_queue, NULL);

	QueueSubmit(&log_queue, log);

	PrintLog(log);
}

void KPrint(int lvl, const char *fmt, ...)
{
	struct Log log = (struct Log) { 0 };

	log.type = lvl;

	va_list ap;
	va_start(ap, fmt);

	log.len = Format(log.msg, LOG_MSG_SIZE, fmt, ap);

	va_end(ap);

	Log(&log);
}
