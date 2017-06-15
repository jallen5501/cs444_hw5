/* sched.h, Scheduler API */
/* Note lack of encapsulation here: these functions and others all */
/*   access the proctab data.  */

/* Call this with interrupts off */
/* Find next runnable user process */
/* if none, select kernel (null) process */
void scheduler(int entry);

/* call this with interrupts off */
/* set current process to BLOCKED on specified event */
/* indicate event and call scheduler to find another process */
void sleep(int event);

/* wakeup all processes blocked on event */
void wakeup(int event);
