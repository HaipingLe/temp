////////////////////////////////////////////////////////////////////////////////
///  @file MallocHelper.h
///  @brief Instruments malloc and free for test purpose.
///  @author extern.mike.werner@technisat.de
///  Copyright (c) TechniSat Digital GmbH,
////////////////////////////////////////////////////////////////////////////////
#ifndef MALLOCHELPER_H_
#define MALLOCHELPER_H_

#ifndef __cplusplus
typedef enum{
	false = 0,
	true = 1
} bool;
#endif

#ifndef MALLOCHELPER_H_IMPL
extern size_t mallocCount; /* number of times malloc was called */
extern size_t mallocSize; /* number of bytes allocated */
extern size_t trueMallocSize; /* number of bytes allocated actually (incl. malloc(0)) */
extern size_t freeCount; /* number of times free was called */
extern size_t freeSize; /* number of bytes freed */
#endif

#ifdef __cplusplus
extern "C" {
#endif

void resetMemoryStatistics();
void setCapturingMemoryStatisticsEnabled(bool value);
size_t allocatedMemory();
size_t trueAllocatedMemory();
void printMallocDiagnostics(const char* message);

#ifdef __cplusplus
}
#endif


#endif /* MALLOCHELPER_H_ */
