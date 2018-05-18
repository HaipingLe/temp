////////////////////////////////////////////////////////////////////////////////
///  @file MallocHelper.c
///  @brief Instruments malloc and free for test purpose.
///  @author extern.mike.werner@technisat.de
///  Copyright (c) TechniSat Digital GmbH,
////////////////////////////////////////////////////////////////////////////////

#include <malloc.h>

#define MALLOCHELPER_H_IMPL
#include "MallocHelper.h"

/**
 * Instrument malloc and free for testing memory de-/allocation
 */
bool captureMemoryStatistics = false;
size_t mallocCount = 0; /* number of times malloc was called */
size_t mallocSize = 0; /* number of bytes allocated */
size_t trueMallocSize = 0; /* number of bytes allocated actually (incl. malloc(0)) */
size_t freeCount = 0; /* number of times free was called */
size_t freeSize = 0; /* number of bytes freed
 */
void resetMemoryStatistics() {
	captureMemoryStatistics = true;
	mallocCount = 0;
	mallocSize = 0;
	freeCount = 0;
	freeSize = 0;
	trueMallocSize = 0;
}

void setCapturingMemoryStatisticsEnabled(bool value) {
	captureMemoryStatistics = value;
}

size_t allocatedMemory() {
	return mallocSize - freeSize;
}

size_t trueAllocatedMemory() {
	return trueMallocSize - freeSize;
}

void printMallocDiagnostics(const char* message){

	printf(
			"\n%s\nmallocCount=%u\nmallocSize=%u\ntrueMallocSize=%u\nfreeCount=%u\nfreeSize=%u\nallocatedMemory=%u\ntrueAllocatedMemory=%u\n",
			message, mallocCount, mallocSize, trueMallocSize, freeCount,
			freeSize, allocatedMemory(), trueAllocatedMemory());
}

/* Prototypes for our hooks.  */
static void my_init_hook(void);
static void *my_malloc_hook(size_t, const void *);
static void my_free_hook(void* ptr, const void *caller);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wunused-parameter"
/* Variables to save original hooks. */
static void *(*old_malloc_hook)(size_t, const void *caller);
static void (*old_free_hook)(void *ptr, const void *caller);

/* Override initializing hook from the C library. */
void (* volatile __malloc_initialize_hook)(void) = my_init_hook;

static void my_init_hook(void) {
	old_malloc_hook = __malloc_hook;
	__malloc_hook = my_malloc_hook;

	old_free_hook = __free_hook;
	__free_hook = my_free_hook;
}

static void * my_malloc_hook(size_t size, const void* caller) {
	void *result;

	if (captureMemoryStatistics == true) {
		mallocCount++;
		mallocSize += size;
	}

	/* Restore all old hooks */
	__malloc_hook = old_malloc_hook;
	__free_hook = old_free_hook;

	/* Call recursively */
	result = malloc(size);

	/* Save underlying hooks */
	old_malloc_hook = __malloc_hook;
	old_free_hook = __free_hook;

	/* printf might call malloc, so protect it too. */
	//printf ("malloc (%u) returns %p\n", (unsigned int) size, result);
	if (captureMemoryStatistics == true) {
		trueMallocSize += malloc_usable_size(result);
	}

	/* Restore our own hooks */
	__malloc_hook = my_malloc_hook;
	__free_hook = my_free_hook;

	return result;
}

static void my_free_hook(void* ptr, const void* caller) {

	if (captureMemoryStatistics == true && ptr != NULL) {
		freeCount++;
		freeSize += malloc_usable_size(ptr);
	}

	/* Restore all old hooks */
	__malloc_hook = old_malloc_hook;
	__free_hook = old_free_hook;

	/* Call recursively */
	free(ptr);

	/* Save underlying hooks */
	old_malloc_hook = __malloc_hook;
	old_free_hook = __free_hook;

	/* printf might call free, so protect it too. */
	//printf ("freed pointer %p\n", ptr);
	/* Restore our own hooks */
	__malloc_hook = my_malloc_hook;
	__free_hook = my_free_hook;
}

#pragma GCC diagnostic pop
