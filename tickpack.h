#include <cpu.h>

#define TEN_MS 0x2e9d
#define QUANTUM 5

extern int total_time;

/* Start ticking service */
void init_ticks(void);

/* Activate a timer */
int set_timer(IntHandler *function, int time, int reinittime);

/* Deactivate a timer */
int stop_timer(IntHandler *function);

/* Shut down ticking service */
void shutdown_ticks(void);

