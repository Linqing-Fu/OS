/* Just "flies a plane" over the screen. */
#include "common.h"
#include "syslib.h"
#include "util.h"
#include "printf.h"


void __attribute__((section(".entry_function"))) _start(void)
{
  pid_t myPid = getpid();
  pid_t p1_pid = 2;
  int myRow = myPid / 2;
  int myCol = 40* (myPid % 2);
  int i;
  printf(myRow, myCol, "mypid %d",
      myPid);
  kill(p1_pid);
  printf(myRow, myCol, "kill p1 successfully");
  mylock_acq(1);
  printf(myRow+1, myCol, "acquire lock successfully");
  mylock_real(1);
  while (1){
  	printf(50,10, "1");
  }
  exit();
}