/* pic.h: header for the PC's programmable interrupt controller */
#ifndef PIC_H
#define PIC_H

/* the master handles irqs 0-7, with 2 reserved for cascading to the
   slave.  The slave handles irqs 8-15 */
#define PIC_MASTER0 0x20
#define PIC_MASTER1 0x21
#define PIC_SLAVE0 0xa0
#define PIC_SLAVE1 0xa1

/* Here we disregard PIC initialization, depending on the Linux
   code in Tutor to initialize it properly on bootup.  Linux init's
   the PIC to use int numbers 0x20-0x2f for irqs 0-0xf */
#define IRQ_TO_INT_N_SHIFT 0x20

/* OCW1: operations cmd. word 1, read/write to 0x21/0xa1: 
   one bit for each irq, 1 to disable that irq, 0 to enable it.
   See the library routines pic_enable/disable_irq described below. */

/* OCW2 and OCW3, operations cmd words 2 and 3:  these are multiplexed 
   into port 0x20/0xa0.  An 8-bit write to port 0x20/0xa0 writes to OCW2
   or OCW3 depending on the value of its bit 3 as follows. */
#define OCW2 (0<<3)
#define OCW3 (1<<3)

/* OCW2: most important is EOI, end-of-interrupt command to PIC.  This
 tells the PIC to forget about its currently highest-priority in-service
 interrupt. See the library routine pic_end_int() or pic_end_int2()
 described below. */
#define PIC_EOI (1<<5)

/* OCW3: Ways to get more info from the PIC, of possible debugging use.
   Use one of these commands, then read 0x20/0xa0 to get the PIC's IRR 
   (interrupt request register) or ISR (int. service register).
   for ex., write OCW3|PIC_READ_IRR to 0x20/0xa0, then read 0x20/0xa0 
   to get the IRR, bits for whether or not each IRQ has an active request */
#define PIC_READ_IRR 2
/* ISR: Whether or not each IRQ is currently being serviced, i.e, the IRQ's 
   interrupt has been reported to the CPU but not yet EOI'd */
#define PIC_READ_ISR 3

/* Library functions */
/* Command PIC to let signals for a specified IRQ get through to CPU.
   Works for irqs 0-15, except 2, which is reserved for cascading to
   the slave chip. */
void pic_enable_irq(int irq);

/* Command PIC to stop signals on line irq from reaching CPU. */
void pic_disable_irq(int irq);

/* Tell PIC that this (highest-current-priority) interrupt is being
   handled, so it can go on to another.  pic_end_int() works for irqs 0-7.
   Above that, you need to tell *both* MASTER and SLAVE chips, by 
   using pic_end_int2(). Be sure to use one of these exactly once
   for each interrupt.  */
void pic_end_int(void);
void pic_end_int2(void);
#endif



