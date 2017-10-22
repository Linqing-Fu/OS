#include "common.h"
#include "scheduler.h"
#include "util.h"

//uint64_t time;
char *string = "thread: ";
char *prostr = "process: ";
uint32_t th_to_th_time = 0;
uint32_t th_to_pro_time = 0;


void thread4(void)
{
	int i;
	unsigned int time;

	th_to_th_time = get_timer();

	for (i = 0; i < 10000; ++i){
		th_to_pro_time = get_timer();
		th_to_pro_time = (th_to_pro_time - th_to_th_time);
		time = ((unsigned int) th_to_pro_time) / MHZ;

		print_str( 0, 2, prostr);
		print_int( 10, 2, (i==0 || i==1)?0:time/2);

		th_to_th_time = get_timer();
		do_yield();
	}
	do_exit();
}

void thread5(void)
{
	th_to_th_time = get_timer() - th_to_th_time;
	print_str(0, 0, string);
	print_int(10, 0, th_to_th_time/MHZ);
	do_exit();
}

