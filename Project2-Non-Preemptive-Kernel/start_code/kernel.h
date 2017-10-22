/* kernel.h: definitions used by kernel code */

#ifndef KERNEL_H
#define KERNEL_H

#define NUM_REGISTERS 8

#include "common.h"

/* ENTRY_POINT points to a location that holds a pointer to kernel_entry */
#define ENTRY_POINT ((void (**)(int)) 0xa080fff8)

/* System call numbers */
enum {
    SYSCALL_YIELD,
    SYSCALL_EXIT,
};

/* All stacks should be STACK_SIZE bytes large
 * The first stack should be placed at location STACK_MIN
 * Only memory below STACK_MAX should be used for stacks
 */
enum {
    STACK_MIN = 0xa0880000,
    STACK_SIZE = 0x1000,
    STACK_MAX = 0xa0890000,
};

typedef enum {
	PROCESS_BLOCKED,
	PROCESS_READY,
	PROCESS_RUNNING,
	PROCESS_EXITED,
}process_state;
/*
struct context {
	uint32_t s0;
	uint32_t s1;
	uint32_t s2;
	uint32_t s3;
	uint32_t s4;
	uint32_t s5;
	uint32_t s6;
	uint32_t s7;
	uint32_t sp;
	uint32_t s8;
	uint32_t ra;
};


typedef struct pcb {
	//char *mem;               //start of process memory(kernel address)
	struct context *context; //switch here to run process :edi esi ebx ebp eip
	process_state state;      //process state
	int pid;        //process ID
	
} pcb_t;
*/

typedef struct pcb {
	
    int pid;//program ID
    process_state state; //program state
    uint32_t s0;
    uint32_t s1;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t fp;//frame pointer
    uint32_t sp;//stack pointer
    uint32_t ra;// return address
} pcb_t;

/* The task currently running.  Accessed by scheduler.c and by entry.s assembly methods */
extern volatile pcb_t *current_running;

void kernel_entry(int fn);

#endif                          /* KERNEL_H */
