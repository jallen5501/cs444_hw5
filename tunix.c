/* file: tunix.c core kernel code */

#include <stdio.h>
#include <cpu.h>
#include <gates.h>
#include <pic.h>
#include <timer.h>
#include "tsyscall.h"
#include "tsystm.h"
#include "proc.h"
#include "sched.h"
#include "tickpack.h"

/* for saved eflags register initialization */
#define EFLAGS_IF 0x200
/* for saved esp register--stack bases for user programs */
/* could use enum here */
#define STACKBASE1 0x3f0000
#define STACKBASE2 0x3e0000
#define STACKBASE3 0x3d0000
extern IntHandler syscall; /* the assembler envelope routine    */
extern void ustart1(void),ustart2(void), ustart3(void);
extern void finale(void);

void k_init(void);
void syscallc( int user_eax, int devcode, char *buff , int bufflen);
void debug_set_trap_gate(int n, IntHandler *inthand_addr, int debug);
void set_trap_gate(int n, IntHandler *inthand_addr);
void process0(void), init_proctab(void), shutdown(void);
int sysexit(int);

/* Record debug info in otherwise free memory between program and stack */
/* 0x300000 = 3M, the start of the last M of user memory on the SAPC */
#define DEBUG_AREA 0x300000
char *debug_log_area = (char *)DEBUG_AREA;
char *debug_record;  /* current pointer into log area */ 

/* kernel globals added for scheduler */
PEntry proctab[NPROC],*curproc; 
int number_of_zombie;
int process_entry_count[4];	/***** JA	count for preemptive scheduler ****/
#define MAX_CALL 6

/* syscall dispatch table */
static  struct sysent {
       short   sy_narg;        /* total number of arguments */
       int     (*sy_call)(int, ...);   /* handler */
} sysent[MAX_CALL];

/* end of kernel globals */


/****************************************************************************/
/* k_init: this function for the initialize  of the kernel system*/
/****************************************************************************/

void k_init(void){
	int i;
  debug_record = debug_log_area; /* clear debug log */
  strcpy(debug_record,"");

  cli();
  ioinit();            /* initialize the deivce */ 
  set_trap_gate(0x80, &syscall);   /* SET THE TRAP GATE*/

	/***** JA  turn on pit*****/
	init_ticks(); 
	/***** 			JA				*****/

	for (i=0; i < 4; i++)	
		process_entry_count[i]=0;

  /* Note: Could set these with initializers */
  /* Need to cast function pointer type to keep ANSI C happy */
  sysent[TREAD].sy_call = (int (*)(int, ...))sysread;
  sysent[TWRITE].sy_call = (int (*)(int, ...))syswrite;
  sysent[TEXIT].sy_call = (int (*)(int, ...))sysexit;
 
  sysent[TEXIT].sy_narg = 1;    /* set the arg number of function */
  sysent[TREAD].sy_narg = 3;
  sysent[TIOCTL].sy_narg = 3;
  sysent[TWRITE].sy_narg = 3;

  init_proctab();
  process0();			/* rest of kernel operation (non-init) */
}

/****************************************************************************/
/* process0:  code for process 0: runs when necessary, shuts down            */
/****************************************************************************/
void process0() {

  int i, j;

  /* execution will come back here when restore process 0*/
  while (number_of_zombie < NPROC-1) { /* proc 0 can't be zombie */     
    sti();			/* let proc 0 take interrupts (important!) */
    cli();
    scheduler(1); /***** JA 1 is for this call	*****/
  }
  kprintf("SHUTTING DOWN\n");
  sti();
  for (i=0; i< 1000000; i++)	/***** JA idle loop to make sure preemptive scheduler is working *****/
    ;				/* let output finish (kludge) */
   
  for(i=1;i<NPROC;i++) {
    kprintf("\nEXIT CODE OF PROCESS %d: %d\n", i, proctab[i].p_exitval);
  	kprintf("TICKS IN PROCESS %d: %d\n", i, proctab[i].p_ticks);
		kprintf("CHARACTERS ENQUEUED IN PROCESS %d: %d\n", i, proctab[i].enqueue_count);
	}
	kprintf("\nCALLS TO SCHEDULER:\n");
	kprintf("0 is sysexit 1 and 2 are in tunix.c and 3 is the preemptive scheduler\n");
	for (j=0; j<4; j++)
		kprintf("CALLS TO SCHEDULER %d: %d\n",j,process_entry_count[j]); 

  shutdown();
  /* note that we can return, in process0, to the startup module
     with its int $3.  It's OK to jump to finale, but not necessary */
}
/****************************************************************************/
/* init_proctab: this function for setting init_sp, init_pc              */
/* zeroing out savededp, and set to RUN                                     */
/****************************************************************************/
void init_proctab() {
  int i;

  proctab[1].p_savedregs[SAVED_PC] = (int)&ustart1;
  proctab[2].p_savedregs[SAVED_PC] = (int)&ustart2;
  proctab[3].p_savedregs[SAVED_PC] = (int)&ustart3;
 
  proctab[1].p_savedregs[SAVED_ESP] = STACKBASE1;
  proctab[2].p_savedregs[SAVED_ESP] = STACKBASE2;
  proctab[3].p_savedregs[SAVED_ESP] = STACKBASE3;

  for(i=0;i<NPROC;i++){
    proctab[i].p_savedregs[SAVED_EBP] = 0;
    proctab[i].p_savedregs[SAVED_EFLAGS] = EFLAGS_IF; /* make IF=1 */
    proctab[i].p_status=RUN;
		proctab[i].p_quantum = QUANTUM;
		proctab[i].p_ticks = 0;
		proctab[i].enqueue_count = 0;
		proctab[i].p_number = i;	
  }
  curproc=&proctab[0];
 
  number_of_zombie=0;
}

/* shut the system down */
void shutdown()
{


	/***** JA turn off the timer*****/
	shutdown_ticks();
	
	/*******************************/

  kprintf("SHUTTING THE SYSTEM DOWN!\n");
  kprintf("Debug log from run:\n");
  kprintf("Marking kernel events as follows:\n");
  kprintf("  ^a   COM2 input interrupt, a received\n");
  kprintf("  ~d   COM2 output interrupt, ordinary char output\n");
  kprintf("  ~e   COM2 output interrupt, echo output\n");
  kprintf("  ~s   COM2 output interrupt, shutdown TX ints\n");
  kprintf("  |(1z-2) process switch from 1, now a zombie, to 2\n");
  kprintf("  |(1b-2) process switch from 1, now blocked, to 2\n");
  kprintf("  |(2-1) process switch (by preemption) from 2 to 1\n");
  kprintf("%s", debug_log_area);	/* the debug log from memory */
  kprintf("\nLEAVE KERNEL!\n\n");
  finale();		/* trap to Tutor */
}

/****************************************************************************/
/* syscallc: this function for the C part of the 0x80 trap handler          */
/* OK to just switch on the system call number here                         */
/* By putting the return value of syswrite, etc. in user_eax, it gets       */
/* popped back in sysentry.s and returned to user in eax                    */
/****************************************************************************/

void syscallc( int user_eax, int devcode, char *buff , int bufflen){
  int nargs;
  int syscall_no = user_eax;

  switch(nargs = sysent[syscall_no].sy_narg)
    {
    case 1:         /* 1-argument system call */
	user_eax = sysent[syscall_no].sy_call(devcode);   /* sysexit */
	break;
    case 3:         /* 3-arg system call: calls sysread or syswrite */
	user_eax = sysent[syscall_no].sy_call(devcode,buff,bufflen); 
	break;
    default: kprintf("bad # syscall args %d, syscall #%d\n",
		     nargs, syscall_no);
    }
} 

/****************************************************************************/
/* sysexit: this function for the exit syscall function */
/****************************************************************************/

int sysexit(int exit_code)
{ 
  cli();
  curproc->p_exitval = exit_code;
  curproc->p_status = ZOMBIE;
  number_of_zombie++; 
  scheduler(2);/*****	JA 2 is a call to this scheduler *****/
  /* never returns */       
  return 0;    /* never happens, but avoids warning about no return value */
}

/****************************************************************************/
/* set_trap_gate: this function for setting the trap gate */
/****************************************************************************/

void set_trap_gate(int n, IntHandler *inthand_addr) {
  debug_set_trap_gate(n, inthand_addr, 0);
}

/* write the nth idt descriptor as a trap gate to inthand_addr */
void debug_set_trap_gate(int n, IntHandler *inthand_addr, int debug) {
  char *idt_addr;
  Gate_descriptor *idt, *desc;
  unsigned int limit = 0;
  extern void locate_idt(unsigned int *, char **);

  if (debug)
    kprintf("Calling locate_idt to do sidt instruction...\n");
  locate_idt(&limit,&idt_addr);
  /* convert to CS seg offset, i.e., ordinary address, then to typed pointer */
  idt = (Gate_descriptor *)(idt_addr - KERNEL_BASE_LA);
  if (debug)
    kprintf("Found idt at %x, lim %x\n",idt, limit);
  desc = &idt[n];               /* select nth descriptor in idt table */
  /* fill in descriptor */
  if (debug)
    kprintf("Filling in desc at %x with addr %x\n",(unsigned int)desc,
           (unsigned int)inthand_addr);
  desc->selector = KERNEL_CS;   /* CS seg selector for int. handler */
  desc->addr_hi = ((unsigned int)inthand_addr)>>16; /* CS seg offset of inthand */
  desc->addr_lo = ((unsigned int)inthand_addr)&0xffff;
  desc->flags = GATE_P|GATE_DPL_KERNEL|GATE_TRAPGATE; /* valid, trap */
  desc->zero = 0;
}

/* append msg to memory debugging log */
void debug_log(char *msg) {
    strcpy(debug_record, msg);
    debug_record +=strlen(msg);
}

