/* Author(s): <Your name here>
 * COS 318, Fall 2013: Project 3 Pre-emptive Scheduler
 * Implementation of the process scheduler for the kernel.
 */

#include "common.h"
#include "interrupt.h"
#include "queue.h"
#include "printf.h"
#include "scheduler.h"
#include "util.h"
#include "syslib.h"

pcb_t *current_running;
node_t ready_queue;
node_t sleep_wait_queue;
// more variables...
volatile uint64_t time_elapsed;
int sleep_num = 0;
//volatile uint32_t debug = 0;

/* TODO:wake up sleeping processes whose deadlines have passed */
void check_sleeping(){
     //node_t *pcb=NULL;
     pcb_t *temp = NULL;
     
     int i = 0;
     int wake_num = 0;

     if(is_empty(&sleep_wait_queue)) return;
     while(i < sleep_num){
     //while (pcb != &sleep_wait_queue){
          i++;
          temp = (pcb_t*)dequeue(&sleep_wait_queue);
          if (temp->deadline < time_elapsed){
               temp->status = READY;
               //dequeue(pcb->prev);
               enqueue(&ready_queue, (node_t*)temp);
               //break;
               wake_num++;
          } else {
               enqueue(&sleep_wait_queue, (node_t*)temp);
               //pcb = pcb->next;
               //enqueue(&ready_queue, temp);
          }
          sleep_num -= wake_num;
     }
}

/* Round-robin scheduling: Save current_running before preempting */
void put_current_running(){
     current_running->status = READY;
     enqueue(&ready_queue, (node_t *)current_running);
}

/* Change current_running to the next task */
void scheduler(){
     node_t *pcb;
     pcb_t *best, *temp;
     priority_t max_pri = 0;

     ASSERT(disable_count);
     check_sleeping(); // wake up sleeping processes

     while (is_empty(&ready_queue)){
          leave_critical();
          enter_critical();
          check_sleeping();
     }

     pcb = peek(&ready_queue);
     best = (pcb_t*)pcb;

     while(pcb != &ready_queue){
          temp = (pcb_t*)pcb;
          if(temp->priority > max_pri){
               best = temp;
               max_pri = temp->priority;
          }
          pcb = peek(pcb);
     }

     if(best->priority == 0){
     	  pcb = peek(&ready_queue);
          while(pcb != &ready_queue){
               temp = (pcb_t*)pcb;
               temp->priority = 10*(temp->pid);
               if(temp->priority > max_pri){
                    best = temp;
                    max_pri = temp->priority;
               }
               pcb = peek(pcb);
          }
     }
     
     best->priority = best->priority - 1;
     
     current_running = (pcb_t *)dequeue(((node_t*)best)->prev);

     /*
     while(pcb != &ready_queue){
          temp = (pcb_t*)pcb;
          if(temp->priority == 0){
               temp->priority = 10*(temp->pid);
          }
          if(temp->priority > max_pri){
               best = temp;
               max_pri = temp->priority;
          }
          pcb = peek(pcb);
     }

     best->priority = best->priority - 1;
     
     //if(best->priority > 1){
     //} else {
     //     best->priority = 10*(best->pid + 1);
     //}
     current_running = (pcb_t *)dequeue(((node_t*)best)->prev);
     */
     /*
     //while (pcb != &sleep_wait_queue){
          temp = (pcb_t*)dequeue(&sleep_wait_queue);
          if (temp->priority > time_elapsed){
               temp->status = READY;
               //dequeue(pcb->prev);
               enqueue(&ready_queue, (node_t*)temp);
               //break;
               wake_num++;
          
               //enqueue(&sleep_wait_queue, (node_t*)temp);
               pcb = peek(pcb);//->next;
               //enqueue(&ready_queue, temp);
          
     }
     */
     //current_running = (pcb_t *)best;//dequeue(&ready_queue);

     //print_char(debug, debug + 3, '9');
     ASSERT(NULL != current_running);
     ++current_running->entry_count;
     //print_char(debug, debug + 7,'6');
}

int lte_deadline(node_t *a, node_t *b) {
     pcb_t *x = (pcb_t *)a;
     pcb_t *y = (pcb_t *)b;

     if (x->deadline <= y->deadline) {
          return 1;
     } else {
          return 0;
     }
}

void do_sleep(int milliseconds){
     ASSERT(!disable_count);

     enter_critical();
     // TODO
     current_running->deadline = time_elapsed + milliseconds/1000;
     current_running->status = SLEEPING;
     enqueue(&sleep_wait_queue, (node_t *)current_running);//////////
     /*
     while (current_running->deadline >= time_elapsed){
     	break;
     }*/
     sleep_num++;
     scheduler_entry();
     //leave_critical();
}

void do_yield(){
     enter_critical();
     put_current_running();
     scheduler_entry();///////////////////////?leave_critical
     //leave_critical();
}

void do_exit(){
     enter_critical();
     current_running->status = EXITED;
     scheduler_entry();
     /* No need for leave_critical() since scheduler_entry() never returns */
}

void block(node_t * wait_queue){
     ASSERT(disable_count);
     current_running->status = BLOCKED;
     enqueue(wait_queue, (node_t *) current_running);
     scheduler_entry();
     //enter_critical();
}

void unblock(pcb_t * task){
     ASSERT(disable_count);
     task->status = READY;
     enqueue(&ready_queue, (node_t *) task);
}

pid_t do_getpid(){
     pid_t pid;
     enter_critical();
     pid = current_running->pid;
     leave_critical();
     return pid;
}

uint64_t do_gettimeofday(void){
     return time_elapsed;
}

priority_t do_getpriority(){
	/* TODO */
	priority_t priority;
	enter_critical();
	priority = current_running->priority;
	leave_critical();
	return priority;
}


void do_setpriority(priority_t priority){
	if (priority >= 1){
		enter_critical();
		current_running->priority = priority;
		leave_critical();
	}
}

uint64_t get_timer(void) {
     return do_gettimeofday();
}
