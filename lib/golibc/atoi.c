#include <stdlib.h>	/* strtol */
#include <stdio.h>	/* NULL */

int atoi(const char *s)
{
	return (int) strtol(s, (const char **) NULL, 10);
}
