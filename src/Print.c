#include <Format.h>
#include <Macros.h>
#include <Queue.h>
#include <Print.h>


static struct Log logs[2000] = { 0 };

static struct Queue log_queue = (struct Queue) { NULL, sizeof(struct Log), 2000, 0, 0 };


void Log(struct Log *log)
{
	if(QueueFull(&log_queue))
		QueueConsume(&log_queue, NULL);

	QueueSubmit(&log_queue, log);
}

void KPrint(int lvl, const char *fmt, ...)
{
	struct Log log = (struct Log) { 0 };

	log.type = lvl;
	log.time = 0 /* TODO: Put actual time here */;

	va_list ap;
	va_start(ap, fmt);

	log.len = Format(log.msg, LOG_MSG_SIZE, fmt, ap);

	va_end(ap);

	Log(&log);
}
