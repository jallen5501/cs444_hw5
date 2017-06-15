/* cpu.h */
#ifndef CPU_H
#define CPU_H

/* We use a "flat" segmentation model: a memory address has the same
   offset in all segments in use.  However, the Intel architecture
   still requires different segment descriptors for code and data,
   so we use descriptor 0x10 for code and 0x18 for all data segments,
   following the Linux setup.
*/

#define KERNEL_CS 0x10
#define KERNEL_DS 0x18
#define KERNEL_SS 0x18
#define KERNEL_ES 0x18

/* All segments have a base "linear address" of 0xc0000000, following
   the Linux kernel setup. (The Linux user process gets the 3G of
   linear address space below 0xc0000000, so that Linux can support
   up to 3G of user address space.)  Paging then maps the kernel
   linear address 0xc0000000 to physical address 0, etc., so the upshot
   is that offset x (address x) in the kernel simply refers to the xth
   byte of physical memory */

#define KERNEL_BASE_LA 0xc0000000

/* CR0: CPU master control register bits 
*                      (with normal values for SAPC in parentheses) 
*/
#define CR0_PG (1<<31)  /* paging enabled (1) */
#define CR0_CD (1<<30)  /* cache disable (0, i.e., cache on) */
#define CR0_NW (1<<29)  /* cache no-write-through (0) */
#define CR0_AM (1<<18)  /* allignment mask: enables bit AC in EFLAGS (1) */
#define CR0_WP (1<<16)  /* write protect effective even in super mode (1) */
#define CR0_NE (1<<5)   /* numerics exception (float) via int 16 (1) */
#define CR0_ET (1<<4)   /* extension type, always 1 on 486 */
#define CR0_TS (1<<3)   /* task switched: for 386 (0) */
#define CR0_EM (1<<2)   /* emulate math coprocressor: for 386 (0) */
#define CR0_MP (1<<1)   /* monitor coprocessor: for 386 (0) */
#define CR0_PE (1<<0)   /* protect enable: protected mode operation (1) */

/* an interrupt handler is a function, no params, void fn val */
typedef void IntHandler(void);

/* CPU-related Library functions */

/* write a byte to i/o port, like S&S, p. 169 except for width of args 
 * (because C "widens" function args to int-size, the outbyte can be 
 * of type int and fit this prototype).  port must be < 64K.
 */
void outpt(int port, unsigned char outbyte);

/* read a byte from i/o port */
unsigned char inpt(int port);

/* write the nth idt descriptor as an interrupt gate to inthand_addr */
/* We use an argument of type *pointer to* IntHandler here so we can 
   reestablish a saved interrupt-handler address (such a variable would need
   type pointer-to-function, and would not match a parameter type
   of IntHandler here--an obscure C gotcha.  */

void set_intr_gate(int n, IntHandler *inthand_addr);

/* --same action, and report progress to CONSOLE if debug */
void debug_set_intr_gate(int n, IntHandler *inthand_addr, int debug);

/* do CLI instruction, clear I bit in EFLAGS, to disable interrupts in CPU */
void cli(void);
/* do STI instruction, set I bit in EFLAGS, to enable interrupts in CPU */
void sti(void);
/* do INVD instruction: invalidate cache */
void invd(void);

/* get EFLAGS register */
unsigned int get_eflags(void);
/* set EFLAGS register */
void set_eflags(unsigned int new_eflags);

/* request location of the IDT from the CPU */
void locate_idt(unsigned int *limitp, char **idtpp);
/* request location of the GDT from the CPU */
void locate_gdt(unsigned int *limitp, char **gdtpp);
/* get CR0 register: cache, paging, floating point control */
unsigned int get_cr0(void);
/* set CR0 register */
void set_cr0(unsigned int new_CR0);
/* get CR2 register: page fault linear address */
unsigned int get_cr2(void);
/* get CR3 register: page directory base, flags */
unsigned int get_cr3(void);
/* set CR3 register: page directory base, flags */
unsigned int set_cr3(unsigned int new_CR3);

/* Turn on cache-disable bits in CR0 to disable i486's on-chip cache
 * Also, empty out both on-chip cache and external cache. */
void cpu_cache_off(void);
/* turn off cache-disable bits in CR0 to enable i486's CPU cache */
void cpu_cache_on(void);


#endif
