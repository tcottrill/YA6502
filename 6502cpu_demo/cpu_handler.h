
#ifndef CPU_HANDLER_H
#define CPU_HANDLER_H

#ifndef UINT32
#define UINT32  unsigned int
#endif

#ifndef UINT16
#define UINT16  unsigned short int
#endif

#ifndef UINT8
#define UINT8   unsigned char
#endif


#ifndef _MEMORYREADWRITEBYTE_
#define _MEMORYREADWRITEBYTE_
#endif

struct MemoryWriteByte
{
	unsigned int lowAddr;
	unsigned int highAddr;
	void(*memoryCall)(unsigned int, unsigned char, struct MemoryWriteByte *);
	void *pUserArea;
};

struct MemoryReadByte
{
	unsigned int lowAddr;
	unsigned int highAddr;
	unsigned char(*memoryCall)(unsigned int, struct MemoryReadByte *);
	void *pUserArea;
};

#endif