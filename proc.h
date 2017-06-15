/*********************************************************************
*
*       file:        proc.h
*                    Process structure info
*
*/
#ifndef PROC_H
#define PROC_H
/* number of processes: 3 user processes, 1 proc0 */
#define  NPROC 4

#define  N_SAVED_REGS 7
/* 7 non-scratch CPU registers, saved in this order: */
enum cpu_regs {SAVED_ESP, SAVED_EBX, SAVED_ESI, SAVED_EDI,
	       SAVED_EBP, SAVED_EFLAGS, SAVED_PC};

/* for p_status ( RUN=1, BLOCKED=2, ZOMBIE=3) */
typedef enum proc_status {RUN = 1, BLOCKED, ZOMBIE} ProcStatus;

/* for p_waitcode, what process is waiting for */
typedef enum proc_wait {TTY0_OUTPUT = 1, TTY1_OUTPUT, TTY0_INPUT, TTY1_INPUT} WaitCode;

/* Process Entry */
typedef struct {
  int p_savedregs[N_SAVED_REGS]; /* saved non-scratch registers */
  ProcStatus p_status;   /* RUN, BLOCKED, or ZOMBIE */
  WaitCode p_waitcode;   /* valid only if status=BLOCKED: TTY0_OUT, etc. */
  int p_exitval;         /* valid only if status=ZOMBIE */
	int p_quantum;
	int p_ticks;
	int enqueue_count;
	int p_number;	/***** JA temp testing member	*****/
} PEntry;

/* extern these globals here, define them in tunix.c */
extern PEntry proctab[], *curproc;
extern int process_entry_count[];
#endif /*PROC_H */
