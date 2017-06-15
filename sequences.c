/* sequences.c: two processes computing sequences (CPU-bound) */
#include <stdio.h>
#include "tunistd.h"
#include "tty_public.h"
#include "sequence.h"

#define MILLION 1000000
#define DELAY (400 * MILLION)
int main1(void);
int main2(void);
int main3(void);

int main1()
{
  int i;

  write(TTY1,"aaaaaaaaaa",10);
  fprintf(TTY1, "zzz");
  for (i=0;i<DELAY;i++)	/* enough time to drain output q */
    ;
  write(TTY1,"AAAAAAAAAA",10);	/* see it start output again */
  return 2;
}


/* find the number that starts the longest sequence defined by:    */
/* n -> 3n + 1 if n odd                                            */
/* n -> n / 2 if n even                                            */
/*  stop search at 'last'                                          */
/* kwn 4/02 */
/* find the number that starts the longest sequence defined by:    */
/* n -> 3n + 1 if n odd                                            */
/* n -> n / 2 if n even                                            */
/*  stop search at 'last'                                          */

/* kwn 4/02 */

#define LASTSTART2 500000
#define LASTSTART3 500000

int main2()
{
  int start, current, length, max_seq_start;
  int max_length = 1; 
  int last = LASTSTART2;

  for(start=first(), max_length = 1;start<=last;start += 2){
    for (current = start, length = 0; current > 4;
	 length++, current = next(current))
      if (current < 0){
	fprintf(TTY1, "ERROR: overflow checking %d\n", start);
	return 3;
      }
    if (length > max_length){
      max_length = length;
      max_seq_start = start;
    }
}
  fprintf(TTY1, "main2: Tested sequences through %d.\n", last);
  fprintf(TTY1, "main2: Longest sequence of length %d started at %d.\n", 
	 max_length, max_seq_start);
  return 1;
}

/* check if every sequence that starts up to 'last' terminates     */
/* sequence defined by:                                            */
/* n -> 3n + 1 if n odd                                            */
/* n -> n / 2 if n even                                            */

/* kwn 4/02 */

int main3()
{
  int start, current;
  int last = LASTSTART3;
	
  for(start=first();start<=last;start += 2 )
 /* only need to check odd numbers */
    for (current = start; current >= start;  
	 /* by recursion can stop when current < start */
	 current = next(current))
      if (current < 0){
	fprintf(TTY1, "ERROR: overflow checking %d\n", start);
	return 3;
      }

  fprintf(TTY1,"main3: Tested all sequences that start at less than %d.\n", last);
  return 1;
}

/******************************************************************/
/* Rest of file:  sequence generator                              */
/* Note: we could put this in another file, but for simplicity we */
/*   want just one file of user C code for hw5.                   */
/******************************************************************/

int first(void){
  return 3;
}

/* sequence defined by:                                            */
/* n -> 3n + 1 if n odd                                            */
/* n -> n / 2 if n even                                            */

int next(int current){
  return (current & 1 ) ? 3 * current + 1 : current >> 1;
}

