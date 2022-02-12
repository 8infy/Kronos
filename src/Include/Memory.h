#pragma once

#include <Types.h>


// System Memory Info
// All fields are in bytes
struct SMInfo
{
	size_t phys_total; // Total physical memory
	size_t  phys_free; // Free physical memory
	size_t  phys_used; // Used physical memory
	size_t  phys_rsvd; // Reserved physical memory
};

void SMInfoGet(struct SMInfo *info);


// Page allocation functions

uintptr_t PMAlloc(size_t npages);

void PMFree(uintptr_t addr);


void *VMAlloc(size_t npages);

void *VFree(void *ptr);


// Slab allocation functions

void *MAlloc(size_t n);

void *CAlloc(size_t nmemb, size_t size);

void Free(void *ptr);
