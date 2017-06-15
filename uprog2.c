#include "tunistd.h"
#include "tty_public.h"

int main2() {
	int i = 0;
	for (;i < 1000000;i++)
		;
  write(TTY1,"bbbbbbbbbb",10);
  return 4;
}
