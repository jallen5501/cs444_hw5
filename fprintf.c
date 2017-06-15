/* fprintf.c - fprintf */
#include "tunistd.h"
#define	OK	1
void tputc(int dev, char c);
void  _doprnt(const char *fmt, int *args, void (*)(int, char), int dev);
/* should really use stdio.h's prototype, but it has "..." */
int fprintf(int, const char *, int);

void tputc(int dev, char c)
{ 
   if (c == '\n')
       tputc(dev, '\r');	/* turn newline into CRLF */
   write(dev, &c,1);
}

/*------------------------------------------------------------------------
 *  fprintf  --  print a formatted message on specified device (file)
 *------------------------------------------------------------------------
 */
int fprintf(int dev, const char *fmt, int args)
{
        _doprnt(fmt, &args, tputc, dev);
        return(OK);
}





