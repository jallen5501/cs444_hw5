/* timetest.c: get timer interrupts going */
#include <stdio.h>
#include <timer.h>
#include <pic.h>
#include <cpu.h>

#define LINELEN 100
#define MILLION 1000000
extern IntHandler irq0inthand; /* assembler envelope */
void irq0inthandc(void);	/* this is called from irq0inthand */
void set_timer_count(int count); /* local function, could be static */
int icount;

int main(void)
{
  int count;
  char buf[LINELEN];
  int i;

  printf("Enter startcount for timer (for example, 30000): ");
  fgets(buf,LINELEN,CONSOLE);	/* fgets takes dev# in SA lib vs. fd on UNIX */
  sscanf(buf, "%d" ,&count);
  cli();			/* disable ints while setting them up */
  printf("Setting IDT interrupt gate descriptor for irq 0 (int n = 0x20)\n");
  set_intr_gate(TIMER0_IRQ+IRQ_TO_INT_N_SHIFT, &irq0inthand);
  printf("Commanding PIC to interrupt CPU for irq 0 (TIMER0_IRQ)\n");
  pic_enable_irq(TIMER0_IRQ);
  printf("Commanding timer to generate pulse train using this count\n");
  set_timer_count(count);	
  printf("Enabling interrupts in CPU\n");
  icount = 0;
  sti();

  for (i=0;i<400*MILLION;i++)	/* 400 million loops on 400Mhz cpu: secs */
    ;

  printf("Shutting down\n");
  cli();
  printf("saw %d interrupts\n",icount);
  pic_disable_irq(TIMER0_IRQ);
  sti();
  return 0;
}

/* Set up timer to count down from given count, then pulse, repeatedly */
/* These output pulses follow wires to the PIC to provide timer interrupts */
/* count of 0 sets max count, 65536 = 2**16 */
void set_timer_count(int count)
{
  outpt(TIMER_CNTRL_PORT, TIMER0|TIMER_SET_ALL|TIMER_MODE_RATEGEN);
  outpt(TIMER0_COUNT_PORT,count&0xff); /* set LSB here */
  outpt(TIMER0_COUNT_PORT,count>>8); /* and MSB here */
}

/* timer interrupt handler */
void irq0inthandc(void)
{
  pic_end_int();	/* notify PIC with EOI command that its part is done */
  putchar('*');		/* just show it for this little test */
  icount++;
}
