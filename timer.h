/* timer.h  Header for Programmable Interrupt Timer */
#ifndef TIMER_H
#define TIMER_H

#define 10ms 0X2E9D

/* Timer 0 is wired to cause an interrupt when its count reaches zero */
#define TIMER0_COUNT_PORT  0x40
/* and that interrupt is irq 0 in the PIC */
#define TIMER0_IRQ 0
/* Timer 1 is set at bootup to help with RAM refresh, best to leave alone*/
/* Timer 2 is wired to speaker, so we can control pitch of sound */
#define TIMER2_COUNT_PORT  0x42

/* for commands to timers
 * for example, to read the 16-bit counter in timer 0:
 * --output TIMER0|TIMER_LATCH to TIMER_CNTRL_PORT, then
 * --input a byte from TIMER_COUNT_PORT to get the low 8 bits of the count,
 * --input another byte from TIMER_COUNT_PORT to get the high bits.  
*/
#define TIMER_CNTRL_PORT 0x43
/* bits 6-7: which timer the command is for */
#define TIMER0 (0<<6)
#define TIMER1 (1<<6)
#define TIMER2 (2<<6)

/* bits 4-5: latch commands (take snapshot of counter and hold it for read) */
#define TIMER_LATCH (0<<4)	/* latch current count bits 0-15, avail in two
				   8-bit reads (low bits, then high bits) */
#define TIMER_SET_LOW (1<<4)	/* set bits 0-7 only */
#define TIMER_SET_HIGH (2<<4)	/* set bits 8-15 only */
#define TIMER_SET_ALL (3<<4)	/* set bits 0-15 (low bits, then high bits) */

/* bits 1-3: Mode select */
/* For the PC, the GATE input is wired high for timers 0 and 1, and to bit 0
 * of port 0x61 for timer 2, so only timer 2 can effectively use the _GATE 
 * modes at all.  The RATEGEN mode is best for timing. */
#define TIMER_MODE_1SHOT (0<<1)	     /* programmable one-shot, program init'd */
#define TIMER_MODE_1SHOT_GATE (1<<1) /* programmable one-shot, GATE input */
#define TIMER_MODE_RATEGEN (2<<1)    /* rate generator (pulse train) */
#define TIMER_MODE_SQUAREWAVE (3<<1) /* low for count, hi for count, low...*/
#define TIMER_MODE_STROBE (4<<1)     /* software-triggered pulse */
#define TIMER_MODE_STROBE_GATE (5<<1) /* GATE-triggered pulse */

/* bit 0: BCD select (BCD = binary coded decimal format for numbers) */
#define TIMER_BINARY_COUNTER 0
#define TIMER_BCD_COUNTER 1

#endif
