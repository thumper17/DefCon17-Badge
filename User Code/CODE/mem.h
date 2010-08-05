/** ###################################################################
**
**     (c) Freescale Semiconductor
**     2004 All Rights Reserved
**
**
**     (c) Copyright UNIS, a.s. 1997-2008
**     UNIS, a.s.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################*/

/* File: mem.h */

#ifndef __memory_H
#define __memory_H

#ifndef SDK_LIBRARY
  #include "configdefines.h"
#endif

#include "port.h"
#include <stdlib.h>
#include "mempx.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************
* Replace ANSI C memory management routines
*******************************************************/
#define malloc   memMallocEM
#define calloc   memCallocEM
#define realloc  memReallocEM
#define free     memFreeEM


/*******************************************************
* Internal memory management routines
*******************************************************/

/*
  Allocate internal data memory, if possible;
  otherwise, allocate external data memory
*/
EXPORT void *  memMallocIM (size_t size);

/*
  Reallocate internal data memory, if possible;
  otherwise, reallocate external data memory
*/
void * memReallocIM ( void *memblock, size_t size );

/*
  Allocate and clear internal data memory, if possible;
  otherwise, allocate and clear external data memory
*/
void *  memCallocIM (size_t num, size_t size);

/*
  Allocate internal data memory aligned on a 2**k boundary
  for modulo addressing, if possible, where 2**k >= size;
  otherwise, allocate aligned external data memory, if possible;
  otherwise, allocate unaligned external data memory, if possible;
  otherwise, allocate unaligned internal data memory, if possible
*/
EXPORT void *  memMallocAlignedIM (size_t size);

/*
  Free data memory allocated with mallocIM or mallocAlignedIM
*/
EXPORT void    memFreeIM   (void * memblock);

/*
  Check to see if address is in internal data memory
*/
EXPORT bool    memIsIM     (void * memblock);

    /* specific platform function */
EXPORT bool    memoryIsIM     (void * memblock);

/*******************************************************
* External memory management routines
*******************************************************/

/*
  Allocate external data memory, if possible;
  otherwise, allocate internal data memory
*/
EXPORT void *  memMallocEM (size_t size);

/*
  Reallocate external data memory, if possible;
  otherwise, reallocate internal data memory
*/
void * memReallocEM ( void *memblock, size_t size );

/*
  Allocate and clear external data memory, if possible;
  otherwise, allocate and clear internal data memory
*/
void *  memCallocEM (size_t num, size_t size);

/*
  Allocate external data memory aligned on a 2**k boundary
  for modulo addressing, if possible, where 2**k >= size;
  otherwise, allocate aligned internal data memory, if possible;
  otherwise, allocate unaligned external data memory, if possible;
  otherwise, allocate unaligned internal dat memory
*/
EXPORT void *  memMallocAlignedEM (size_t size);

/*
  Free data memory allocated with mallocEM or mallocAlignedEM
*/
EXPORT void    memFreeEM   (void * memblock);

/*
  Check to see if address is in internal data memory
*/
EXPORT bool    memIsEM     (void * memblock);

    /* specific platform function */
EXPORT bool    memoryIsEM( void * memblock );

/*******************************************************
* Memory management utility routines
*******************************************************/

/*
  Check to see if address is aligned on a 2**k boundary
  where 2**k >= size
*/
EXPORT bool    memIsAligned (void * memblock, size_t size);

/*
  Initialize internal and external memory partitions
*/
#pragma warn_padding off
typedef struct
{
  void *    partitionAddr;
  UInt16    partitionSize;
} mem_sPartition;

typedef struct
{
  UInt16                 EXbit;
  UInt16                 numExtPartitions;
  UInt16                 numIntPartitions;
  const mem_sPartition * intPartitionList;
  const mem_sPartition * extPartitionList;
} mem_sState;
#pragma warn_padding reset

EXPORT void    memInitialize (mem_sState *);


/*******************************************************
* Memory partition routines
*******************************************************/
/*
// You "own" the pool description, but we don't want to expose it's contents.
// We do, however, need to make sure that enough storage is allocated.
*/

typedef struct { UInt32 memPoolSpace[20]; } mem_sPool;

EXPORT void        memInitializePool(  mem_sPool * pMemPool,
                          void      * pMem,
                          size_t      Size,
                          bool        Prot,
                          bool        Assert
                        );
EXPORT void        memExtendPool    (  mem_sPool * pMemPool,
                          void      * pMem,
                          size_t      Size
                        );
EXPORT void        memProtect       (mem_sPool *);
EXPORT void      * memMallocWrapper (mem_sPool *, size_t, const char *, int);
EXPORT void      * memMalloc        (mem_sPool *, size_t);
EXPORT void      * memMallocAligned (mem_sPool *, size_t);
EXPORT void      * memCalloc        (mem_sPool *, size_t, size_t);
EXPORT void      * memRealloc       (mem_sPool *, void *, size_t);
EXPORT void        memFree          (mem_sPool *, void *);
EXPORT size_t      memCleanUp       (mem_sPool *);

#ifdef __cplusplus
}
#endif

#endif
