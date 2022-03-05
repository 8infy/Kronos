#include <UltraProto.h>
#include <Format.h>
#include <Macros.h>
#include <String.h>
#include <Types.h>
#include <Print.h>
#include <Ports.h>
#include <Lock.h>

#define ANSI_COLOR

#define COM1 0x3F8


enum LogCaps
{
	LOGCAP_NONE     = 0,
	LOGCAP_OUT_E9   = 1,
	LOGCAP_SERIAL   = 2,
	LOGCAP_GFX_TERM = 4,
	LOGCAP_SYMMAP   = 8,
};

struct KSymEntry
{
	uint64_t addr;
	uint64_t  len;
	char   name[];
} PACKED;


static struct KSymEntry *ksym = NULL;

static size_t ksym_size = 0;

static enum LogCaps log_caps = LOGCAP_NONE;

static spinlock_t print_lock = 0;

static const char *log_messages[] = {
	"        ",
#ifdef ANSI_COLOR
	"\x1B[32;1m[INFO]\x1B[0m  ",
	"\x1B[33;1m[WARN]\x1B[0m  ",
	"\x1B[31;1m[ERROR]\x1B[0m ",
	"\x1B[34;1m[TRACE]\x1B[0m ",
#else
	"[INFO]  ",
	"[WARN]  ",
	"[ERROR] ",
	"[TRACE] ",
#endif
};


static const char *KSymFind(uintptr_t address)
{
	if(!(log_caps & LOGCAP_SYMMAP))
		return "?";

	struct KSymEntry *ent  = ksym;
	struct KSymEntry *last = ksym;

	while((uintptr_t) ent < (uintptr_t) ksym + ksym_size) {
		if(address >= last->addr && address <= ent->addr)
			return last->name;

		last = ent;
		ent = (struct KSymEntry *) (&ent->name[ent->len + 1]);
	}

	return "?";
}

static void SerialInit()
{
	Out8(COM1 + 1, 0x00);
	Out8(COM1 + 3, 0x80);
	Out8(COM1 + 0, 0x03);
	Out8(COM1 + 1, 0x00);
	Out8(COM1 + 3, 0x03);
	Out8(COM1 + 2, 0xC7);
	Out8(COM1 + 4, 0x0B);
	Out8(COM1 + 4, 0x1E);
	Out8(COM1 + 0, 0xAE);

	if(In8(COM1 + 0) != 0xAE)
		Panic("Serial is fucked\n");

	Out8(COM1 + 4, 0x0F);

	log_caps |= LOGCAP_SERIAL;
}

static void LogOut(const char *buf, size_t len)
{
	if(log_caps & LOGCAP_OUT_E9) {
		for(size_t i = 0; i < len; i++)
			Out8(0xE9, buf[i]);
	}
	if(log_caps & LOGCAP_SERIAL) {
		for(size_t i = 0; i < len; i++) {
			while((In8(COM1 + 5) & 0x20) == 0) asm volatile("nop");

			Out8(COM1, buf[i]);
		}
	}
}


void LogInit(struct ultra_module_info_attribute *kmap)
{
	if(kmap != NULL) {
		log_caps |= LOGCAP_SYMMAP;
		ksym = (struct KSymEntry *) kmap->address;
		ksym_size = kmap->size;
	}

	log_caps |= In8(0xE9) == 0xE9 ? LOGCAP_OUT_E9 : 0;

	SerialInit();

	if(log_caps & LOGCAP_SERIAL)
		log_caps &= ~LOGCAP_OUT_E9;

	Info("%xl\n", ksym);
}

void Print(int level, const char *fmt, ...)
{
	SpinLock(&print_lock);

	char buf[128] = { 0 };

	va_list ap;
	va_start(ap, fmt);

	size_t len = Format(buf, 124, fmt, ap);

	va_end(ap);

	if(len >= 124) {
		memcpy(&buf[124], "...", 4);
		len = 128;
	}

#ifdef ANSI_COLOR
	LogOut(log_messages[level], level == 0 ? 8 : 20);
#else
	LogOut(log_messages[level], 8);
#endif

	LogOut(buf, len);

	SpinUnlock(&print_lock);
}

void Put(const char *fmt, ...)
{
	SpinLock(&print_lock);

	char buf[128] = { 0 };

	va_list ap;
	va_start(ap, fmt);

	size_t len = min(Format(buf, 127, fmt, ap), 128);

	va_end(ap);

	LogOut(buf, len);

	SpinUnlock(&print_lock);
}

void Panic(const char *fmt, ...)
{
#ifdef ANSI_COLOR
	Put("\x1B[34;1m#!# \x1B[31;1mKernel Panic: \x1B[36m");
#else
	Put("#!# Kernel Panic: ");
#endif

	{
		char buf[128] = { 0 };

		va_list ap;
		va_start(ap, fmt);

		size_t len = min(Format(buf, 127, fmt, ap), 128);

		va_end(ap);

		LogOut(buf, len);
	}

#ifdef ANSI_COLOR
	Put(" \x1B[34;1m#!#\n\x1B[0m");
#else
	Put(" #!#\n");
#endif

	{
		size_t depth    = 8;
		uintptr_t *base = NULL;
		asm volatile("movq %%rbp, %0" : "=r"(base));

		while(--depth && base != NULL && base[1] >= KRNL_BASE) {
			Put(":  %xl <%s>\n", base[1], KSymFind(base[1]));

			base = (uintptr_t *) base[0];
		}
	}


	hang();
}
