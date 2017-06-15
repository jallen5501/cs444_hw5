/* tickpack.c 
 * Bob Wilson
 * 05/11/2003
 * A timer package for PC-Tutor
*/	/*****	JA starting with	*****/ 
#include <stdio.h>
#include <timer.h>
#include <pic.h>
#include <cpu.h>
#include "tickpack.h"
#include "proc.h"
#include "sched.h"
#include "tsystm.h"
extern IntHandler irq0inthand;
void smalldelay(void);
void set_timer_count(int);
void irq0inthandc(void);
void tick(void);
/* internal function prototypes */
/*
void set_timer_count(int);
*/
void init_ticks() {

/***** #define DEBUG	*****/

#ifdef DEBUG
  printf("Disabled interrupts in CPU while setting them up\n");
#endif
cli();	
#ifdef DEBUG  
  printf("Setting interrupt gate for timer, irq 0\n");
#endif
  /* irq 0 maps to slot n = 0x20 in IDT for linux setup */
  set_intr_gate(TIMER0_IRQ+IRQ_TO_INT_N_SHIFT, &irq0inthand);
#ifdef DEBUG
  printf("Commanding PIC to interrupt CPU for irq 0\n");
#endif
  pic_enable_irq(TIMER0_IRQ);
#ifdef DEBUG
  printf("Commanding timer to generate a pulse train using max count\n");
#endif
  set_timer_count(0X0);   
#ifdef DEBUG
  printf("Enabling interrupts in CPU\n");
#endif
sti();	
}

int set_timer(IntHandler *callback, int time, int reinit_time) {
  return 0;        /* Error: table full */
}

int stop_timer(IntHandler *function) {
  return 0;        /* Error: timer was not set */
}
void shutdown_ticks() {
  cli();
#ifdef DEBUG
  printf("Commanding PIC to shut off irq 0 to CPU\n");
#endif
  pic_disable_irq(TIMER0_IRQ);  /* disallow irq 0 ints to get to CPU */
  sti();
}

/* about 10 us on a SAPC (400Mhz Pentium) */
void smalldelay(void) {
  int i;
    
  for (i=0;i<1000;i++)
     ;
}

/* Set up timer to count down from given count, then send a tick interrupt, */
/* over and over. A count of 0 sets max count, 65536 = 2**16 */
void set_timer_count(int count) {
  outpt(TIMER_CNTRL_PORT, TIMER0|TIMER_SET_ALL|TIMER_MODE_RATEGEN);
  outpt(TIMER0_COUNT_PORT,count&0xff); /* set LSB here */
  outpt(TIMER0_COUNT_PORT,count>>8); /* and MSB here */
  smalldelay();                 /* give the timer a moment to init. */
}

/* timer interrupt handler */
void irq0inthandc(void) {		/***** JA *****/
	pic_end_int();
	tick();
}

void tick() {								/***** JA *****/
	char *number;
	number[0]='*'; 
	number[2]='\0';

	++curproc->p_ticks;

	switch (curproc->p_number){
		case 1:
			number[1]='1';
			break;
		case 2:
			number[1]='2';
			break;
	}
	if (curproc->p_number == 1 || curproc->p_number == 2)
		debug_log(number);

#ifdef DEBUG
/*****	kprintf("* ");	JA	used to testing *****/
#endif
	if(--curproc->p_quantum == 0) {
		/* preempting, start over with full quantum */ 
		curproc->p_quantum = QUANTUM; 
#ifdef DEBUG				/***** JA *****/
		kprintf("PREEMPTIVE SCHEDULER BEING CALLED\n");
		kprintf("Leavins process %d\n", curproc->process_number);
#endif
		scheduler(3); /**** JA 3 is for preemptive scheduler call to scheduler */
	}
}
