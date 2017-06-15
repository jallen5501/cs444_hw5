/* uprog123.c Output test, from hw3 uprog1.c, uprog2.c, uprog3.c */
#include <stdio.h>
#include "tunistd.h"
#include "tty_public.h"

#define MILLION 1000000
#define DELAY (400 * MILLION)
int main1(void);
int main2(void);
int main3(void);

int main1()
{
  int i;

  write(TTY1,"aaaaaaaaaa",10);
  write(TTY1, "zzz",3);
  for (i=0;i<DELAY;i++)	/* enough time to drain output q */
    ;
  write(TTY1,"AAAAAAAAAA",10);	/* see it start output again */
  return 2;
}

int main2() {
	int i = 0;
  write(TTY1,"bbbbbbbbbb",10);
  return 4;
}

int main3() {
  write(TTY1,"cccccccccc",10);
  return 6;
}
