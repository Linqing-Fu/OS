/*
   kernel.c
   the start of kernel
   */

#include "common.h"
#include "kernel.h"
#include "scheduler.h"
#include "th.h"
#include "util.h"
#include "queue.h"

#include "tasks.c"

volatile pcb_t *current_running;

queue_t ready_queue, blocked_queue;
struct queue R_Queue, B_Queue;
struct pcb PCBs[NUM_TASKS];
pcb_t *ready_arr[NUM_TASKS];
pcb_t *blocked_arr[NUM_TASKS];

/*
   this function is the entry point for the kernel
   It must be the first function in the file
   */

#define PORT3f8 0xbfe48000

void printnum(unsigned long long n)
{
  int i,j;
  unsigned char a[40];
  unsigned long port = PORT3f8;
  i=10000;
  while(i--);

  i = 0;
  do {
  a[i] = n % 16;
  n = n / 16;
  i++;
  }while(n);

  for (j=i-1;j>=0;j--) {
   if (a[j]>=10) {
      *(unsigned char*)port = 'a' + a[j] - 10;
    }else{
      *(unsigned char*)port = '0' + a[j];
    }
  }
  printstr("\r\n");
}


void _stat(void){
  /* some scheduler queue initialize */
  
  ready_queue = &R_Queue;
  queue_init(ready_queue);
  ready_queue->pcbs = ready_arr;
  ready_queue->capacity = NUM_TASKS;

  blocked_queue = &B_Queue;
  queue_init(blocked_queue);
  blocked_queue->pcbs = blocked_arr;
  blocked_queue->capacity = NUM_TASKS;

  
	clear_screen(0, 0, 45, 40);

	/* Initialize the PCBs and the ready queue */
	/* need student add */
  int i;
  int stack_top = STACK_MIN;
  for(i = 0; i < NUM_TASKS; i++){
    //PCBs[i].sz = STACK_SIZE;
    //PCBs[i].kstack = STACK_SIZE - (STACK_SIZE * (i+1));
    PCBs[i].state = PROCESS_READY;
    PCBs[i].pid = i;   //process ID
    stack_top += STACK_SIZE;
    PCBs[i].s0 = 0;
    PCBs[i].s1 = 0;
    PCBs[i].s2 = 0;
    PCBs[i].s3 = 0;
    PCBs[i].s4 = 0;
    PCBs[i].s5 = 0;
    PCBs[i].s6 = 0;
    PCBs[i].s7 = 0;
    PCBs[i].sp = stack_top;
    PCBs[i].fp = stack_top;
    PCBs[i].ra = task[i]->entry_point;
    
    queue_push(ready_queue,PCBs+i);
    //PCBs[i].context = (struct context*)(STACK_MAX - (STACK_SIZE * i) - sizeof(struct context));
  }
	/*Schedule the first task */
	scheduler_count = 0;
	scheduler_entry();

	/*We shouldn't ever get here */
	ASSERT(0);
}
