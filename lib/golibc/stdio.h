#if (!defined(STDIO_H))

#define STDIO_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

#if (!defined(NULL))
	#define NULL	((void *) 0)
#endif

#include <stdarg.h>

/* golibc */
int sprintf(char *s, const char *format, ...);
int vsprintf(char *s, const char *format, va_list arg);

#if (defined(__cplusplus))
	}
#endif

#endif