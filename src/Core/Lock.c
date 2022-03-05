#include <Types.h>
#include <Print.h>
#include <Core.h>
#include <Lock.h>

int SpinTryLock(spinlock_t *lock)
{
	uint64_t irq = (FlagsGet() >> 9) & 1;
	asm volatile("cli");

	spinlock_t exp = 0;
	spinlock_t des = 1ULL | (irq << 1);

	if(__atomic_compare_exchange_n(lock, &exp, des, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE))
		return 1;

	if(irq)
		asm volatile("sti");

	return 0;
}

void SpinLock(spinlock_t *lock)
{
	uint64_t irq = (FlagsGet() >> 9) & 1ULL;
	asm volatile("cli");

	spinlock_t exp = 0;
	spinlock_t des = 1ULL | (irq << 1);

	size_t timeout = 100000000ULL;

	while(!__atomic_compare_exchange_n(lock, &exp, des, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
		asm volatile("pause");
		exp = 0;

		if(--timeout == 0)
			Panic("Spinlock took too long (deadlocked?)");
	}
}

void SpinUnlock(spinlock_t *lock)
{
	spinlock_t state = __atomic_exchange_n(lock, 0, __ATOMIC_RELAXED);

	if((state >> 1) & 1)
		asm volatile("sti");
}
