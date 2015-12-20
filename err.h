#ifndef _ERR_
#define _ERR_

#define TRY(x) \
	if ((x) == -1) { \
		fatal(#x); \
	}

/* print error message and terminate */
extern void fatal(const char *fmt, ...);


#endif
