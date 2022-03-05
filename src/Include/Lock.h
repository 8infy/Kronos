#pragma once

#include <Types.h>


int SpinTryLock(spinlock_t *lock);

void SpinLock(spinlock_t *lock);

void SpinUnlock(spinlock_t *lock);
