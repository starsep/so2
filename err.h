#ifndef _ERR_
#define _ERR_

#define SYSERR(x) syserr(0, #x)

/* print system call error message and terminate */
extern void syserr(int bl, const char *fmt, ...);

/* print error message and terminate */
extern void fatal(const char *fmt, ...);

#endif
