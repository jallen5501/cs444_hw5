/* written by eoneil */
/* modified by kwn 3/02 */

#include <stdio.h>
#include "tsystm.h"
#include "proc.h"
#include "sched.h"
extern void asmswtch(PEntry *oldproc, PEntry *newproc);

/* for debug log report */
#define BUFLEN 100
/* in this file */
static void debug_report_process_switch(PEntry *oldprocp);

/* call this with interrupts off */
/* find next runnable user process */
/* if none, select kernel (null) process */
/* process switch done by asmswtch() */
/* Note that this attempt to be "fair" by remembering where
   we last left off doesn't randomize the output much in
   our case.  If three processes are blocked when wakeup is
   called, they all get marked RUN, and they all do run, A outputs,
   and calls sleep, B sees full-Q and sleeps, C sees full-q and
   sleeps, leaving A the "next" process to run and output again.
*/
void scheduler(int entry) {
  int i,k; 
  PEntry *oldprocp = curproc;
  static int olduproc=0;
  int nextuproc = olduproc==NPROC-1?1:olduproc+1;
	++process_entry_count[entry];
  /* loop from next user process around back to this one */
  for(i=nextuproc, k=0;k<NPROC-1;i==NPROC-1?i=1:i++,k++){
    if (proctab[i].p_status == RUN){
	olduproc = i;
	curproc=&proctab[i];	/* chosen to run */
	break;
    }
  }
  /* if all user processes are blocked, use process 0 */
  if (k==(NPROC-1)) {
      curproc = &proctab[0];
  }
  debug_report_process_switch(oldprocp);
  asmswtch(oldprocp,curproc);
}

/* Record a debug log entry for a process switch--
   put "|(1b-2)" in debug_log if process 1 blocks and 2 runs
   or "|(1z-2)" if process 1 is now a zombie and 2 runs,
   or "|(2-1)" if process 2 is switched to 1 by preemption, etc.
   but don't report on switches from process 0 back to itself
   because they happen too frequently.
*/
static void debug_report_process_switch(PEntry *oldprocp)
{
  char buf[BUFLEN] = "";

  switch (oldprocp->p_status) {
  case ZOMBIE:
      sprintf(buf, "|(%dz-%d)", oldprocp - proctab, curproc - proctab);
      break;
  case BLOCKED:
      sprintf(buf, "|(%db-%d)", oldprocp - proctab, curproc - proctab);
      break;
  case RUN:
      /* Suppress 0-0 reports for better readability */
      /* preemptions are reported here, |(1-2) for ex. */
      if (oldprocp != proctab || curproc != proctab)
	  sprintf(buf, "|(%d-%d)", oldprocp - proctab, curproc - proctab);
      break;
  default:
      kprintf("Unknown proc_status in debug_report_process_switch\n");
      break;
  }
  if (strlen(buf))		/* if buf has been filled, record it */
      debug_log(buf);  /* show process switch in log */
}
/* call this with interrupts off */
/* set current process to BLOCKED on specified event */
/* indicate event and call scheduler to find another process */
void sleep(int event)
{ 
  curproc->p_status = BLOCKED;
  curproc->p_waitcode = event;

  scheduler(0);	/***** JA 0 is for sleep *****/
}

/* wakeup all processes blocked on event */
void wakeup(int event)
{ 
    int k; 

    for(k = 1; k < NPROC; k++)
	if((proctab[k].p_status==BLOCKED) && 
	   (proctab[k].p_waitcode==event)){
	    proctab[k].p_status=RUN; /* wake up process */
	}
}





