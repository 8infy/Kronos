#include <Format.h>
#include <Macros.h>
#include <Queue.h>
#include <Print.h>
#include <Ports.h>


static struct Log logs[2000] = { 0 };

static struct Queue log_queue = (struct Queue) { NULL, sizeof(struct Log), 2000, 0, 0 };

static const char *log_messages[4] =
{
	"\x1B[32;1m[INFO]\x1B[0m  ",
	"\x1B[33;1m[WARN]\x1B[0m  ",
	"\x1B[31;1m[ERROR]\x1B[0m ",
	"\x1B[34;1m[TRACE]\x1B[0m "
};


static void PrintBuffer(const char *buf, size_t len)
{
#ifdef OUT_E9
	while(len--)
		Out8(0xE9, *(buf++));
#endif
}

static void FlushLogs()
{
	struct Log log;
	int r = QueueConsume(&log_queue, &log);

	do {
		PrintBuffer(log_messages[log.type], 20);
		PrintBuffer(log.msg, log.len);

		r = QueueConsume(&log_queue, &log);
	} while(r);
}

void Log(struct Log *log)
{
	if(QueueFull(&log_queue))
		QueueConsume(&log_queue, NULL);

	QueueSubmit(&log_queue, log);

	FlushLogs();
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
