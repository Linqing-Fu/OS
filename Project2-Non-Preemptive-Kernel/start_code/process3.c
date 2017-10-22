#include "common.h"
#include "syslib.h"
#include "util.h"


//int flag = 1;

void _start(void)
{
	int i;
	
	//start_ticks = get_timer();
	for(i=0; i < 9999; i++){
		yield();
	}
	exit();
/*
	//reference th1.c
	for (i = 0; i < 1000; ++i){
		ticks = get_timer();
		ticks = (ticks - start_ticks);
		time = ((unsigned int) ticks) / MHZ;

		print_str( 0, 2, prostr);
		print_int( 10, 2, time/2);
		
		yield();
	}
	exit();
	*/

}
