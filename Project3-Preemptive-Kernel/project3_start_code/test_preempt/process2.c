#include "printf.h"

void _start(void)
{
  int i;
  for(i=0; ;++i)
  {
  	//printf(i+8, i, "yes get here!");
    printf(3,1, "Process 2: %d", i);
  }
}
