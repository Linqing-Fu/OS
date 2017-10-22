/* scheduler.c */


#include "common.h"
#include "kernel.h"
#include "scheduler.h"
#include "util.h"
#include "queue.h"

int scheduler_count;
// process or thread runs time
uint64_t cpu_time;

void printstr(char *s);
void printnum(unsigned long long n);
void scheduler(void)
{
	++scheduler_count;

	// pop new pcb off ready queue
	/* need student add */ 
	
	current_running = queue_pop(ready_queue);
	while (!current_running){
		print_str(0, 0, "absense of tasks:\n");;
	}
	current_running->state = PROCESS_RUNNING;
		//print_str(0,0,"no task");
		//return;
	
}

void do_yield(void)
{
	save_pcb();
	/* push the qurrently running process on ready queue */
	/* need student add */
	current_running->state = PROCESS_READY;
	queue_push(ready_queue, (pcb_t*)current_running);

	// call scheduler_entry to start next task
	scheduler_entry();

	// should never reach here
	ASSERT(0);
}

void do_exit(void)
{
	/* need student add */
	current_running->state = PROCESS_EXITED;
	scheduler_entry();
}

void block(void)
{
	save_pcb();
	/* need student add */
	current_running->state = PROCESS_BLOCKED;
	queue_push(blocked_queue, (pcb_t*)current_running);
	scheduler_entry();

	// should never reach here
	ASSERT(0);
}

int unblock(void)
{
	/* need student add */
	
	
	pcb_t *unblocked = queue_pop(blocked_queue);
	if(unblocked){
		unblocked->state = PROCESS_READY;
		queue_push(ready_queue, unblocked);
	}

}

bool_t blocked_tasks(void)
{
	return !blocked_queue->isEmpty;
}
